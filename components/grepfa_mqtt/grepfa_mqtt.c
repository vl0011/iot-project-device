//
// Created by vl0011 on 23. 6. 1.
//

static const char* TAG = "grepfa_mqtt";

#include <grepfa_mqtt_libconfig.h>
#include <grepfa_mqtt.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_log.h"
#include "grepfa_uuid.h"
#include "mqtt_client.h"
#include "grepfa_mqtt_event.h"
#include <esp_random.h>


#ifdef GREPFA_MQTT_CONFIGURATION_CERTIFICATION_FROM_ASM
extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");

extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");

extern const char root_cert_auth_start[] asm("_binary_root_cert_auth_crt_start");
extern const char root_cert_auth_end[]   asm("_binary_root_cert_auth_crt_end");
#endif

static uint64_t GrepfaMqttDeviceGetHash(const void* item, uint64_t seed0, uint64_t seed1) {
    const GrepfaDeviceV1_t *dev = item;
    return hashmap_sip(dev->id, UUID_STR_LEN, seed0, seed1);
}

static int GrepfaMqttDeviceCompare(const void *a, const void *b, void *udata) {
    const GrepfaDeviceV1_t *ua = a;
    const GrepfaDeviceV1_t *ub = b;
    return strcmp(ua->id, ub->id);
}


static void mqtt_event_handler(void *args, esp_event_base_t base, int32_t event_id, void *data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)data;
    GrepfaMqttConnectorV1_t* it = (GrepfaMqttConnectorV1_t*) args;

    ESP_LOGI(TAG, "server id [%s] event occurs", it->id);

    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            mqtt_event_router_error(it, event);
            break;

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            if(it->connectWaitSem) {
                xSemaphoreGive(it->connectWaitSem);
            }
//            GrepfaMqttConnectorPublish(it, GREPFA_MQTT_CONFIGURATION_LOG_TOPIC, 1, "hello", 0);
            mqtt_event_router_connected(it, event);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_event_router_disconnected(it, event);

            if(it->connectWaitSem) {
                vSemaphoreDelete(it->connectWaitSem);
                it->connectWaitSem = NULL;
            }

            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
            mqtt_event_router_subscribe(it, event);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
            mqtt_event_router_unsubscribe(it, event);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
            mqtt_event_router_published(it, event);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "topic: %.*s ->\n%.*s", event->topic_len, event->topic, event->data_len, event->data);
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            mqtt_event_router_data(it, event);
            break;

        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            mqtt_event_router_connected(it, event);
            break;

        case MQTT_EVENT_DELETED:
            ESP_LOGI(TAG, "MQTT_EVENT_DELETED");
            mqtt_event_router_deleted(it, event);
            break;

        default:
            ESP_LOGI(TAG, "UNKNOWN_EVENT");
            mqtt_event_router_unknown(it, event);

    }
}


GrepfaMqttConnectorV1_t * GrepfaMqttConnectorNew(const char* endpoint, const char *id, bool wait) {
    const esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = endpoint,
            .broker.verification.certificate = (const char *)root_cert_auth_start,
            .credentials = {
                    .authentication = {
                            .certificate = (const char *)client_cert_pem_start,
                            .key = (const char *)client_key_pem_start,
                    },
            }
    };
    GrepfaMqttConnectorV1_t* ret = malloc(sizeof (GrepfaMqttConnectorV1_t));
    memset(ret, 0, sizeof (GrepfaMqttConnectorV1_t));
    strncpy(ret->id, id, UUID_STR_LEN);
    ret->mqttClient = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(ret->mqttClient, MQTT_EVENT_ANY, mqtt_event_handler, ret);
    esp_mqtt_client_start(ret->mqttClient);
    ESP_LOGI(TAG, "mqtt client start...");

    if (wait) {
        ESP_LOGI(TAG, "waiting mqtt client connect...");
        ret->connectWaitSem = xSemaphoreCreateBinary();
        if (ret->connectWaitSem == NULL) {
            ESP_LOGE(TAG, "ESP_ERR_NO_MEM");
            free(ret);
            return NULL;
        }
        xSemaphoreTake(ret->connectWaitSem, portMAX_DELAY);
    }

    ESP_LOGI(TAG, "create device hashmap...");
    ret->devices = hashmap_new(sizeof(GrepfaDeviceV1_t), 0, esp_random(), esp_random(), GrepfaMqttDeviceGetHash, GrepfaMqttDeviceCompare, NULL, NULL);

    return ret;
}

void GrepfaMqttConnectorAddDevice(GrepfaMqttConnectorV1_t *client, GrepfaDeviceV1_t* device, int qos) {
    char buf[255];
    strcpy(buf, GREPFA_MQTT_CONFIGURATION_ROOT_TOPIC);
    strcat(buf, "actuator/");
    strcat(buf, device->id);

    ESP_LOGI(TAG, "subscribed: %s", buf);
    esp_mqtt_client_subscribe(client->mqttClient, buf, qos);
    hashmap_set(client->devices, device);
}

void GrepfaMqttConnectorPublish(GrepfaMqttConnectorV1_t *client, GrepfaDeviceV1_t* device, const GrepfaPayloadData_t* data, int qos) {
    char topicBuf[255];
    strcpy(topicBuf, GREPFA_MQTT_CONFIGURATION_ROOT_TOPIC);
    strcat(topicBuf, "sensor/");
    strcat(topicBuf, device->id);

    char* dataStr = GrepfaPayloadDataToJson(data);

    ESP_LOGI(TAG, "data string created");

    ESP_LOGI(TAG, "%s:%s", topicBuf, dataStr);

    int ret = esp_mqtt_client_publish(client->mqttClient, topicBuf, dataStr, 0, qos, 0);

    ESP_LOGI(TAG, "data published");

    DeleteGrepfaPayloadDataJsonStr(dataStr);

    if (ret == -1) {
        ESP_LOGW(TAG, "%s - publish error", topicBuf);
    } else {
        ESP_LOGI(TAG, "%s - publish success", topicBuf);
    }
}

//void GrepfaMqttConnectorPublishAsync(GrepfaMqttConnectorV1_t *client, char *topic, int qos, const char *dataStr,
//                                     int dataSizeIfDataIsBinary, bool store) {
//    esp_mqtt_client_enqueue(client->mqttClient, topic, dataStr, dataSizeIfDataIsBinary, qos, 0, store);
//}

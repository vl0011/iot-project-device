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


#ifdef GREPFA_MQTT_CONFIGURATION_CERTIFICATION_FROM_ASM
extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");

extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");

extern const char root_cert_auth_start[] asm("_binary_root_cert_auth_crt_start");
extern const char root_cert_auth_end[]   asm("_binary_root_cert_auth_crt_end");
#endif





void mqtt_event_handler(void *args, esp_event_base_t base, int32_t event_id, void *data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)data;
    GrepfaMqttConnectorV1_t* it = (GrepfaMqttConnectorV1_t*) args;

    ESP_LOGI(TAG, "server id [%s] event occurs", it->id);

    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if(it->mqtt_event_router_error != NULL)
                it->mqtt_event_router_error(it, event);
            break;
        case MQTT_EVENT_CONNECTED:
            it->isConnected = true;
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            if(it->mqtt_event_router_connected != NULL)
                it->mqtt_event_router_connected(it, event);
            break;
        case MQTT_EVENT_DISCONNECTED:
            it->isConnected = false;
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            if(it->mqtt_event_router_disconnected != NULL)
                it->mqtt_event_router_disconnected(it, event);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
            if(it->mqtt_event_router_subscribe != NULL)
                it->mqtt_event_router_subscribe(it, event);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
            if(it->mqtt_event_router_unsubscribe != NULL)
                it->mqtt_event_router_unsubscribe(it, event);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
            if(it->mqtt_event_router_published != NULL)
                it->mqtt_event_router_published(it, event);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            if(it->mqtt_event_router_data != NULL)
                it->mqtt_event_router_data(it, event);
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            if(it->mqtt_event_router_connected != NULL)
                it->mqtt_event_router_connected(it, event);
            break;
        case MQTT_EVENT_DELETED:
            ESP_LOGI(TAG, "MQTT_EVENT_DELETED");
//            if(it->mqtt_event_router_deleted != NULL)
//            it->mqtt_event_router_deleted(it, event);
            break;
        default:
            ESP_LOGI(TAG, "UNKNOWN_EVENT");
//            if(it->mqtt_event_router_unknown != NULL)
//            it->mqtt_event_router_unknown(it, event);

    }
}


GrepfaMqttConnectorV1_t * GrepfaMqttConnectorNew(const char* endpoint, const char *id) {
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

    return ret;
}
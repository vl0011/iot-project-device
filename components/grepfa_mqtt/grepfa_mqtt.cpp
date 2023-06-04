//
// Created by vl0011 on 23. 6. 1.
//

const char* TAG = "grepfa_mqtt";

#include <grepfa_mqtt_libconfig.h>
#include <grepfa_mqtt.h>

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "grepfa_uuid.h"
#include "mqtt_client.h"

GrepfaMQTTConnectorV1::GrepfaMQTTConnectorV1(const char *endpoint, const char *id) {
    strncpy(this->id, id, 255);

    cfg.broker.address.uri = endpoint;
    clientHandle = esp_mqtt_client_init(&cfg);

    esp_mqtt_client_register_event(clientHandle, MQTT_EVENT_ANY, GrepfaMQTTConnectorV1::mqtt_event_handler, this);

    esp_mqtt_client_start(clientHandle);
}

void GrepfaMQTTConnectorV1::mqtt_event_handler(void *args, esp_event_base_t base, int32_t event_id, void *data) {
    auto event = (esp_mqtt_event_handle_t)data;
    auto it = (GrepfaMQTTConnectorV1*) args;
    esp_mqtt_client_handle_t client = event->client;

    ESP_LOGI(TAG, "server id [%s] event occurs", it->id);

    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            it->mqtt_event_router_error(it, event);
            break;
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            it->mqtt_event_router_connected(it, event);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            it->mqtt_event_router_disconnected(it, event);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
            it->mqtt_event_router_subscribe(it, event);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
            it->mqtt_event_router_unsubscribe(it, event);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
            it->mqtt_event_router_published(it, event);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            it->mqtt_event_router_data(it, event);
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            it->mqtt_event_router_connected(it, event);
            break;
        case MQTT_EVENT_DELETED:
            ESP_LOGI(TAG, "MQTT_EVENT_DELETED");
//            it->mqtt_event_router_deleted(it, event);
            break;
        default:
            ESP_LOGI(TAG, "UNKNOWN_EVENT");
//            it->mqtt_event_router_unknown(it, event);

    }
}


void GrepfaMQTTConnectorV1::mqtt_event_router_error(GrepfaMQTTConnectorV1 *it, esp_mqtt_event_handle_t ev_handler) {
    auto err_type = ev_handler->error_handle->error_type;

    switch (err_type) {

        case MQTT_ERROR_TYPE_NONE:
            break;
        case MQTT_ERROR_TYPE_TCP_TRANSPORT:
            // ev_handler->error_handle->esp_tls_...
            break;
        case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
            break;
        case MQTT_ERROR_TYPE_SUBSCRIBE_FAILED:
            break;
    }
}

void GrepfaMQTTConnectorV1::AddDevice(GrepfaDeviceV1 *device) {
    contains.insert(std::make_pair(device->getId(), device));
}


const char *GrepfaDeviceV1::getId() const {
    return id;
}

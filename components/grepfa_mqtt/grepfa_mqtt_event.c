//
// Created by vl0011 on 23. 6. 8.
//
static const char* TAG = "grepfa_mqtt_event";

#include <grepfa_mqtt_event.h>
#include <grepfa_payload_v1.h>
#include <hashmap.h>
#include <esp_log.h>

void mqtt_event_router_data(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle) {

    ESP_LOGI(TAG, "payload parsing...");

    GrepfaPayloadData_t* payload = JsonToGrepfaPayloadData(handle->data, handle->data_len);
    if (payload == NULL) {
        ESP_LOGI(TAG, "payload error");
        return;
    }

    ESP_LOGI(TAG, "device id: %s finding", payload->things_id);

    GrepfaDeviceV1_t temp;
    strcpy(temp.id, payload->things_id);
    const GrepfaDeviceV1_t* dev = hashmap_get(client->devices, &temp);

    if (dev == NULL) {
        ESP_LOGW(TAG, "device not found");
        return;
    }
    if(dev->inboundCallback == NULL) {
        ESP_LOGW(TAG, "event callback not set");
        return;
    }

    dev->inboundCallback(payload);
}

void mqtt_event_router_error(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_connected(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_disconnected(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_subscribe(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_unsubscribe(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_published(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_deleted(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

void mqtt_event_router_unknown(GrepfaMqttConnectorV1_t *client, esp_mqtt_event_handle_t handle) {}

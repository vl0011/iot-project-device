//
// Created by vl0011 on 23. 6. 8.
//

#ifndef P1_TYPES_H
#define P1_TYPES_H

#include <grepfa_mqtt_libconfig.h>
#include <mqtt_client.h>

typedef struct grepfa_device_payload_v1{
    uint8_t channel;
    char type[GREPFA_MQTT_CONFIGURATION_PAYLOAD_TYPE_LEN];
    char value[GREPFA_MQTT_CONFIGURATION_PAYLOAD_VALUE_LEN];
} GrepfaDevicePayloadV1_t;

typedef struct grepfa_mqtt_connector_v1 GrepfaMqttConnectorV1_t;
typedef void(*GrepfaMqttV1Event_t) (GrepfaMqttConnectorV1_t*, esp_mqtt_event_handle_t);
typedef void (*GrepfaMqttDeviceGetMessageEventHandler_t) (GrepfaDevicePayloadV1_t* payload, int num);

struct grepfa_mqtt_connector_v1{
    struct hashmap * devices;
    char id[UUID_STR_LEN];
    esp_mqtt_client_handle_t mqttClient;

    GrepfaMqttV1Event_t mqtt_event_router_error;
    GrepfaMqttV1Event_t mqtt_event_router_connected;
    GrepfaMqttV1Event_t mqtt_event_router_disconnected;
    GrepfaMqttV1Event_t mqtt_event_router_subscribe;
    GrepfaMqttV1Event_t mqtt_event_router_unsubscribe;
    GrepfaMqttV1Event_t mqtt_event_router_published;
    GrepfaMqttV1Event_t mqtt_event_router_data;
    GrepfaMqttV1Event_t mqtt_event_router_unknown;

    SemaphoreHandle_t connectWaitSem;
};



typedef enum GrepfaMQTTEventType_t {
    PAYLOAD_ARRIVED

}GrepfaMQTTEventType_t;



typedef struct grepfa_device_v1 {
    GrepfaMqttConnectorV1_t* connector;
    char id[GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN];
    char type[GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN];

    GrepfaMqttDeviceGetMessageEventHandler_t inboundCallback;
} GrepfaDeviceV1_t;


#endif //P1_TYPES_H

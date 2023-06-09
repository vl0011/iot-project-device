//
// Created by vl0011 on 23. 6. 8.
//

#ifndef P1_TYPES_H
#define P1_TYPES_H

#include <grepfa_mqtt_libconfig.h>
#include <mqtt_client.h>
#include <grepfa_payload_v1.h>

typedef struct grepfa_mqtt_connector_v1 GrepfaMqttConnectorV1_t;
typedef void(*GrepfaMqttV1Event_t) (GrepfaMqttConnectorV1_t*, esp_mqtt_event_handle_t);

struct grepfa_mqtt_connector_v1{
    struct hashmap * devices;
    char id[UUID_STR_LEN];
    esp_mqtt_client_handle_t mqttClient;

    SemaphoreHandle_t connectWaitSem;
};


typedef void (*GrepfaMqttDeviceGetMessageEventHandler_t) (const GrepfaPayloadData_t * payload);

struct grepfa_device_v1_t {
    GrepfaMqttConnectorV1_t* connector;
    char id[GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN];

    GrepfaMqttDeviceGetMessageEventHandler_t inboundCallback;
};
typedef struct grepfa_device_v1_t GrepfaDeviceV1_t;


#endif //P1_TYPES_H

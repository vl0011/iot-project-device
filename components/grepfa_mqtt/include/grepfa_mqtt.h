//
// Created by vl0011 on 23. 6. 1.
//

#ifndef P1_GREPFA_MQTT_H
#define P1_GREPFA_MQTT_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <mqtt_client.h>
#include <grepfa_uuid.h>


typedef struct grepfa_mqtt_connector_v1 GrepfaMqttConnectorV1_t;

typedef void(*GrepfaMqttV1Event_t) (GrepfaMqttConnectorV1_t*, esp_mqtt_event_handle_t);


struct grepfa_mqtt_connector_v1{
    char id[UUID_STR_LEN];
    char* rootTopic;
    esp_mqtt_client_handle_t mqttClient;

    bool isConnected;

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


void mqtt_event_handler(void *args, esp_event_base_t base, int32_t event_id, void *data);
GrepfaMqttConnectorV1_t * GrepfaMqttConnectorNew(const char* endpoint, const char *id, bool wait);

#ifdef __cplusplus
};
#endif

#endif //P1_GREPFA_MQTT_H

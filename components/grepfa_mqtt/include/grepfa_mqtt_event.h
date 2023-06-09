//
// Created by vl0011 on 23. 6. 8.
//

#ifndef P1_GREPFA_MQTT_EVENT_H
#define P1_GREPFA_MQTT_EVENT_H

#include "grepfa_mqtt_types.h"

void mqtt_event_router_error(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_connected(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_disconnected(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_subscribe(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_unsubscribe(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_published(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_data(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_deleted(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);
void mqtt_event_router_unknown(GrepfaMqttConnectorV1_t* client, esp_mqtt_event_handle_t handle);

#endif //P1_GREPFA_MQTT_EVENT_H

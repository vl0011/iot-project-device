//
// Created by vl0011 on 23. 6. 1.
//

#ifndef P1_GREPFA_MQTT_H
#define P1_GREPFA_MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "grepfa_mqtt_types.h"

#include <stdint.h>
#include <mqtt_client.h>
#include <grepfa_uuid.h>

#include <hashmap.h>
#include <grepfa_mqtt_device.h>



GrepfaMqttConnectorV1_t * GrepfaMqttConnectorNew(const char* endpoint, const char *id, bool wait);
void GrepfaMqttConnectorAddDevice(GrepfaMqttConnectorV1_t *client, GrepfaDeviceV1_t* device, int qos);
void GrepfaMqttConnectorPublish(GrepfaMqttConnectorV1_t *client, GrepfaDeviceV1_t* device, const GrepfaPayloadData_t* data, int qos);
//        void GrepfaMqttConnectorPublishAsync(GrepfaMqttConnectorV1_t* client, char* topic, int qos, const char* dataStr, int dataSizeIfDataIsBinary, bool store);


#ifdef __cplusplus
};
#endif

#endif //P1_GREPFA_MQTT_H

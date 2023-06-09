//
// Created by vl0011 on 23. 6. 3.
//

#ifndef P1_GREPFA_MQTT_DEVICE_H
#define P1_GREPFA_MQTT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "grepfa_mqtt_libconfig.h"

#include "grepfa_mqtt_types.h"
#include "grepfa_mqtt.h"
#include "grepfa_mqtt_device.h"
#include <cJSON.h>
#include <time.h>
#include <string.h>
#include <grepfa_payload_v1.h>

void GrepfaMqttDeviceSet(GrepfaDeviceV1_t* dev, char* id, GrepfaMqttDeviceGetMessageEventHandler_t event);


#ifdef __cplusplus
};
#endif

#endif //P1_GREPFA_MQTT_DEVICE_H

//
// Created by vl0011 on 23. 6. 3.
//

#ifndef P1_GREPFA_MQTT_DEVICE_H
#define P1_GREPFA_MQTT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "grepfa_mqtt_libconfig.h"

#include "grepfa_mqtt.h"
#include "grepfa_mqtt_payload.h"
#include <cJSON.h>
#include <time.h>
#include <string.h>

typedef enum GrepfaMQTTEventType_t {
    PAYLOAD_ARRIVED

}GrepfaMQTTEventType_t;

typedef void (*GrepfaMQTTEventHandler_t) ();

typedef struct grepfa_device_v1 {
    GrepfaMqttConnectorV1_t* connector;
    char id[GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN];
    char type[GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN];

    GrepfaMQTTEventHandler_t sd;
} GrepfaDeviceV1_t;

    void GrepfaMqttDeviceSet(GrepfaDeviceV1_t* dev, char* id, char* type);
    char* GrepfaMqttDeviceGetJsonPayload(GrepfaDeviceV1_t* dev, GrepfaDevicePayloadV1_t* arr, int num);
    void GrepfaMqttDeviceSetConnector(GrepfaDeviceV1_t* device, GrepfaMqttConnectorV1_t* connector);

#ifdef __cplusplus
};
#endif


#endif //P1_GREPFA_MQTT_DEVICE_H

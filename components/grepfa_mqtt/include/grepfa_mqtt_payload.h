//
// Created by vl0011 on 23. 6. 3.
//

#ifndef P1_PAYLOAD_H
#define P1_PAYLOAD_H

#ifdef __cplusplus
extern "C"{
#endif


#include <grepfa_mqtt_libconfig.h>
#include <stdint.h>
#include <cJSON.h>

typedef struct grepfa_device_payload_v1{
    uint8_t channel;
    char type[GREPFA_MQTT_CONFIGURATION_PAYLOAD_TYPE_LEN];
    char value[GREPFA_MQTT_CONFIGURATION_PAYLOAD_VALUE_LEN];
} GrepfaDevicePayloadV1_t;


void GrepfaDevicePayloadV1Set(GrepfaDevicePayloadV1_t * payload, uint8_t channel, char* type, char* value);
cJSON* GrepfaDevicePayloadV1ToJsonObj(GrepfaDevicePayloadV1_t* payload);
cJSON* GrepfaDevicePayloadV1ToArr(GrepfaDevicePayloadV1_t* arr, int num);

#ifdef __cplusplus
};
#endif

#endif //P1_PAYLOAD_H

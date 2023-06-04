//
// Created by vl0011 on 23. 6. 3.
//

#include <grepfa_mqtt_payload.h>

#include <string.h>

/**
 * @brief 페이로드를 cJSON 오브젝트로 리턴한다.
 * @details
 * @return cJSON object
 */

void GrepfaDevicePayloadV1Set(GrepfaDevicePayloadV1_t * payload, uint8_t channel, char* type, char* value) {
    payload->channel = channel;
    strncpy(payload->type, type, GREPFA_MQTT_CONFIGURATION_PAYLOAD_TYPE_LEN);
    strncpy(payload->value, value, GREPFA_MQTT_CONFIGURATION_PAYLOAD_VALUE_LEN);
}
cJSON* GrepfaDevicePayloadV1ToJsonObj(GrepfaDevicePayloadV1_t* payload) {
    cJSON* ret;
    ret = cJSON_CreateObject();
    cJSON_AddNumberToObject(ret, "channel", payload->channel);
    cJSON_AddStringToObject(ret, "type", payload->type);
    cJSON_AddStringToObject(ret, "value", payload->value);

    return ret;
}

cJSON* GrepfaDevicePayloadV1ToArr(GrepfaDevicePayloadV1_t* arr, int num) {
    cJSON* ret = cJSON_CreateArray();

    for (int i = 0; i < num; i++) {
        cJSON* obj = GrepfaDevicePayloadV1ToJsonObj(&arr[i]);
        cJSON_AddItemToArray(ret, obj);
    }

    return ret;
}
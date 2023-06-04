//
// Created by vl0011 on 23. 6. 2.
//


#include <grepfa_device.h>
#include <cstring>

GrepfaDeviceV1::GrepfaDeviceV1(const char *id, const char *type) {
    strncpy(this->id, id, GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN);
    strncpy(this->type, type, GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN);


}

cJSON * GrepfaDevicePayloadV1::toJsonObj() {
    cJSON* ret;
    ret = cJSON_CreateObject();
    cJSON_AddNumberToObject(ret, "channel", channel);
    cJSON_AddStringToObject(ret, "type", value);
    cJSON_AddStringToObject(ret, "value", value);
    return ret;
}

GrepfaDevicePayloadV1::GrepfaDevicePayloadV1(int channel, const char *type, const char *value) {
    this->channel = channel;
    strncpy(this->type, type, GREPFA_MQTT_CONFIGURATION_PAYLOAD_TYPE_LEN);
    strncpy(this->value, value, GREPFA_MQTT_CONFIGURATION_PAYLOAD_VALUE_LEN);
}

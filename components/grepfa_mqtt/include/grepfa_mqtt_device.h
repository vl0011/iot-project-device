//
// Created by vl0011 on 23. 6. 3.
//

#ifndef P1_GREPFA_DEVICE_H
#define P1_GREPFA_DEVICE_H

#include <vector>
#include "grepfa_mqtt_libconfig.h"
#include <cJSON.h>


class GrepfaDevicePayloadV1{
private:
    int channel;
    char type[GREPFA_MQTT_CONFIGURATION_PAYLOAD_TYPE_LEN]{};
    char value[GREPFA_MQTT_CONFIGURATION_PAYLOAD_VALUE_LEN]{};

public:
    GrepfaDevicePayloadV1(int channel, const char *type, const char *value);

    cJSON * toJsonObj();
};


class GrepfaDeviceV1{
private:

    // devicce uuid
    char id[GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN];
    char type[GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN];

public:
    GrepfaDeviceV1(const char* id, const char* type);

    void sendValue(std::vector<GrepfaDevicePayloadV1> msg);

    // TODO:: event
};




#endif //P1_GREPFA_DEVICE_H

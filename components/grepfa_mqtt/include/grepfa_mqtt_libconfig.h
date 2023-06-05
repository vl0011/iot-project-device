//
// Created by vl0011 on 23. 6. 1.
//

#ifndef P1_GREPFA_MQTT_LIBCONFIG_H
#define P1_GREPFA_MQTT_LIBCONFIG_H

#include <grepfa_uuid.h>

#define GREPFA_MQTT_CONFIGURATION_CERTIFICATION_FROM_ASM
#define GREPFA_MQTT_CONST_MQTT_TOPIC_LENGTH 255

#define GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN UUID_STR_LEN
#define GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN 10

#define GREPFA_MQTT_CONFIGURATION_PAYLOAD_TYPE_LEN 10
#define GREPFA_MQTT_CONFIGURATION_PAYLOAD_VALUE_LEN 10

#define GREPFA_MQTT_CONFIGURATION_PROTOCOL_VERSION 1
#define GREPFA_MQTT_CONFIGURATION_PROTOCOL_VERSION_STR "v1"
#define GREPFA_MQTT_CONFIGURATION_ROOT_TOPIC "grepfa/" GREPFA_MQTT_CONFIGURATION_PROTOCOL_VERSION_STR  "/"

#define GREPFA_MQTT_CONFIGURATION_LOG_TOPIC GREPFA_MQTT_CONFIGURATION_ROOT_TOPIC "log"

#endif //P1_GREPFA_MQTT_LIBCONFIG_H

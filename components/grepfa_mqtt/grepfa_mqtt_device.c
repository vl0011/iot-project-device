//
// Created by vl0011 on 23. 6. 2.
//
static const char* TAG = "mqtt_dev";

#include <grepfa_mqtt_device.h>
#include <string.h>
#include <grepfa_uuid.h>
#include <esp_log.h>

void GrepfaMqttDeviceSet(GrepfaDeviceV1_t* dev, char* id,GrepfaMqttDeviceGetMessageEventHandler_t event) {
    strncpy(dev->id, id, UUID_STR_LEN);
    dev->inboundCallback = event;
}

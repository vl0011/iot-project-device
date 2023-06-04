//
// Created by vl0011 on 23. 6. 2.
//
static const char* TAG = "mqtt_dev";

#include <grepfa_mqtt_device.h>
#include <string.h>
#include <time.h>
#include <grepfa_uuid.h>
#include <esp_log.h>


//
//
//
//GrepfaDeviceV1::GrepfaDeviceV1(const char *id, const char *type) {
//    strncpy(this->id, id, GREPFA_MQTT_CONFIGURATION_DEVICE_ID_LEN);
//    strncpy(this->type, type, GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN);
//
//
//}
//
//cJSON *GrepfaDeviceV1::toJsonObj() {
//    auto ret = cJSON_CreateObject();
//    cJSON_AddNumberToObject(ret, "timestamp", time(nullptr));
//    cJSON_AddStringToObject(ret, "things_id", id);
//
//    char rnd_uuid[UUID_STR_LEN];
//    random_uuid(rnd_uuid);
//    cJSON_AddStringToObject(ret, "payload_id", rnd_uuid);
//
//    return ret;
//}
//
//void GrepfaDeviceV1::sendValue(std::vector<GrepfaDevicePayloadV1> *msg) {
//
//
//
//}
//
//char *GrepfaDeviceV1::toJsonStr(std::vector<GrepfaDevicePayloadV1> *msg) {
//    auto ret = toJsonObj();
//    auto arr = cJSON_AddArrayToObject(ret, "values");
//
//    for (auto data: *msg) {
//        cJSON_AddItemReferenceToArray(arr, data.toJsonObj());
//    }
//
//    char* printed = cJSON_Print(ret);
//    char* retStr = strdup(printed);
//    cJSON_Delete(ret);
//    cJSON_free(printed);
//    ESP_LOGI(TAG, "data: %s", retStr);
//    return retStr;
//}
//

inline void GrepfaMqttDeviceSet(GrepfaDeviceV1_t* dev, char* id, char* type) {
    strncpy(dev->id, id, UUID_STR_LEN);
    strncpy(dev->type, type, GREPFA_MQTT_CONFIGURATION_DEVICE_TYPE_LEN);
}

char *GrepfaMqttDeviceGetJsonPayload(GrepfaDeviceV1_t *dev, GrepfaDevicePayloadV1_t *arr, int num) {
    cJSON* obj = cJSON_CreateObject();

    // "time": timestamp,
    cJSON_AddNumberToObject(obj, "timestamp", time(NULL));

    // "things_id": device_uuid
    cJSON_AddStringToObject(obj, "things_id", dev->id);

    // "payload_id": random_uuid
    char rnd_uuid[UUID_STR_LEN];
    random_uuid(rnd_uuid);
    cJSON_AddStringToObject(obj, "payload_id", rnd_uuid);

    // "values": payload_arr
    cJSON_AddItemToObject(obj, "values",
                          GrepfaDevicePayloadV1ToArr(arr, num)
    );

    char* temp_str = cJSON_Print(obj);
    char* ret_str = strdup(temp_str);

    cJSON_free(temp_str);
    cJSON_Delete(obj);

    return ret_str;
}

void GrepfaMqttDeviceSetConnector(GrepfaDeviceV1_t* device, GrepfaMqttConnectorV1_t* connector) {
    device->connector = connector;
}

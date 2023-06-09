//
// Created by vl0011 on 23. 6. 8.
//

static const char * TAG = "grepfa_json";

#include <grepfa_payload_v1.h>
#include <string.h>
#include <cJSON.h>
#include <esp_log.h>

GrepfaPayloadData_t *NewGrepfaPayloadData(const char* things_id, time_t timestamp) {
    GrepfaPayloadData_t * ret = malloc(sizeof(GrepfaPayloadData_t));
    memset(ret, 0x00, sizeof(GrepfaPayloadData_t));
    strncpy(ret->things_id, things_id, UUID_STR_LEN);
    ret->timestamp = (timestamp == 0) ? time(NULL) : timestamp;
    random_uuid(ret->payload_id);
    return ret;
}

void DeleteGrepfaPayloadData(GrepfaPayloadData_t *data) {
    free(data->values);
    free(data);
}

GrepfaPayloadValue_t *SetGrepfaPayloadValue(GrepfaPayloadValue_t * target, int channel, const char *type, const char *value) {
    target->channel = channel;
    strncpy(target->type, type, GREPFA_PAYLOAD_CONFIGURATION_TYPE_LEN);
    strncpy(target->value, value, GREPFA_PAYLOAD_CONFIGURATION_VALUE_LEN);
    return target;
}

void DeleteGrepfaPayloadValue(GrepfaPayloadValue_t *value) {
    free(value);
}

GrepfaPayloadData_t *AddGrepfaPayloadValue(GrepfaPayloadData_t * payload, int channel, const char *type, const char *value) {
    payload->values = (payload->valueLen == 0) ?
            malloc(sizeof(GrepfaPayloadValue_t)) :
            realloc(payload->values, sizeof(GrepfaPayloadValue_t) * (payload->valueLen + 1));
    SetGrepfaPayloadValue(&(payload->values[payload->valueLen]), channel, type, value);
    ++payload->valueLen;
    return payload;
}

GrepfaPayloadData_t *JsonToGrepfaPayloadData(const char *jsonStr, int jsonLen) {
    cJSON *json = cJSON_ParseWithLength(jsonStr, jsonLen);

    if (json == NULL) { ESP_LOGE(TAG, "invalid json format"); return NULL; }

    cJSON * tIdObj = cJSON_GetObjectItemCaseSensitive(json, "things_id");
    if(tIdObj == NULL || !cJSON_IsString(tIdObj)) { ESP_LOGE(TAG, "things_id error"); goto fail;}
    const char* thingsId = tIdObj->valuestring;

    cJSON * pIdObj = cJSON_GetObjectItemCaseSensitive(json, "payload_id");
    if(pIdObj == NULL || !cJSON_IsString(pIdObj)) { ESP_LOGE(TAG, "payload_id error"); goto fail;}
    const char* payloadId = pIdObj->valuestring;

    cJSON * timestampObj = cJSON_GetObjectItemCaseSensitive(json, "timestamp");
    if (timestampObj == NULL || !cJSON_IsNumber(timestampObj)) { ESP_LOGE(TAG, "timestamp error"); goto fail;}
    time_t timestamp = timestampObj->valuedouble;

    cJSON * valuesObj = cJSON_GetObjectItemCaseSensitive(json, "values");
    if (valuesObj == NULL || !cJSON_IsArray(valuesObj)) { ESP_LOGE(TAG, "values error"); goto fail;}

    GrepfaPayloadData_t* ret = NewGrepfaPayloadData(thingsId, timestamp);

    strncpy(ret->payload_id, payloadId, UUID_STR_LEN);

    cJSON* valuesEl;
    cJSON_ArrayForEach(valuesEl, valuesObj) {
        cJSON* channelObj = cJSON_GetObjectItemCaseSensitive(valuesEl, "channel");
        if(channelObj == NULL || !cJSON_IsNumber(channelObj)) {
            continue;
        }
        int channel = valuesEl->valuedouble;

        cJSON * typeObj = cJSON_GetObjectItemCaseSensitive(valuesEl, "type");
        if(typeObj == NULL || !cJSON_IsString(typeObj)) {
            continue;
        }
        const char* type = typeObj->valuestring;

        cJSON * valueObj = cJSON_GetObjectItemCaseSensitive(valuesEl, "value");
        if(valueObj == NULL || !cJSON_IsString(valueObj)) {
            continue;
        }
        const char* value = valueObj->valuestring;

        AddGrepfaPayloadValue(ret,channel, type, value);
    }

    return ret;

fail:
    cJSON_Delete(json);
    return NULL;
}

char *GrepfaPayloadDataToJson(const GrepfaPayloadData_t *payload) {
    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "timestamp", payload->timestamp);
    cJSON_AddStringToObject(json, "things_id", payload->things_id);
    cJSON_AddStringToObject(json, "payload_id", payload->payload_id);

    cJSON* valArr = cJSON_CreateArray();

    for (int i = 0; i < payload->valueLen; ++i) {
        cJSON * valJe = cJSON_CreateObject();
        GrepfaPayloadValue_t * val = &(payload->values[i]);

        cJSON_AddNumberToObject(valJe, "channel", val->channel);
        cJSON_AddStringToObject(valJe, "type", val->type);
        cJSON_AddStringToObject(valJe, "value", val->value);

        cJSON_AddItemToArray(valArr, valJe);
    }

    cJSON_AddItemToObject(json, "values", valArr);

    char* buf = cJSON_PrintUnformatted(json);
    char* ret = strdup(buf);

    cJSON_Delete(json);
    cJSON_free(buf);

    return ret;
}
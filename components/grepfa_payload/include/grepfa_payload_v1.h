//
// Created by vl0011 on 23. 6. 8.
//

#ifndef P1_GREPFA_PAYLOAD_H
#define P1_GREPFA_PAYLOAD_H

#include <grepfa_uuid.h>
#include <time.h>
#include <stdint.h>

#define GREPFA_PAYLOAD_CONFIGURATION_VALUE_LEN 20
#define GREPFA_PAYLOAD_CONFIGURATION_TYPE_LEN 20

typedef struct grepfa_payload_value_t {
    int channel;
    char type[GREPFA_PAYLOAD_CONFIGURATION_TYPE_LEN];
    char value[GREPFA_PAYLOAD_CONFIGURATION_VALUE_LEN];
}GrepfaPayloadValue_t;

typedef struct grepfa_payload_Data_t{
    time_t timestamp;
    char things_id[UUID_STR_LEN];
    char payload_id[UUID_STR_LEN];
    GrepfaPayloadValue_t * values;
    int valueLen;
}GrepfaPayloadData_t;


GrepfaPayloadData_t * JsonToGrepfaPayloadData(const char* jsonStr, int jsonLen);
char* GrepfaPayloadDataToJson(GrepfaPayloadData_t* data);
#define DeleteGrepfaPayloadDataJsonStr(str) free(str)

GrepfaPayloadData_t* NewGrepfaPayloadData(const char* things_id, time_t timestamp);
void DeleteGrepfaPayloadData(GrepfaPayloadData_t* data);

GrepfaPayloadValue_t * SetGrepfaPayloadValue(GrepfaPayloadValue_t * target, int channel, const char* type, const char* value);

GrepfaPayloadData_t *AddGrepfaPayloadValue(GrepfaPayloadData_t * payload, int channel, const char *type, const char *value);

#endif //P1_GREPFA_PAYLOAD_H

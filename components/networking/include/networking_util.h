//
// Created by vl0011 on 23. 6. 1.
//

#ifndef P1_NETWORKING_UTIL_H
#define P1_NETWORKING_UTIL_H

#include <esp_wifi_types.h>
const char* auth_mode_str(wifi_auth_mode_t authmode);
const char* cipher_type_str(wifi_cipher_type_t cipher);
const char* disconnect_cause_str();

#endif //P1_NETWORKING_UTIL_H

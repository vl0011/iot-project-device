//
// Created by vl0011 on 23. 6. 1.
//

#include <networking_util.h>

const char* auth_mode_str(wifi_auth_mode_t authmode)
{
    switch (authmode) {
        case WIFI_AUTH_OPEN:
            return "OPEN";

        case WIFI_AUTH_OWE:
            return "OWE";
            
        case WIFI_AUTH_WEP:
            return "WEP";
            
        case WIFI_AUTH_WPA_PSK:
            return "WPA_PSK";
            
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2_PSK";
            
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA_WPA2_PSK";
            
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2_ENTERPRISE";
            
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3_PSK";
            
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2_WPA3_PSK";
            
        default:
            return "UNKNOWN";
            
    }
}

const char *cipher_type_str(wifi_cipher_type_t cipher) {
    switch (cipher) {
        case WIFI_CIPHER_TYPE_NONE:
            return "NONE";
            
        case WIFI_CIPHER_TYPE_WEP40:
            return "WEP40";
            
        case WIFI_CIPHER_TYPE_WEP104:
            return "WEP104";
            
        case WIFI_CIPHER_TYPE_TKIP:
            return "TKIP";
            
        case WIFI_CIPHER_TYPE_CCMP:
            return "CCMP";
            
        case WIFI_CIPHER_TYPE_TKIP_CCMP:
            return "TKIP_CCMP";
            
        case WIFI_CIPHER_TYPE_AES_CMAC128:
            return "AES_CMAC128";
            
        case WIFI_CIPHER_TYPE_SMS4:
            return "SMS4";
            
        case WIFI_CIPHER_TYPE_GCMP:
            return "GCMP";
            
        case WIFI_CIPHER_TYPE_GCMP256:
            return "GCMP256";
            
        default:
            return "UNKNOWN";
            
    }
}

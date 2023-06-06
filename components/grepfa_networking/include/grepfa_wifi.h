//
// Created by vl0011 on 23. 6. 4.
//

#ifndef P1_GREPFA_WIFI_H
#define P1_GREPFA_WIFI_H

#include <esp_wifi_types.h>

#ifdef __cplusplus
extern "C"{
#endif

void GrepfaWiFiInit();
void GrepfaWiFiStop();
void GrepfaWiFiSTA();
esp_err_t GrepfaWiFiDisconnectSTA();
esp_err_t GrepfaWiFiConnectSTA(const char* SSID, const char* password, bool wait, int retry);
void GrepfaPrintAllIPs();
esp_err_t GrepfaWiFiStartScan(bool wait, bool showHidden, bool activeScan);
esp_err_t GrepfaWiFiStopScan();
esp_err_t GrepfaWiFiGetScanRecord(uint16_t num, uint16_t * found, wifi_ap_record_t* records);
esp_err_t GrepfaWiFiSTAGetApInfo(wifi_ap_record_t* info);

#ifdef __cplusplus
};
#endif
#endif //P1_GREPFA_WIFI_H

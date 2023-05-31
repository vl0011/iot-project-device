//
// Created by vl0011 on 23. 5. 31.
//

/*
 * networking.h
 * ESP-IDF wifi syscall c++ static class wrapper
 */

#ifndef P1_NETWORKING_H
#define P1_NETWORKING_H

#include "libconfig.h"
#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>
#include "esp_netif.h"
#include "networking_util.h"


class WiFi{
private:

    //
    static esp_netif_t *s_sta_netif;

    // semaphore
    static SemaphoreHandle_t s_semph_get_ip4_addrs;

#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
    static SemaphoreHandle_t s_semph_get_ip6_addrs;
#endif
    // retry counter
    static int retryCounter;
    static int retryNum;

    // wifi scan filter
    static wifi_scan_method_t scanType;
    static wifi_sort_method_t sortType;
    static wifi_auth_mode_t authThreshold;
    static int8_t rssiThreshold;
    static bool usePmf;


    // sta event
    static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data);
    static void on_sta_got_ipv4(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);

#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
    static void on_sta_got_ipv6(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);
#endif

    static void on_wifi_connect(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);
    static bool is_our_netif(char* prefix, esp_netif_t *netif);

    static void wifi_shutdown();

public:
    // initializer
    static void init();

    // dynamic configuration
    static void set_scan_method(wifi_scan_method_t method);
    static void set_sort_method(wifi_sort_method_t method);
    static void set_auth_threshold(wifi_auth_mode_t mode);
    static void set_rssi_threshold(int8_t rssi);

    // ap scan
    static esp_err_t scan_start(bool wait, bool showHidden, bool activeScan);
    static esp_err_t scan_stop();
    static esp_err_t scan_get_record(uint16_t num, uint16_t * found, wifi_ap_record_t* records);

    // for sta
    static void start_sta();
    static esp_err_t connect_sta(const char* SSID, const char * pw, bool wait, int retry);

    static esp_err_t disconnect_sta();


    // for ap
    // not test
    static void start_ap();

    static void stop();

    static void print_all_wifi_ips();
};


#endif //P1_NETWORKING_H

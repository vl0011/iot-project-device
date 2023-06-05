//
// Created by vl0011 on 23. 6. 6.
//

static const char* TAG = "grepfa_sntp";

#include <sdkconfig.h>
#include <grepfa_sntp.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"

static void init() {
    ESP_LOGI(TAG, "init sntp server");

#if CONFIG_LWIP_SNTP_MAX_SERVERS > 1
    esp_sntp_config_t cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(
            GREPFA_SNTP_CONFIGURATION_SNTP_SERVER_POOL_NUM,
            ESP_SNTP_SERVER_LIST(GREPFA_SNTP_CONFIGURATION_SNTP_SERVER_POOL)
        );
#else
#error check config sntp max servers
#endif
#ifndef CONFIG_LWIP_DHCP_GET_NTP_SRV
#error check config enable lwip dhcp get ntp config
#endif

    cfg.start = false;
    cfg.server_from_dhcp = true;
    cfg.renew_servers_after_new_IP = true;
    cfg.index_of_first_server = 1;
    cfg.ip_event_to_renew = IP_EVENT_STA_GOT_IP;
    cfg.smooth_sync = GREPFA_SNTP_CONFIGURATION_SMOOTH_SYNC;

    esp_netif_sntp_init(&cfg);
}


void GrepfaSntpObtainTime(){
    init();
    esp_netif_sntp_start();
    ESP_LOGI(TAG, "synchronize time start");

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 15;
    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}
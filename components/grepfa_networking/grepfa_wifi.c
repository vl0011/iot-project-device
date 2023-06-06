//
// Created by vl0011 on 23. 6. 4.
//

static const char* TAG = "grepfa_wifi";

#include "freertos/FreeRTOS.h"
#include <grepfa_wifi.h>
#include <esp_log.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include <networking_libconfig.h>
#include <string.h>
#include <freertos/semphr.h>

static wifi_scan_method_t scanType = NETWORKING_DEFAULT_SCAN_METHOD;
static wifi_sort_method_t sortType = NETWORKING_DEFAULT_SORT_METHOD;
static wifi_auth_mode_t authThreshold = NETWORKING_DEFAULT_AUTH_THRESHOLD;
static int8_t rssiThreshold = NETWORKING_DEFAULT_RSSI_THRESHOLD;
static bool usePmf = NETWORKING_DEFAULT_PMF;

static int retryCounter = 0;
static int retryNum = 0;

// retry counter
static int retryCounter;
static int retryNum;

// wifi scan filter
static wifi_scan_method_t scanType;
static wifi_sort_method_t sortType;
static wifi_auth_mode_t authThreshold;
static int8_t rssiThreshold;
static bool usePmf;

static esp_netif_t *s_sta_netif;
static SemaphoreHandle_t s_semph_get_ip4_addrs;



static bool is_our_netif(char *prefix, esp_netif_t *netif) {
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;;
}

static void on_wifi_connect(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_netif_create_ip6_linklocal((esp_netif_t*)esp_netif);
}

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    retryCounter ++;
    wifi_event_sta_disconnected_t* cause = (wifi_event_sta_disconnected_t*)event_data;
    ESP_LOGI(TAG, "WiFi Connect failed. %d", cause->reason);

    if(retryCounter > retryNum) {
        ESP_LOGI(TAG, "wifi connect failed %d times", retryCounter);
        if(s_semph_get_ip4_addrs) xSemaphoreGive(s_semph_get_ip4_addrs);

        return;
    }

    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi syscall error");
        retryNum = 0;
    }
}

static void on_sta_got_ipv4(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    retryCounter = 0;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

    if(!is_our_netif(WIFI_NETIF_DESC, event->esp_netif)) {
        return;
    }

    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));

    if (s_semph_get_ip4_addrs) {
        xSemaphoreGive(s_semph_get_ip4_addrs);
    } else {
        ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
    }
}

void GrepfaWiFiShutdown() {
    GrepfaWiFiDisconnectSTA();
    GrepfaWiFiStop();
}

void GrepfaWiFiInit() {
    ESP_LOGI(TAG, "init wifi resource start");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "init wifi resource done");
}

void GrepfaWiFiStop() {
    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_sta_netif));
    esp_netif_destroy(s_sta_netif);
    s_sta_netif = NULL;
}

void GrepfaWiFiSTA() {
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

    esp_netif_config.if_desc = WIFI_NETIF_DESC;

    s_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
    esp_wifi_set_default_wifi_sta_handlers();

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}


esp_err_t GrepfaWiFiDisconnectSTA() {
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_sta_got_ipv4));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect));

    if (s_semph_get_ip4_addrs) {
        vSemaphoreDelete(s_semph_get_ip4_addrs);
    }

    return esp_wifi_disconnect();
}


esp_err_t GrepfaWiFiConnectSTA(const char* SSID, const char* password, bool wait, int retry) {
    retryNum = retry;

    wifi_config_t cfg;
    strncpy((char*)&cfg.sta.ssid, SSID, MAX_SSID_LEN);
    if (password != NULL) {
        strncpy((char*)&cfg.sta.password, password, MAX_PASSPHRASE_LEN);
    }

    cfg.sta.scan_method = scanType;
    cfg.sta.sort_method = sortType;
    cfg.sta.threshold.rssi = rssiThreshold;
    cfg.sta.threshold.authmode = authThreshold;
    cfg.sta.pmf_cfg.required = usePmf;
    if (wait) {
        s_semph_get_ip4_addrs = xSemaphoreCreateBinary();
        if (s_semph_get_ip4_addrs == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_sta_got_ipv4, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect, s_sta_netif));

    ESP_LOGI(TAG, "connecting to [%s] ...", cfg.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));

    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
        return ret;
    } else {
        ESP_LOGI(TAG, "connecting...");
    }

    if (wait) {
        ESP_LOGI(TAG, "Waiting for IP(s)");
        xSemaphoreTake(s_semph_get_ip4_addrs, portMAX_DELAY);
    }

    ESP_ERROR_CHECK(esp_register_shutdown_handler(&GrepfaWiFiShutdown));

    return ESP_OK;
}

void GrepfaPrintAllIPs() {
    esp_netif_t *netif = NULL;
    for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
        netif = esp_netif_next(netif);
        if (is_our_netif(WIFI_NETIF_DESC, netif)) {
            ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
            esp_netif_ip_info_t ip;
            ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));
            ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&ip.ip));
        }
    }
}

esp_err_t GrepfaWiFiStartScan(bool wait, bool showHidden, bool activeScan) {
    wifi_scan_config_t cfg = {
            .show_hidden = showHidden,
            .scan_type = activeScan ? WIFI_SCAN_TYPE_ACTIVE:WIFI_SCAN_TYPE_PASSIVE,
    };

    return esp_wifi_scan_start(&cfg, wait);
}

esp_err_t GrepfaWiFiStopScan() {
    return esp_wifi_scan_stop();
}

esp_err_t GrepfaWiFiGetScanRecord(uint16_t num, uint16_t * found, wifi_ap_record_t* records) {
    esp_err_t res;
    res = esp_wifi_scan_get_ap_records(&num, records);
    if(res != ESP_OK) {
        return res;
    }

    return esp_wifi_scan_get_ap_num(found);
}

esp_err_t GrepfaWiFiSTAGetApInfo(wifi_ap_record_t* info) {
    return esp_wifi_sta_get_ap_info(info);
}

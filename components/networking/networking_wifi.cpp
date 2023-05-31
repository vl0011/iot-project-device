// temp define


static const char *TAG = "net_wifi";

#include "networking.h"

#include <cstdio>
#include <cstring>

#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// default value initializer start
esp_netif_t *WiFi::s_sta_netif = nullptr;

SemaphoreHandle_t WiFi::s_semph_get_ip4_addrs = nullptr;

#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
SemaphoreHandle_t WiFi::s_semph_get_ip6_addrs = nullptr;
#endif

wifi_scan_method_t WiFi::scanType = NETWORKING_DEFAULT_SCAN_METHOD;
wifi_sort_method_t WiFi::sortType = NETWORKING_DEFAULT_SORT_METHOD;

wifi_auth_mode_t WiFi::authThreshold = NETWORKING_DEFAULT_AUTH_THRESHOLD;
int8_t WiFi::rssiThreshold = NETWORKING_DEFAULT_RSSI_THRESHOLD;

bool WiFi::usePmf = NETWORKING_DEFAULT_PMF;

int WiFi::retryCounter = 0;
int WiFi::retryNum = 0;
// default value initializer end



void WiFi::init() {
    ESP_LOGI(TAG, "init wifi resource start");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "init wifi resource done");
}

void WiFi::stop() {
    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_sta_netif));
    esp_netif_destroy(s_sta_netif);
    s_sta_netif = nullptr;
}

void WiFi::start_sta() {
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

    esp_netif_config.if_desc = WIFI_NETIF_DESC;
//    esp_netif_config.route_prio = 128;

    s_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
    esp_wifi_set_default_wifi_sta_handlers();

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// NOT_TESTED
void WiFi::start_ap() {
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();

    esp_netif_config.if_desc = WIFI_NETIF_DESC;

    s_sta_netif = esp_netif_create_wifi(WIFI_IF_AP, &esp_netif_config);

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
}


esp_err_t WiFi::connect_sta(const char *SSID, const char *pw, bool wait, int retry) {
    retryNum = retry;

    wifi_config_t cfg;
    strncpy((char*)&cfg.sta.ssid, SSID, MAX_SSID_LEN);
    if (pw != nullptr) {
        strncpy((char*)&cfg.sta.password, pw, MAX_PASSPHRASE_LEN);
    }

    cfg.sta.scan_method = WiFi::scanType;
    cfg.sta.sort_method = WiFi::sortType;
    cfg.sta.threshold.rssi = WiFi::rssiThreshold;
    cfg.sta.threshold.authmode = WiFi::authThreshold;
    cfg.sta.pmf_cfg.required = WiFi::usePmf;

    if (wait) {
        s_semph_get_ip4_addrs = xSemaphoreCreateBinary();
        if (s_semph_get_ip4_addrs == nullptr) {
            return ESP_ERR_NO_MEM;
        }
#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
        s_semph_get_ip6_addrs = xSemaphoreCreateBinary();
        if (s_semph_get_ip6_addrs == nullptr) {
            vSemaphoreDelete(s_semph_get_ip4_addrs);
            return ESP_ERR_NO_MEM;
        }
#endif
    }

    // wifi event register
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &WiFi::on_wifi_disconnect, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFi::on_sta_got_ipv4, nullptr));
#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &WiFi::on_sta_got_ipv6, nullptr));
#endif
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
#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
        xSemaphoreTake(s_semph_get_ip6_addrs, portMAX_DELAY);
#endif
    }

    ESP_ERROR_CHECK(esp_register_shutdown_handler(&wifi_shutdown));

    return ESP_OK;
}

void WiFi::on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    retryCounter ++;
    auto* cause = (wifi_event_sta_disconnected_t*)event_data;
    ESP_LOGI(TAG, "WiFi Connect failed. %d", cause->reason);

    if(retryCounter > retryNum) {
        ESP_LOGI(TAG, "wifi connect failed %d times", retryCounter);
        if(s_semph_get_ip4_addrs) xSemaphoreGive(s_semph_get_ip4_addrs);
#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
        if(s_semph_get_ip6_addrs) xSemaphoreGive(s_semph_get_ip6_addrs);
#endif
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

void WiFi::on_sta_got_ipv4(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    retryCounter = 0;

    auto *event = (ip_event_got_ip_t *)event_data;

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

#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
void WiFi::on_sta_got_ipv6(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    auto *event = (ip_event_got_ip6_t *)event_data;

    if(!is_our_netif(WIFI_NETIF_DESC, event->esp_netif)) {
        return;
    }

//    ESP_LOGI(TAG, "Got IPv6 event: Interface \"%s\" address: " IPV6STR ", type: %s", esp_netif_get_desc(event->esp_netif),
//             IPV62STR(event->ip6_info.ip), example_ipv6_addr_types_to_str[ipv6_type]);



//    if (ipv6_type == EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE) {
//        if (s_semph_get_ip6_addrs) {
//            xSemaphoreGive(s_semph_get_ip6_addrs);
//        } else {
//            ESP_LOGI(TAG, "- IPv6 address: " IPV6STR ", type: %s", IPV62STR(event->ip6_info.ip), example_ipv6_addr_types_to_str[ipv6_type]);
//        }
//    }
}
#endif

void WiFi::on_wifi_connect(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_netif_create_ip6_linklocal((esp_netif_t*)esp_netif);
}

bool WiFi::is_our_netif(char *prefix, esp_netif_t *netif) {
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;;
}

esp_err_t WiFi::disconnect_sta() {
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_sta_got_ipv4));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect));
#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, &on_sta_got_ipv6));
#endif
    if (s_semph_get_ip4_addrs) {
        vSemaphoreDelete(s_semph_get_ip4_addrs);
    }
#if NETWORKING_CONFIGURATION_USE_IPV6 != 0
    if (s_semph_get_ip6_addrs) {
        vSemaphoreDelete(s_semph_get_ip6_addrs);
    }
#endif
    return esp_wifi_disconnect();
}

void WiFi::wifi_shutdown() {
    disconnect_sta();
    stop();
}

void WiFi::print_all_wifi_ips() {
    esp_netif_t *netif = nullptr;
    for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
        netif = esp_netif_next(netif);
        if (is_our_netif(WIFI_NETIF_DESC, netif)) {
            ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
            esp_netif_ip_info_t ip;
            ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));

            ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&ip.ip));
//            esp_ip6_addr_t ip6[MAX_IP6_ADDRS_PER_NETIF];
//            int ip6_addrs = esp_netif_get_all_ip6(netif, ip6);
//            for (int j = 0; j < ip6_addrs; ++j) {
//                esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&(ip6[j]));
//                ESP_LOGI(TAG, "- IPv6 address: " IPV6STR ", type: %s", IPV62STR(ip6[j]), example_ipv6_addr_types_to_str[ipv6_type]);
//            }
        }
    }
}


void WiFi::set_scan_method(wifi_scan_method_t method) {
    scanType = method;
}

void WiFi::set_sort_method(wifi_sort_method_t method) {
    sortType = method;
}

void WiFi::set_auth_threshold(wifi_auth_mode_t mode) {
    authThreshold = mode;
}

void WiFi::set_rssi_threshold(int8_t rssi) {
    rssiThreshold = rssi;
}

esp_err_t WiFi::scan_start(bool wait, bool showHidden, bool activeScan) {
    wifi_scan_config_t cfg = {
            .show_hidden = showHidden,
            .scan_type = activeScan ? WIFI_SCAN_TYPE_ACTIVE:WIFI_SCAN_TYPE_PASSIVE,
    };

    return esp_wifi_scan_start(&cfg, wait);
}

esp_err_t WiFi::scan_stop() {
    return esp_wifi_scan_stop();
}

esp_err_t WiFi::scan_get_record(uint16_t num, uint16_t * found, wifi_ap_record_t* records) {
    esp_err_t res;
    res = esp_wifi_scan_get_ap_records(&num, records);
    if(res != ESP_OK) {
        return res;
    }

    return esp_wifi_scan_get_ap_num(found);
}








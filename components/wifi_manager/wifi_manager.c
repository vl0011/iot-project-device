// temp define

#define CONFIG_EXAMPLE_CONNECT_WIFI 1

const char *TAG = "connector";

#include <stdio.h>
#include "wifi_manager.h"

#include <string.h>
#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define NETIF_DESC_STA "desc"
static esp_netif_t *s_sta_netif = NULL;

// 세마포어
static SemaphoreHandle_t s_semph_get_ip_addrs = NULL;

static int s_retry_num = 0;

// 함수

// 초기화
void init_wifi();

esp_err_t do_connect_wifi(wifi_config_t cfg, bool wait);

esp_err_t wifi_connect();

// 이벤트

// 와이파이 연결 중단 이벤트
static void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// 와이파이 연결 이벤트
static void on_wifi_connect(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data);

// ip 획득 이벤트
static void on_sta_got_ip(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    s_retry_num++;
    if (s_retry_num > CONFIG_CONN_MAX_RETRY) {
        ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.", s_retry_num);
        /* let example_wifi_sta_do_connect() return */
        if (s_semph_get_ip_addrs) {
            xSemaphoreGive(s_semph_get_ip_addrs);
        }
        return;
    }
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }
    ESP_ERROR_CHECK(err);
}


// 와이파이 개체 초기화
// STA 모드로 wifi 사용
void init_wifi() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

    esp_netif_config.if_desc = NETIF_DESC_STA;
    esp_netif_config.route_prio = 128;

    s_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
    esp_wifi_set_default_wifi_sta_handlers();

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}


// 실제 와이파이 연결을 수행 합니다.
// IPV4 만 수행
esp_err_t do_connect_wifi(wifi_config_t cfg, bool wait) {
    if (wait) {
        s_semph_get_ip_addrs = xSemaphoreCreateBinary();
        if (s_semph_get_ip_addrs == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    s_retry_num = 0;
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_sta_got_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect, s_sta_netif));

    ESP_LOGI(TAG, "Connecting to %s...", cfg.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
        return ret;
    }
    if (wait) {
        ESP_LOGI(TAG, "Waiting for IP(s)");

        xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);


        if (s_retry_num > CONFIG_CONN_MAX_RETRY) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t wifi_connect() {
    ESP_LOGI(TAG, "start wifi connect...");

    init_wifi();

    wifi_config_t wifi_config = {
            .sta = {
                    .ssid = CONFIG_WIFI_SSID,
                    .password = CONFIG_WIFI_PASSWORD,

                    // 풀 스캔
                    .scan_method = WIFI_ALL_CHANNEL_SCAN,

                    // RSSI 순
                    .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,


                    .threshold.rssi = -127,
//                    .threshold.authmode = WIFI_AUTH_MAX,
            },
    };

    return do_connect_wifi(wifi_config, true);
}

void on_sta_got_ip(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    s_retry_num = 0;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    if (strncmp(NETIF_DESC_STA, esp_netif_get_desc(event->esp_netif), strlen(NETIF_DESC_STA) - 1) != 0) {
        return;
    }
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif),
             IP2STR(&event->ip_info.ip));
    if (s_semph_get_ip_addrs) {
        xSemaphoreGive(s_semph_get_ip_addrs);
    } else {
        ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
    }
}

void on_wifi_connect(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "wifi connected");
}


esp_err_t start_connection() {
#if CONFIG_EXAMPLE_CONNECT_WIFI


#endif
    return ESP_OK;
}

esp_err_t stop_connection() {

    return ESP_OK;
}

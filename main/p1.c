#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <esp_log.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include <grepfa_wifi.h>
#include <grepfa_uuid.h>
#include <grepfa_mqtt.h>
#include <esp_wifi_types.h>
#include <networking_util.h>
#include <grepfa_mqtt_device.h>

static const char* TAG = "app";

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32" bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    vTaskDelay(1000/portTICK_PERIOD_MS);

    GrepfaWiFiInit();

    GrepfaWiFiSTA();

    GrepfaWiFiStartScan(true, false, true);

    wifi_ap_record_t records[16];
    uint16_t scanned;

    GrepfaWiFiGetScanRecord(16, &scanned, records);
    GrepfaWiFiStopScan();

    for (int i = 0; i < scanned; ++i) {
        ESP_LOGI(TAG, "wifi %d ->", i);
        ESP_LOGI(TAG, "SSID: %s", records[i].ssid);
        ESP_LOGI(TAG, "AUTH: %s", auth_mode_str(records[i].authmode));
        puts("=================");
    }

    GrepfaWiFiConnectSTA("sys2.4G", "shin0114", true, 15);

    char client_id[UUID_STR_LEN];
    random_uuid(client_id);

    GrepfaMqttConnectorV1_t *conn = GrepfaMqttConnectorNew("mqtts://a2bp9adt6od3cn-ats.iot.ap-northeast-2.amazonaws.com", client_id, true);

    GrepfaDeviceV1_t vDev;

    random_uuid(client_id);
    GrepfaMqttDeviceSet(&vDev, client_id, "sensor");
    GrepfaMqttDeviceSetConnector(&vDev, conn);

    GrepfaDevicePayloadV1_t payloads[2];

    for (int i = 0; i < 2; ++i) {
        char itoaBuf[10];
        GrepfaDevicePayloadV1Set(&payloads[i], i, "xxx", itoa(i, itoaBuf, 10));
    }

    char* jsonStr = GrepfaMqttDeviceGetJsonPayload(&vDev, payloads, 2);

    ESP_LOGI(TAG, "%s", jsonStr);

    while (1){
        ESP_LOGI(TAG, "hoho");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

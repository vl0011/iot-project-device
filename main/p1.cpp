#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <esp_log.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include <networking.h>

static const char* TAG = "app";

extern "C" void app_main(void)
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


    WiFi::init();
    WiFi::start_sta();

    WiFi::scan_start(true, false, true);

    wifi_ap_record_t records[16];
    uint16_t scanned;

    WiFi::scan_get_record(16, &scanned, records);
    WiFi::scan_stop();

    for (int i = 0; i < scanned; ++i) {
        ESP_LOGI(TAG, "wifi %d ->", i);
        ESP_LOGI(TAG, "SSID: %s", records[i].ssid);
        ESP_LOGI(TAG, "AUTH: %s", auth_mode_str(records[i].authmode));
        puts("=================");
    }

    WiFi::connect_sta("sys2.4G", "shin0114", true, 15);



    while (1){
        ESP_LOGI(TAG, "hoho");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

//
// Created by vl0011 on 23. 6. 8.
//


#include <stdint.h>
#include <string.h>
#include <esp_log.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "grepfa_sntp.h"

#include <grepfa_wifi.h>
#include <grepfa_uuid.h>
#include <grepfa_mqtt.h>
#include <esp_wifi_types.h>
#include <networking_util.h>
#include <grepfa_mqtt_device.h>

static const char* TAG = "bootstrap";

void bootstrap() {
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

    vTaskDelay(100/portTICK_PERIOD_MS);


}
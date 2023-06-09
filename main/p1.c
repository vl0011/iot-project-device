#include <bootstrap.h>
#include <app_config.h>

#include <stdint.h>
#include <string.h>
#include <esp_log.h>
#include "esp_system.h"
#include "esp_event.h"
#include "grepfa_sntp.h"

#include <grepfa_wifi.h>
#include <grepfa_neopixel.h>
#include <grepfa_mqtt.h>
#include <esp_wifi_types.h>
#include <networking_util.h>
#include <grepfa_mqtt_device.h>
#include <grepfa_payload_v1.h>
#include <grepfa_ping.h>

static const char* TAG = "app";

static void pingTestTask(void* arg);
static void rssiTestTask(void* arg);
static void LedCallback(const GrepfaPayloadData_t * payload);

void app_main(void)
{

    bootstrap();

    GrepfaNeoPixelInit();

    GrepfaNeoPixelOn(100, 0, 0);
    GrepfaWiFiInit();
    GrepfaWiFiSTA();

    GrepfaNeoPixelOn(0, 100, 0);

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

    GrepfaNeoPixelOn(0, 0, 100);
    GrepfaWiFiConnectSTA(APP_CONFIG_SSID, APP_CONFIG_PASSWORD, true, 15);

    GrepfaNeoPixelOn(0, 100, 100);
    GrepfaSntpObtainTime();

    const char* devId = APP_CONFIG_DEV_ID;

    GrepfaNeoPixelOn(100, 100, 0);
    GrepfaMqttConnectorV1_t *conn = GrepfaMqttConnectorNew(APP_CONFIG_MQTT_BROKER_SERVER_ENDPOINT, devId, true);

    GrepfaNeoPixelOff();
    // Ping, RSSI task create
    xTaskCreate(pingTestTask, "pingTestTask", 4096, conn, 10, NULL);
    xTaskCreate(rssiTestTask, "rssiTestTask", 4096, conn, 10, NULL);

    // led
    GrepfaDeviceV1_t ledVDev;
    GrepfaMqttDeviceSet(&ledVDev, APP_CONFIG_LED_VDEV_ID, LedCallback);
    GrepfaMqttConnectorAddDevice(conn, &ledVDev, 1);


    while (1){
        ESP_LOGI(TAG, " ");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void pingTestTask(void* arg) {
    const char* id = APP_CONFIG_PING_VDEV_ID;

    GrepfaDeviceV1_t vDev;
    GrepfaMqttConnectorV1_t * conn = arg;
    GrepfaMqttDeviceSet(&vDev, id, NULL);
    GrepfaMqttConnectorAddDevice(conn, &vDev, 1);
    char itoaBuf[100];

    GrepfaPing_t req = GrepfaPingDefaultReq();
    GrepfaPingResult_t res;
    while (1) {
        ESP_LOGI(TAG, "ping test...");
        GrepfaPingStart("a2bp9adt6od3cn-ats.iot.ap-northeast-2.amazonaws.com", true, &req, &res);
        int avgPing = res.total_time_ms / 5;

        GrepfaPayloadData_t * d = NewGrepfaPayloadData(id, 0);

        itoa(avgPing, itoaBuf, 10);
        AddGrepfaPayloadValue(d, 0, "pingAvg", itoaBuf);

        itoa(res.loss, itoaBuf, 10);
        AddGrepfaPayloadValue(d, 0, "pingLoss", itoaBuf);
        GrepfaMqttConnectorPublish(conn, &vDev, d, 1);

        DeleteGrepfaPayloadData(d);
    }
}

static void rssiTestTask(void* arg) {
    const char* id = APP_CONFIG_RSSI_VDEV_ID;

    GrepfaDeviceV1_t vDev;
    GrepfaMqttConnectorV1_t * conn = arg;
    GrepfaMqttDeviceSet(&vDev, id, NULL);
    char itoaBuf[100];

    GrepfaMqttConnectorAddDevice(conn, &vDev, 1);

    wifi_ap_record_t currentWiFiInfo;

    while (1) {
        ESP_LOGI(TAG, "rssi test...");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        GrepfaWiFiSTAGetApInfo(&currentWiFiInfo);
        GrepfaPayloadData_t * d = NewGrepfaPayloadData(id, 0);
        itoa(currentWiFiInfo.rssi, itoaBuf, 10);
        AddGrepfaPayloadValue(d,0, "rssi", itoaBuf);
        GrepfaMqttConnectorPublish(conn, &vDev, d, 1);
        DeleteGrepfaPayloadData(d);
    }
}

void LedCallback(const GrepfaPayloadData_t *payload) {
    char *r = (payload[0].values[0].value);
    char *g = (payload[0].values[1].value);
    char *b = (payload[0].values[2].value);

    int ir = atoi(r);
    int ig = atoi(g);
    int ib = atoi(b);

    GrepfaNeoPixelOn(ir, ig, ib);

    vTaskDelay(10 / portTICK_PERIOD_MS);
}

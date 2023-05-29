#include <sdkconfig.h>
#include <esp_err.h>

// check Wi-Fi ssid
#ifndef CONFIG_WIFI_SSID
#error WiFi SSID not set
#endif

// check Wi-Fi pw
// TODO:: none pw wifi
#ifndef CONFIG_WIFI_PASSWORD
#error WiFi password not set
#endif

esp_err_t start_connection();
esp_err_t stop_connection();



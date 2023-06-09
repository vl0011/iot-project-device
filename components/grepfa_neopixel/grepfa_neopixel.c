//
// Created by vl0011 on 23. 6. 9.
//

#include "led_strip.h"
#include <grepfa_neopixel.h>
#include "driver/gpio.h"

static int targetGpio = 48;
static led_strip_config_t strip_config;
static led_strip_rmt_config_t rmt_config;
static led_strip_handle_t led_strip;

void GrepfaNeoPixelSetGPIO(int gpio) {
    targetGpio = gpio;
}

void GrepfaNeoPixelInit() {
    strip_config.strip_gpio_num = targetGpio;
    strip_config.max_leds = 1;
    rmt_config.resolution_hz = 10 * 1000 * 1000;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);
}

void GrepfaNeoPixelOn(int r, int g, int b) {
    led_strip_set_pixel(led_strip, 0, r, g, b);
    led_strip_refresh(led_strip);
}

void GrepfaNeoPixelOff() {
    led_strip_clear(led_strip);
}
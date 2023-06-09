//
// Created by vl0011 on 23. 6. 9.
//

static char* TAG = "grepfa_ping";

#include <grepfa_ping.h>

#include <esp_log.h>
#include "sdkconfig.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"
#include "esp_event.h"

static void ping_end(esp_ping_handle_t hdl, void *args)
{
    GrepfaPingResult_t* res = args;

    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    uint32_t loss;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));

    if (transmitted > 0) {
        loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    } else {
        loss = 0;
    }

    if (IP_IS_V4(&target_addr)) {
        char* ip4 = inet_ntoa(*ip_2_ip4(&target_addr));
        ESP_LOGI(TAG, "--- %s ping statistics ---\n", ip4);
        res->addr = ip4;
    } else {
        char* ip6 = inet6_ntoa(*ip_2_ip6(&target_addr));
        ESP_LOGI(TAG, "--- %s ping statistics ---\n", ip6);
        res->addr = ip6;
    }
    ESP_LOGI(TAG, "%" PRIu32 " packets transmitted, %" PRIu32 " received, %" PRIu32 "%% packet loss, time %" PRIu32 "ms\n",
           transmitted, received, loss, total_time_ms);
    esp_ping_delete_session(hdl);

    res->loss = loss;
    res->received = received;
    res->total_time_ms = total_time_ms;
    res->transmitted = transmitted;
    memcpy(&res->ip, &target_addr, sizeof(ip_addr_t));

    if(res->waitSem) {
        xSemaphoreGive(res->waitSem);
    }

    ESP_LOGI(TAG, "end ping test");

    if (res->cb != NULL) {
        res->cb(*res);
    }
}




int GrepfaPingStart(char *addr, bool wait, GrepfaPing_t * req, GrepfaPingResult_t* result) {
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();

    if (req->count > 0) {
        config.count = req->count;
    }
    if(req->timeout > 0) {
        config.timeout_ms = req->timeout;
    }
    if(req->interval > 0) {
        config.interval_ms = req->interval;
    }
    if(req->ttl > 0) {
        config.ttl = req->ttl;
    }

    struct sockaddr_in6 sock_addr6;
    ip_addr_t target_addr;
    memset(&target_addr, 0, sizeof(target_addr));

    if (inet_pton(AF_INET6, addr, &sock_addr6.sin6_addr) == 1) {
        /* convert ip6 string to ip6 address */
        ipaddr_aton(addr, &target_addr);
    } else {
        struct addrinfo hint;
        struct addrinfo *res = NULL;
        memset(&hint, 0, sizeof(hint));
        /* convert ip4 string or hostname to ip4 or ip6 address */
        if (getaddrinfo(addr, NULL, &hint, &res) != 0) {
            printf("ping: unknown host %s\n", addr);
            return 1;
        }
        if (res->ai_family == AF_INET) {
            struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
            inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
        } else {
            struct in6_addr addr6 = ((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr;
            inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
        }
        freeaddrinfo(res);
    }

    config.target_addr = target_addr;

    esp_ping_callbacks_t cbs = {
            .cb_args = result,
            .on_ping_success = NULL,
            .on_ping_timeout = NULL,
            .on_ping_end = ping_end
    };
    esp_ping_handle_t ping;
    esp_ping_new_session(&config, &cbs, &ping);
    esp_ping_start(ping);

    result->wait = wait;
    if (wait) {
        ESP_LOGI(TAG, "waiting ping request...");
        result->waitSem = xSemaphoreCreateBinary();
        if (result->waitSem == NULL) {
            ESP_LOGE(TAG, "ESP_ERR_NO_MEM");
            return 1;
        }

        xSemaphoreTake(result->waitSem, portMAX_DELAY);
    }

    if(result->waitSem) {
        vSemaphoreDelete(result->waitSem);
    }

    return 0;
}
//
// Created by vl0011 on 23. 6. 9.
//

#ifndef P1_GREPFA_PING_H
#define P1_GREPFA_PING_H

#include <stdbool.h>
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"

#define GrepfaPingDefaultReq()   {\
    .count = 0,                   \
    .timeout = 0,                 \
    .interval = 0,                \
    .ttl = 0,                     \
}

typedef struct grepfa_ping_t {
    int count;
    int timeout;
    int interval;
    int ttl;
}GrepfaPing_t;
typedef struct grepfa_ping_result_t GrepfaPingResult_t;
typedef void (*GrepfaPingCallback) (GrepfaPingResult_t result);
struct grepfa_ping_result_t {
    char* addr;
    ip_addr_t ip;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    uint32_t loss;
    GrepfaPingCallback cb;
    SemaphoreHandle_t waitSem;
    bool wait;
};

int GrepfaPingStart(char* addr, bool wait, GrepfaPing_t * req, GrepfaPingResult_t* res);




#endif //P1_GREPFA_PING_H

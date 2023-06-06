idf_component_register(SRCS "networking_util.c" "grepfa_wifi.c"
                    INCLUDE_DIRS "include"
        PRIV_REQUIRES esp_netif esp_wifi)

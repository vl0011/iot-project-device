idf_component_register(SRCS "wifi_manager.c"
                    INCLUDE_DIRS "include"
        PRIV_REQUIRES esp_netif driver esp_wifi vfs)

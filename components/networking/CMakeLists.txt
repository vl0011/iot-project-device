idf_component_register(SRCS "networking_wifi.cpp" "networking_util.cpp"
                    INCLUDE_DIRS "include"
        PRIV_REQUIRES esp_netif driver esp_wifi vfs)

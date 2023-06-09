idf_component_register(SRCS "grepfa_mqtt.c" "grepfa_mqtt_device.c" "grepfa_mqtt_event.c"
                    INCLUDE_DIRS "include"
        PRIV_REQUIRES mqtt grepfa_uuid json grepfa_hashmap esp_hw_support grepfa_payload)

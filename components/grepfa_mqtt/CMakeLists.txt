set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

idf_component_register(SRCS "grepfa_mqtt.c" "grepfa_mqtt_device.c" "grepfa_mqtt_payload.c"
                    INCLUDE_DIRS "include"
        PRIV_REQUIRES mqtt grepfa_uuid json)

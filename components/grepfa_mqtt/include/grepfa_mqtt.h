//
// Created by vl0011 on 23. 6. 1.
//

#ifndef P1_GREPFA_MQTT_H
#define P1_GREPFA_MQTT_H

#include <cstdint>
#include <mqtt_client.h>
#include <unordered_map>
#include <vector>


#ifdef GREPFA_MQTT_CONFIGURATION_CERTIFICATION_FROM_ASM
extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");

extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");

extern const uint8_t server_cert_pem_start[] asm("_binary_aws_crt_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_aws_crt_end");
#endif


class ChannelValue{
    int channel;
    char* type;
    char* value;

public:
    ChannelValue(int channel, char *type, char *value);

};

class MessageBase{
private:
    time_t timestamp;
    char* things_id;
    char* payload_id;
    std::vector<ChannelValue> channel;
};

enum class DeviceType {
    MANAGEMENT,
    SENSOR,
    ACTUATOR,

};

// 가상 기기 개체
class GrepfaDeviceV1{
private:
    // device unique id
    char id[255];
public:
    const char *getId() const;

    void sendToServer(std::vector<ChannelValue> values);
    void sendToServerAsync(std::vector<ChannelValue> values);


};

class GrepfaMQTTConnectorV1 {
private:

    char id[255];

    const char* rootTopic = "grepfa/v1";
    esp_mqtt_client_handle_t clientHandle;

    std::unordered_map<const char*, GrepfaDeviceV1*> contains;

    esp_mqtt_client_config_t cfg = {
            .broker = {
                    .verification= {
                            .certificate = (const char*) server_cert_pem_start,
                    }
            },
            .credentials = {
                .authentication {
                    .certificate = (const char*)client_cert_pem_start,
                    .key = (const char*)client_key_pem_start
                }
            }
    };

    static void mqtt_event_handler(void* args, esp_event_base_t base, int32_t event_id, void* data);

protected:
    void mqtt_event_router_connected(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
    void mqtt_event_router_disconnected(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
    void mqtt_event_router_subscribe(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
    void mqtt_event_router_unsubscribe(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
    void mqtt_event_router_published(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
    void mqtt_event_router_data(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
    void mqtt_event_router_error(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
//    void mqtt_event_router_deleted(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);
//    void mqtt_event_router_unknown(GrepfaMQTTConnectorV1* it, esp_mqtt_event_handle_t ev_handler);

public:
    void AddDevice(GrepfaDeviceV1* device);
    GrepfaMQTTConnectorV1(const char* endpoint, const char* id);
};


#endif //P1_GREPFA_MQTT_H

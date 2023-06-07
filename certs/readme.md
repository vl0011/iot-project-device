# certs
SSL 세션을 생성하기 위한 인증서 저장 폴더 입니다.  
다음 파일이 필요합니다.
- root_cert_auth.crt Root CA 인증서, AWS의 키를 사용합니다.
- client.crt 클라이언트 공개 키, AWS IoT Core에 발급 받아 사용해야 합니다.
- client.key 클라이언트 비밀 키, AWS IoT Core에 발급 받아 사용해야 합니다.

## CA 정보
CN=AWS IoT Certificate  
OU=Amazon Web Services  
O=Amazon.com Inc.  
L=Seattle  
ST=Washington  
C=US  

## 링킹
프로젝트 루트 [CMakeLists.txt](../CMakeLists.txt) 의 `target_add_binary_data` CMake 함수로 바이너리에 인증서 데이터가 링킹 됩니다.

## 사용
[/components/grepfa_mqtt/grepfa_mqtt.c](../components/grepfa_mqtt/grepfa_mqtt.c)
```c
#ifdef GREPFA_MQTT_CONFIGURATION_CERTIFICATION_FROM_ASM
extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");

extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");

extern const char root_cert_auth_start[] asm("_binary_root_cert_auth_crt_start");
extern const char root_cert_auth_end[]   asm("_binary_root_cert_auth_crt_end");
#endif
```
`..._start[]` 에 바이너리의 인증서 텍스트 시작 포인터가 저장됩니다.
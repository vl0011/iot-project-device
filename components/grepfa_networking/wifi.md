# grepfa_networking - wifi 구현

STA 모드로 외부 AP와 ESP를 연결하는 컴포넌트 입니다.

현재 ipv4 만 지원 됩니다.

## 기반 라이브러리
- [esp_netif](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32s3/api-reference/network/esp_netif.html)  
스레드 세이프한 TCP/IP 응용 프로그램 계층에 대한 추상화 제공 라이브러리
- [esp_wifi](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32s3/api-reference/network/esp_wifi.html)  
wifi 연결을 수행하는 라이브러리

## grepfa_wifi.h
### void GrepfaWiFiInit();
wifi 사용을 위한 기기 초기화 함수

### void GrepfaWiFiStop();
wifi 사용을 중지

### void GrepfaWiFiSTA();
STA 모드롤 wifi 초기화

### esp_err_t GrepfaWiFiDisconnectSTA();
STA 연결을 종료

### esp_err_t GrepfaWiFiConnectSTA(const char* SSID, const char* password, bool wait, int retry);
- SSID
- password
- wait wifi 연결이 완료 될때까지 대기
- retry 연결 재시도

### void GrepfaPrintAllIPs();
- **networking_util 로 이동 예정**
할당된 모든 ip를 출력

### esp_err_t GrepfaWiFiStartScan(bool wait, bool showHidden, bool activeScan);
- wait 스캔이 완료될 때 까지 대기
- showHidden 숨겨진 SSID 스캔 여부
- activeScan

주변 AP를 스캔

### esp_err_t GrepfaWiFiStopScan();
스캔을 종료 합니다.

### esp_err_t GrepfaWiFiGetScanRecord(uint16_t num, uint16_t * found, wifi_ap_record_t* records);
- num 가져올 스캔 레코드 수
- found 찾은 레코드 수
- records 레코드를 저장할 배열
# grepf_sntp
ntp 서버에서 시간을 받아와 하드웨어 RTC에 시간을 동기화 하는 컴포넌트 입니다.

sntp 프로토콜을 사용 합니다.

DHCP 제공 타임 서버와 "time.windows.com", "pool.ntp.org" NTP 서버에서 시간을 받아옵니다.

현재 이 컴포넌트는 WiFi 연결 후에만 사용 가능 합니다.

## 기반 라이브러리
- [esp netif](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32s3/api-reference/network/esp_netif.html)  
스레드 세이프한 TCP/IP 응용 프로그램 계층에 대한 추상화 제공 라이브러리

## <grepfa_sntp.h>

### #define GREPFA_SNTP_CONFIGURATION_SNTP_SERVER_POOL "time.windows.com", "pool.ntp.org"
사용할 타임서버 리스트

### #define GREPFA_SNTP_CONFIGURATION_SNTP_SERVER_POOL_NUM 2
타임서버의 개수

### #define GREPFA_SNTP_CONFIGURATION_SMOOTH_SYNC false
SMOOTH_SYNC method 사용 여부

### void GrepfaSntpObtainTime();
RTC 에 시간을 설정 합니다.


## TODO
- 이더넷 대응
- KConfig
  - 타임서버 설정
  - 
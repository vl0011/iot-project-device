# grepfa_uuid

## 기반 라이브러리
- esp_hw_support  
ESP 하드웨어 난수 생성 [API](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32s3/api-reference/system/random.html)

## 기반 소스코드
- [더쿨라이프 블로그](https://thecoollife.tistory.com/2457)
- [digipine 블로그](http://www.digipine.com/index.php?mid=clan&document_srl=42035)

[RF 노이즈 엔트로피 기반 랜던 생성기](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#rng) 로 랜덤 UUID를 생성 합니다.

[WiFi 혹은 블루투스 시스템이 활성화 되어야](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32s3/api-reference/system/random.html) 실제 하드웨어 난수가 생성 됩니다.


## <grepfa_uuid.h>
### #define UUID_STR_LEN 37
UUID 문자열 길이 정의

### void random_uuid(char* dest);
dest 에 랜덤 UUID 값을 체워줍니다.

dest 는 `UUID_STR_LEN` 보다 길어야 합니다.

### void random_u128(uint64_t* word);
16 바이트 바이너리(uint64_t[2]) 데이터에 랜덤 데이터를 체웁니다.

### void bin_to_uuid(uint64_t* word, char* dest);
16 바이트 바이너리(uint64_t[2]) 데이터를 UUID로 변환 하여 dest에 체웁니다.


## TODO
- char* uuid to bin
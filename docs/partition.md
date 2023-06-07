# 파티션 정의
| Name     | Type | SubType | Offset   | Size | Flags |
|----------|------|---------|----------|------|-------|
| nvs      | data | nvs     |          | 16K  |       |
| otadata  | data | ota     | 0xD000   | 8K   |       |
| phy_init | data | phy     | 0xF000   | 4K   |       |
| ota_0    | app  | ota_0   | 0x10000  | 3M   |       |
| ota_1    | app  | ota_1   | 0x310000 | 3M   |       |

현재 사용중인 칩셋의 플레시 메모리는 `8MB` 입니다.

실제 빌드된 바이너리는 `ota_*` 파티션에 저장 되며 바이너리의 최대 크기는 `0x300000`(`3MB`) 입니다.

추후 OTA 기능 구현을 위해 구성된 파티션 입니다.

사용하지 않는 남은 메모리 영역은 OTA 기능 구현을 위해 남겨논 공간 입니다.

## [partitions.csv](../partitions.csv)
ESP32 의 파티션 테이블을 정의하는 파일 입니다.

## [partitionCalculator.ods](../partitionCalculator.ods)
파티션 메모리 크기와 오프셋 계산을 위한 리브레오피스 Calc 파일 입니다.


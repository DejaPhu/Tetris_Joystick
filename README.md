# Tetris Joystick — STM32F103C8T6 + FreeRTOS

Điều khiển Tetris bằng joystick analog hai trục và nút SW, OLED SSD1306 128×64, buzzer và UART debug. Mã nền từ [Tetris-STM32F103-FreeRTOS](https://github.com/DejaPhu/Tetris-STM32F103-FreeRTOS), nhánh `feature/hal-uart-debug`, commit `36ea9cd444f3cb390b02e967306454244b80ea75` (đã bao gồm bản tích hợp `develop`). Nhánh `main` của repo nguồn tại thời điểm lấy mã chỉ có README.

Đã kiểm thử logic trên host và build/link ARM GCC trong giới hạn **64 KB Flash / 20 KB RAM** của STM32F103C8. **Chưa nạp thử board hoặc rebuild bằng Keil**; cần kiểm tra chiều joystick theo cách lắp thực tế.

## Đấu dây

| Joystick | STM32 Blue Pill | Cấu hình |
|---|---|---|
| GND | GND | Chung mass |
| +5V / VCC (tên in trên module) | **3.3V** | Cấp 3.3V để VRx/VRy không vượt mức ADC |
| VRx | PA0 | ADC1_IN0, 12 bit |
| VRy | PA1 | ADC2_IN1, 12 bit |
| SW | PB10 | Input pull-up, nhấn xuống GND |

| Thiết bị còn lại | Chân |
|---|---|
| Nút PAUSE | PA8 → GND |
| Nút ROTATE bổ sung, tùy chọn | PB14 → GND |
| OLED SSD1306, địa chỉ 7-bit 0x3C | PB8 SCL / PB9 SDA, I2C1 remap, pull-up 3.3V |
| Buzzer active kích LOW (`低电平触发`) | IO → PB6, GPIO output: HIGH tắt / LOW kêu |
| USB–UART TTL 3.3V | PA9 TX → RX adapter; GND chung; PA10 RX tùy chọn |
| ST-Link SWD | PA13 SWDIO / PA14 SWCLK / GND |

PB12/PB13/PB15 không cần nút nữa; mặc định phần mềm không đọc ba nút cũ này. PB10 được dùng trực tiếp cho SW, không cần thêm nút hard drop. Xem [hướng dẫn joystick](docs/JOYSTICK.md) và [pin mapping](docs/PIN_MAPPING.md).

Buzzer đã sửa cho module kích LOW; xem [đấu nối, mức tắt và kiểm thử](docs/BUZZER.md).

## Điều khiển

| Thao tác | Khi chơi | Trong menu |
|---|---|---|
| Gạt trái | Di chuyển trái | Back |
| Gạt phải | Di chuyển phải | OK |
| Gạt lên | Xoay một lần mỗi lần gạt | Chọn mục trên |
| Gạt xuống | Soft drop, giữ để lặp | Chọn mục dưới |
| Nhấn SW | Hard drop một lần mỗi lần nhấn | OK |
| Nút PAUSE | Pause/Resume | Back; ở màn PAUSED thì Resume |
| Nút ROTATE tùy chọn | Xoay | Chọn mục trên |

Nhả cần về tâm rồi gạt lại để chọn mục kế tiếp trong menu. Trái tại PAUSED trở về game; trái tại WIN/GAME_OVER trở về menu chính. Ở HELP, SW cũng quay về menu chính.

## Mở và nạp bằng Keil

1. Clone repo này, checkout `main` và giữ nguyên cây thư mục.
2. Mở `Teris/MDK-ARM/Teris.uvprojx` bằng Keil µVision 5 với **Arm Compiler 5**, bật C99 như project. Port FreeRTOS hiện dùng `RVDS/ARM_CM3`.
3. Target STM32F103C8: Flash `0x08000000`, size `0x10000`; RAM `0x20000000`, size `0x5000`. Rebuild All và nạp bằng ST-Link.
4. Để joystick ở tâm khoảng **0,32 giây sau khi scheduler bắt đầu**. Nhấn SW để vào menu; chọn độ khó rồi chơi.
5. Nếu chiều ngược hoặc trục bị đổi, chỉnh `JOYSTICK_INVERT_X`, `JOYSTICK_INVERT_Y`, `JOYSTICK_SWAP_XY` trong [joystick_config.h](app/inc/joystick_config.h), build và nạp lại. Mặc định X thấp = trái, Y thấp = lên.

Không cần generate lại CubeMX. File `Teris/Teris.ioc` đã đồng bộ ADC với phần cứng trong `Joystick.zip`; cây thư mục đã được tổ chức lại so với CubeMX mặc định, xem [lưu ý regenerate](docs/JOYSTICK.md#cubemx-và-mã-nguồn).

## Xử lý input và game

- InputTask mỗi 10 ms đọc hai ADC, hiệu chỉnh tâm từ 32 mẫu, vùng kích hoạt ±700 và vùng nhả ±400 trên thang 0–4095. Gạt chéo khóa theo một trục; không đồng thời xoay và dịch ngang.
- Debounce 30 ms. Giữ trái/phải/xuống lặp sau 300 ms, tiếp theo mỗi 100 ms; menu bỏ sự kiện repeat. Gạt lên và SW không tự lặp.
- Lỗi đọc ADC không tạo sự kiện hướng mới; cần về tâm trước khi nhận lại hướng. PAUSE, SW và ROTATE vẫn hoạt động. Queue đầy: giữ một press chờ cho mỗi mã, ưu tiên PAUSE; repeat có thể bị bỏ và được thống kê.
- Giữ nguyên **3 application Tasks, 2 Queues**, game engine, điểm số và kiến trúc snapshot. ADC polling có timeout 1 ms mỗi trục; không dùng DMA hay thêm Task.
- Bàn 10×20, 7 khối, xoay/wall-kick đơn giản, xóa hàng, next preview, pause/retry/help. EASY 800 ms, NORMAL 500 ms, HARD 250 ms; WIN khi score ≥1000.

## Kiểm tra trên máy

```sh
python3 tests/check_project.py
./tests/run_host.sh
python3 tools/build_gcc.py  # arm-none-eabi-gcc + newlib trong PATH
```

Host tests dùng AddressSanitizer/UndefinedBehaviorSanitizer: game/FSM, UART, hiệu chỉnh/hysteresis joystick, lỗi ADC và luồng InputTask → Queue. ARM GCC dùng startup/linker/port riêng trong `tools/`, tạo `build/gcc/tetris.elf` và `tetris.hex`; đây là build bổ sung, không thay kết quả Keil. Xem [kết quả kiểm thử](docs/test-results/JOYSTICK.md).

UART 115200 8N1 ghi `JOY_RAW`, `JOY_CENTER`, `KEY_PRESS`, `KEY_ENQ`, `KEY_RX`, trạng thái game và telemetry. Xem [UART debug](docs/UART_DEBUG.md). Các tài liệu test cũ được giữ như lịch sử bản nền.

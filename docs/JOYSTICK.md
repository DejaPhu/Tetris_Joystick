# Tích hợp joystick

## Nguồn tham khảo

- Game nền: `DejaPhu/Tetris-STM32F103-FreeRTOS`, commit `36ea9cd444f3cb390b02e967306454244b80ea75`, nhánh `feature/hal-uart-debug`.
- File người dùng cung cấp: `Joystick.zip`, SHA-256 `adbdb1c59bd54380170297dd21f16b14ca3b986944091e958f9a75965945c6ed`.
- ZIP là project CubeMX với ADC1_IN0 PA0 và ADC2_IN1 PA1; không chứa thư viện xử lý joystick riêng. Bản này kế thừa cấu hình ADC/MSP đó, bổ sung driver và filter riêng; không nhập các file build, object hoặc IDE cá nhân từ ZIP.
- ADC2 được tăng sampling time từ 1.5 lên 55.5 chu kỳ giống ADC1; clock ADC = 72 MHz / 6 = 12 MHz, trong giới hạn STM32F103. Cả hai ADC được hardware calibration trước khi khởi động RTOS.

## Hiệu chỉnh và hướng

Cấp module bằng 3.3V, VRx→PA0, VRy→PA1, SW→PB10; PAUSE→PA8 và ROTATE tùy chọn→PB14, các nút nối GND. Giữ cần ở tâm trong 32 lần lấy mẫu đầu (~320 ms). Nếu tất cả mẫu trong 1500–2600 và độ dao động mỗi trục không quá 120, tâm là trung bình 32 mẫu. Nếu đang gạt hoặc chuyển động lúc boot, dùng tâm mặc định 2048 và chờ cần trở lại vùng tâm mới nhận điều khiển. Module có tâm lệch ngoài phạm vi này cần điều chỉnh cấu hình sau khi xem log.

Tham số trong `app/inc/joystick_config.h`:

| Tham số | Mặc định | Tác dụng |
|---|---|---|
| `JOYSTICK_SWAP_XY` | 0 | Đổi hai trục trước hiệu chỉnh |
| `JOYSTICK_INVERT_X` | 0 | Đảo trái/phải |
| `JOYSTICK_INVERT_Y` | 0 | Đảo lên/xuống |
| `JOYSTICK_LEGACY_BUTTONS` | 0 | Đặt 1 để dùng thêm PB12/PB13/PB15 |
| `JOYSTICK_ENTER` | 700 | Lệch tâm tối thiểu để kích hoạt |
| `JOYSTICK_RELEASE` | 400 | Lệch tâm để nhả, phải nhỏ hơn ENTER |

Bật UART 115200, thử từng hướng. `JOY_RAW a` là ADC PA0, `b` là ADC PA1, `c=1` khi đọc thành công. X thấp mặc định là trái, Y thấp là lên. Nếu gạt lên lại đi ngang, đổi `SWAP_XY`; nếu chiều ngược, đổi cờ invert tương ứng. `JOY_CENTER` báo tâm sau đổi trục; đây là ADC trước invert, còn `c` báo đã sẵn sàng nhận hướng hay chưa.

Filter chọn trục có độ lệch lớn hơn; nếu bằng nhau chọn X. Giữ trục đó tới khi nó vào vùng nhả, rồi mới chọn lại. Gạt lên và SW tạo một press sau debounce; cần nhả ổn định 30 ms để nhận lần nhấn tiếp. Không tự lặp trong menu.

ADC polling có timeout 1 ms/trục, dừng conversion sau mỗi lần đọc. Khi start/poll/stop lỗi, không trả mẫu nửa cặp; filter nhả hướng và chỉ hoạt động lại sau khi về tâm. Đây không phải phát hiện đứt dây: chân analog bị tháo có thể trôi tới giá trị hợp lệ. Phải nối đủ VRx/VRy/GND trước khi chơi.

## Các module

| File | Trách nhiệm |
|---|---|
| `main.c`, `bsp/src/stm32f1xx_hal_msp.c` | Khởi tạo ADC1/ADC2, clock, PA0/PA1 analog, hardware calibration |
| `hal/src/hal_joystick.c` | Đọc cặp ADC có timeout; InputTask là chủ sở hữu |
| `app/src/joystick_filter.c` | Hiệu chỉnh tâm, hysteresis, chọn một hướng, rearm |
| `app/src/task_input.c` | Ghép hướng và nút, debounce/repeat, gửi mã `uint16_t` vào Queue |
| `app/src/game_fsm.c` | Dùng mã điều khiển chung theo màn hình; bổ sung LEFT để Back ở PAUSED/WIN/GAME_OVER |
| `app/src/ui_renderer.c` | Hướng dẫn OLED dùng UP/SW |

Không đổi kích thước Queue hay quyền sở hữu game. `BTN_ROTATE` đồng thời đại diện gạt lên; `BTN_DROP` là SW. `hal_gpio` vẫn giữ mapping nút cũ để cho phép tương thích bằng cờ cấu hình. SW PB10 và nút hard drop cũ dùng cùng chân; không cần lắp cả hai.

## CubeMX và mã nguồn

`Teris/Teris.ioc` giữ cấu hình Tasks/Queues/UART IRQ của bản nền và bổ sung ADC từ ZIP. Keil trỏ tới `main.c` ở gốc và `bsp/`, `hal/`, `app/`, `middlewares/`. CubeMX mặc định tạo `Core/Src` và `Drivers`; generate trực tiếp rồi build mà không tích hợp lại sẽ bỏ qua nguồn tùy chỉnh. Nếu cần regenerate, thực hiện vào thư mục tạm, đối chiếu ADC/MSP/HAL config, giữ wrapper `task_*_run`, queue contract và USART1 IRQ tùy chỉnh; kiểm tra lại bằng `tests/check_project.py`.

## Thử trên board

1. Build Keil, kiểm tra `.map` không vượt 64 KB Flash / 20 KB RAM, nạp ST-Link.
2. Reset khi cần ở tâm: xem `JOY_RAW` hai trục gần tâm, `JOY_CENTER c=1`; đứng yên không phát `KEY_PRESS`.
3. SW vào menu; lên/xuống chọn mục, phải/SW OK, trái/PAUSE Back. Nhả và gạt lại giữa các lần chọn.
4. Trong game: trái/phải dịch và giữ để lặp; lên xoay đúng một lần; xuống soft drop; SW hard drop một lần dù giữ.
5. PAUSE rồi lên/xuống chọn Resume/Restart/Main Menu; trái hoặc PAUSE tiếp tục chơi. Nút PB14 xoay bổ sung nếu có.
6. Reset trong khi gạt cần: không tự chọn menu, đưa về tâm rồi thử lại. Thử gạt chéo và xem không có hai hướng cùng lúc.
7. Chơi liên tục, theo dõi stack/heap/Queue/UART; đo chu kỳ InputTask bằng debugger/logic analyzer nếu cần. Kết quả timing và chiều vật lý chỉ được xác nhận sau các bước này.

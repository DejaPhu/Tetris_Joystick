# USART1 debug — hỗ trợ tìm lỗi, không tự sửa lỗi

## Nối máy tính

Dùng USB–UART TTL mức logic **3,3 V** (CP2102/CH340/FT232 có mức logic phù hợp). Không dùng cổng RS-232 điện áp ±12 V.

| USB–UART | Blue Pill |
|---|---|
| RXD | PA9 / USART1_TX |
| GND | GND |
| TXD | PA10 / USART1_RX, tùy chọn vì hiện chưa nhận lệnh |

Nếu board đã có nguồn riêng, không nối thêm VCC của adapter. Chọn đúng COM trong PuTTY/Tera Term/terminal tương đương: **115200 baud, 8 data bits, no parity, 1 stop bit, flow control None**. Mở terminal rồi Reset board để thấy BOOT. Đấu TX/RX chéo, không TX–TX. UART chỉ xuất chẩn đoán; nhập ký tự từ máy tính chưa điều khiển game.

Không cần generate lại CubeMX để dùng checkout này: USART1 đã cấu hình, ISR/source/Keil đã nối. File .ioc đã bổ sung USART1 IRQ priority 6; nếu Leader regenerate cần giữ ISR tùy biến và cấu trúc repo hiện tại.

## Cách đọc log

Mỗi dòng là `t=<HAL tick ms> EVENT a=<số> b=<số> c=<số>`, kết thúc CRLF. Các giá trị là số thập phân. Dưới đây là ví dụ minh họa, chưa phải log thu từ kit:

```text
t=12 BOOT a=115200 b=3 c=2
t=12 GAME a=0 b=0 c=1
t=840 KEY_PRESS a=6 b=5 c=840
t=840 KEY_ENQ a=6 b=5 c=0
t=850 KEY_RX a=6 b=0 c=0
t=850 GAME a=1 b=0 c=1
```

| Event | a | b | c |
|---|---|---|---|
| BOOT | baud 115200 | 3 application Tasks | 2 Queues |
| KEY_PRESS | mã nút sau debounce | index điều khiển 0..5 | thời điểm nhấn được chấp nhận |
| KEY_ENQ | mã đã gửi Queue thành công | index điều khiển | 0 |
| KEY_RX | mã GameTask vừa nhận | state trước xử lý | score trước xử lý |
| GAME | state mới | score | difficulty |
| JOY_RAW | ADC PA0 0..4095 | ADC PA1 0..4095 | 1 cặp mẫu hợp lệ, 0 lỗi ADC |
| JOY_CENTER | tâm X sau swap | tâm Y sau swap | 1 đã về tâm và sẵn sàng, 0 chưa sẵn sàng |
| HEAP | free bytes | minimum-ever free bytes | 0 |
| STACK_WORDS | Input free words | Game free words | Display free words |
| INPUT_QUEUE | số lượt gửi press chưa được do Queue đầy | số event bị bỏ | 0 |
| TIMING_OLED | Game max ms | flush OLED max ms | tổng lỗi OLED |
| UART_TX | số dòng bị bỏ do UART chưa sẵn sàng/quá đầy/quá dài | số byte đã nạp thanh ghi TX | 0 |
| FAULT | reason 1 RTOS/2 malloc/3 stack/4 PWM | 0 | 0 |

Mã nút: 1 LEFT, 2 RIGHT, 3 ROTATE, 4 DOWN, 5 PAUSE, 6 DROP. Index điều khiển tương ứng là 0..5; index 5 là SW PB10. Không log từng repeat để giảm lưu lượng.

State: 0 WELCOME, 1 MAIN_MENU, 2 HELP, 3 DIFFICULTY, 4 PLAYING, 5 PAUSED, 6 WIN, 7 GAME_OVER. Difficulty: 0 EASY, 1 NORMAL, 2 HARD. GAME chỉ xuất khi state hoặc score đổi; không xuất mỗi frame.

JOY_RAW/JOY_CENTER và nhóm telemetry được lấy khoảng mỗi giây. Mẫu ADC thưa có thể bỏ lỡ press ngắn; KEY_PRESS bắt theo debounce 10 ms/30 ms. STACK_WORDS nhân 4 để ra byte còn trống. Thời gian có độ phân giải 1 ms, không phải WCET chính xác. INPUT_QUEUE đếm lượt thử thất bại, không bằng số press mất. UART_TX bytes đếm byte giao cho phần cứng, không xác nhận máy tính nhận được.

## Tìm lỗi HARD DROP

1. Nhấn SW nối PB10 xuống GND ít nhất 30 ms: phải có `KEY_PRESS a=6 b=5`, sau đó `KEY_ENQ a=6`. Giữ SW không sinh thêm press; nhả rồi nhấn lại để thử lần tiếp theo.
2. Không có press: kiểm tra SW→PB10 (không phải PA10), GND chung và pull-up. Xem GPIOB IDR bit 10 trong debugger: thả = 1, nhấn = 0; CRH nibble PB10 dự kiến 8. `JOY_RAW` chỉ đo trục analog, không đo SW.
3. GPIO đúng nhưng thiếu KEY_PRESS a=6: kiểm tra debounce/chu kỳ InputTask. KEY_PRESS mà thiếu KEY_ENQ: xem INPUT_QUEUE.
4. KEY_ENQ mà thiếu KEY_RX: xem GameTask/Queue và thao tác đổi state. Khi đổi màn GameTask bỏ sự kiện tồn trong Queue để không áp dụng phím cũ vào màn mới; thử nhả/nhấn riêng từng lần.
5. KEY_RX a=6 state 4 nhưng game không hard drop: lưu log, video và firmware SHA để kiểm tra engine. KEY_RX ở state 0/1/3 phải dẫn đến GAME chuyển state tương ứng.

## Thời gian thực và quyền sở hữu UART

- Chỉ hal_uart.c điều khiển TX USART1. Không gọi xen HAL_UART_Transmit/Transmit_IT, printf/fputc hoặc DMA cho cùng USART1. RX hiện không có giao thức xử lý.
- Bộ đệm vòng cố định 512 byte (511 byte sử dụng), không dùng malloc, Task thứ tư hoặc Queue thứ ba. Copy cả dòng tối đa 128 byte trong critical section PRIMASK ngắn, phục hồi đúng trạng thái IRQ trước đó.
- TXE IRQ priority 6 gửi một byte/lượt; không gọi API FreeRTOS trong ISR. Task không chờ UART rỗng. Khi đầy bỏ cả dòng và tăng dropped; log có thể mất dưới tải cao.
- BOOT được xuất khi GameTask bắt đầu sau khởi tạo HAL/RTOS, không chứng minh mọi ngoại vi đã hoạt động. Nếu scheduler không chạy hoặc UART init thất bại trước đó có thể không có BOOT; xem Watch/ST-Link.
- FAULT chỉ best effort: drain giới hạn số vòng, không phụ thuộc HAL tick/IRQ. Không bảo đảm log khi HardFault, stack hỏng, mất clock hoặc UART chưa init; app_diagnostics.fault trong Watch là nguồn chẩn đoán dự phòng.

## Test trên kit cần ghi bằng chứng

Mở terminal trước Reset → thấy BOOT/GAME; thử bốn hướng joystick, SW, PAUSE/ROTATE và menu/gameplay; kiểm tra PB10 theo mục trên; so sánh telemetry với Keil Watch; thử chơi liên tục và kiểm tra UART dropped/heap/stack. Dùng logic analyzer đo PA9 115200 và độ trễ Task nếu cần. Chưa có các kết quả phần cứng này.

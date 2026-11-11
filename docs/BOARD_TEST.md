# Build Keil và kiểm tra trên board hiện tại

Đây là checklist **cần nhóm thực hiện**, chưa phải báo cáo phần cứng PASS. Dùng firmware joystick và buzzer active kích LOW, không dùng checklist sáu nút/PWM của bản cũ.

## Build và nạp

1. Mở toàn bộ checkout tại `MDK-ARM/Tetris_Joystick.uvprojx`, dùng Arm Compiler 5, C99 và port FreeRTOS `RVDS/ARM_CM3`. Không đổi compiler dropdown sang 6 rồi mặc định port vẫn tương thích.
2. Kiểm tra IROM1 `0x08000000/0x10000`, IRAM1 `0x20000000/0x5000`; Rebuild All, lưu Build Output và map. Flash Keil = Code+RO-data+RW-data; RAM = RW-data+ZI-data.
3. Giữ heap FreeRTOS 8192 byte và stack ứng dụng 768/1280/1536 byte trước khi có số đo để đề xuất thay đổi.
4. ST-Link: SWDIO PA13, SWCLK PA14, GND chung, nguồn/VTref 3.3V đúng cách, BOOT0=0; chọn SWD và flash algorithm STM32F10x medium-density. Download, Verify, Reset/Run. Nếu không kết nối, kiểm tra dây/nguồn và thử Connect under Reset.
5. Không generate CubeMX đè checkout. Xem [cấu trúc](REPO_STRUCTURE.md) nếu cần thay pin/clock.

## Bảng ghi kết quả

| Kiểm tra | Kỳ vọng / bằng chứng | Trạng thái |
|---|---|---|
| Clock/nguồn | HSE 8 MHz, SYSCLK 72 MHz, ADC 12 MHz; ADC inputs 0–3.3V | Chưa đo |
| Joystick ở tâm | Reset với cần thả: sau ~320 ms không có press giả; JOY_CENTER armed | Chưa đo |
| Bốn hướng | Trái/phải dịch, lên xoay một lần, xuống soft drop; hướng lắp đúng | Chưa đo |
| SW/Pause/Rotate | SW hard drop một lần khi giữ, PAUSE dừng/tiếp tục, PB14 tùy chọn | Chưa đo |
| Menu | Lên/xuống chọn, phải/SW OK, trái Back; không tự qua nhiều mục do hold | Chưa đo |
| ADC lỗi/startup gạt | Mock đã có; trên kit kiểm tra reset đang gạt rồi về tâm, không kết luận đứt dây được tự phát hiện | Chưa đo |
| Buzzer active-low | Idle PB6 HIGH, báo PB6 LOW rồi HIGH; hết âm và reset không bị kẹt kêu | Chưa đo |
| OLED | SSD1306 0x3C, PB8/PB9 remap; đủ 8 màn, chữ/board/preview đúng | Chưa đo |
| I2C lỗi | Thử mất OLED theo cách an toàn; game/UART tiếp tục, nối đúng trở lại có thể phục hồi | Chưa đo |
| Gravity | EASY/NORMAL/HARD 800/500/250 ms; ghi sai số đo | Chưa đo |
| Game | Va chạm/tường/khóa, 1–4 hàng, điểm/level, WIN ưu tiên spawn, Retry reset | Chưa đo |
| Pause/Resume | Không rơi bù do tính cả thời gian pause | Chưa đo |
| RAM/timing | Ghi heap_min, stack high-water và thời gian Game/OLED | Chưa đo |
| Chạy lâu | Chơi và Retry/Pause nhiều lần, không fault hoặc mất điều khiển bất thường | Chưa đo |

Nếu có tiếng kêu trong lúc reset trước khi GPIO được cấu hình, xem [pull-up và giới hạn phần mềm](BUZZER.md). Không đo tần số PWM trên PB6: bản hiện tại dùng module tự tạo âm, GPIO chỉ bật/tắt.

## Watch và UART

`app_diagnostics`: fault, heap_free/heap_min, input/game/display_stack_free_words, game_max_ms, display_max_ms, input_queue_full/input_dropped, oled_errors. Telemetry khoảng mỗi giây, độ phân giải đo 1 ms; không thay cho đo WCET hoặc analyzer.

High-water tính bằng word 4 byte, không phải byte. Ngưỡng thảo luận ban đầu: heap_min ≥512 byte và mỗi stack còn ≥64 word; đây là đề xuất để nhóm đánh giá, không phải kết quả đã đạt. Cần xét riêng MSP/ISR và stack runtime C.

Fault codes giữ tương thích: 1 đối tượng RTOS, 2 malloc, 3 stack, 4 là mã lịch sử `APP_FAULT_PWM` tại kiểm tra buzzer init. Driver GPIO hiện trả thành công sau khi đặt HIGH, nên mã 4 không xác nhận phần cứng module đang kêu đúng.

Ghi báo cáo có firmware SHA, compiler/version, log build/map, sơ đồ dây, ảnh/video và số đo. Chỉ đánh dấu PASS khi có bằng chứng. Build/test phần mềm không tự biến thành release đã nghiệm thu kit.

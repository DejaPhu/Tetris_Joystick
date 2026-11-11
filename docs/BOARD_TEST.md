> Bản joystick: xem [đấu dây, module và checklist phần cứng](JOYSTICK.md) và [kết quả kiểm thử mới](test-results/JOYSTICK.md). Nội dung bên dưới được giữ từ bản nền.

# Build Keil, nạp ST-Link và test chấp nhận

## Build/nạp

1. Dùng toàn bộ checkout integration; mở `Teris/MDK-ARM/Teris.uvprojx`. Xác nhận include `../../app/inc`; nguồn ứng dụng trong group Application/Tasks and Engine và driver trong HAL/Board peripherals và các nguồn nền không trùng.
2. Chọn Arm Compiler 5 tương thích port RVDS/ARM_CM3. Nếu máy chỉ có Compiler 6, cần cài compiler phù hợp hoặc migration riêng được review; không chỉ đổi dropdown và giả định port tương thích.
3. Kiểm tra IROM1 0x08000000/0x10000 và IRAM1 0x20000000/0x5000. Rebuild All, không lỗi; phân loại warning. Flash = Code+RO-data+RW-data; RAM = RW-data+ZI-data. Giữ nguyên heap 8192 và stack ứng dụng 768/1280/1536 byte trước khi có số đo.
4. ST-Link: chung GND, VTref/nguồn đúng mức 3.3 V, PA13 SWDIO, PA14 SWCLK; có thể nối NRST. BOOT0=0. Chọn ST-Link Debugger, SWD, tốc độ ban đầu 1 MHz; flash algorithm STM32F10x medium-density. Download → Verify → Reset/Run.
5. Khi không kết nối, thử Connect under Reset và kiểm tra dây/nguồn. Không đổi chân SWD hoặc cấp nguồn xung đột.

## Quan sát qua Watch

Thêm `app_diagnostics`: `fault=0`, heap_free/heap_min (byte), input/game/display_stack_free_words (mỗi word=4 byte), game_max_ms, display_max_ms, input_queue_full/input_dropped/oled_errors. Telemetry cập nhật khoảng mỗi giây, thời gian đo có độ phân giải 1 ms. Đây không phải đo WCET chính xác.

Fault: 1=RTOS object, 2=malloc, 3=stack overflow, 4=PWM start. Dừng tại app_fault để xem lỗi. Hook chỉ là phát hiện cuối cùng; phải theo dõi high-water trước khi overflow.

Đề xuất ngưỡng thử ban đầu: heap_min >=512 byte; mỗi stack còn >=64 words; queue_full/dropped=0 khi chơi bình thường. Đây là tiêu chí cần đo, không phải kết quả đã đạt. High-water không tính thay cho MSP/ISR; stack startup 1024 byte cũng cần kiểm tra khi debug.

## Bảng ghi kết quả thực tế

| Test | Cách thử / kỳ vọng | Kết quả |
|---|---|---|
| Nguồn/clock | Board ổn định 3.3 V, HSE đúng 8 MHz, SYSCLK 72 MHz | Chưa test |
| 6 nút | Mỗi press một event; debounce >=30 ms; chỉ trái/phải/xuống repeat | Chưa test |
| OLED | SSD1306 0x3C, đủ tám màn, số không lộn ngược, ô và preview rõ | Chưa test |
| I2C lỗi | Rút/nối OLED khi an toàn; lỗi có bộ đếm, game vẫn chạy, nối lại hiển thị phục hồi | Chưa test |
| Gravity | Đo timestamp/logic analyzer từng lần rơi: 800/500/250 ms; ghi sai số | Chưa test |
| Spawn/drop | Hard/soft drop khóa đúng, khối mới không rơi ngay; kiểm tường/đáy và bảy loại | Chưa test |
| Xóa/điểm | 1/2/3/4 hàng: 100/300/500/800 × level trước clear; mốc 10 hàng | Chưa test |
| Pause/menu | WELCOME → menu/help/difficulty → play → pause/resume/restart/menu | Chưa test |
| WIN/thua | >=1000 → YOU WIN!, spawn bị chặn → GAME OVER; Retry reset điểm/board | Chưa test |
| PWM | Oscilloscope PB6: 900/1500/350/2200 Hz theo hiệu ứng; duty khoảng 50%, tự tắt | Chưa test |
| RAM/timing | Ghi heap_min, stack high-water, max task/flush; đo scheduling bằng GPIO nếu cần | Chưa test |
| Chạy lâu | Chơi liên tục ít nhất 30 phút, thử nhiều retry/pause; không fault/mất phím | Chưa test |

Tạo báo cáo gồm commit SHA, phiên bản compiler, Build Output/map, ảnh từng màn, video chơi, đo PWM/I2C và bảng telemetry. Chỉ đánh dấu PASS khi có bằng chứng thực. Chưa release/main trước khi review và hoàn tất các mục trên.

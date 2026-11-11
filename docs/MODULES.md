> Bản joystick: xem [đấu dây, module và checklist phần cứng](JOYSTICK.md) và [kết quả kiểm thử mới](test-results/JOYSTICK.md). Nội dung bên dưới được giữ từ bản nền.

# Module và giải thích mã lệnh chính

Comment trong nguồn giải thích mục đích, quyền sở hữu dữ liệu, giới hạn thời gian và xử lý lỗi; không lặp lại từng câu lệnh C hiển nhiên.

| Tệp | Vai trò / hàm chính |
|---|---|
| app_cfg.h / app_ipc.h / task_*.h | Cấu hình RTOS/chu kỳ, extern Queue, entry points không tạo Task mới |
| app_shared.h | Contract: mã nút, FSM, snapshot, Queue handles do CubeMX tạo |
| button_filter.c | `button_filter_step`: đổi mẫu thì ghi thời điểm; chỉ chấp nhận khi ổn định >=30 ms; phép trừ unsigned chịu wrap |
| task_input.c | Quét active-low, retry press pending ưu tiên PAUSE, không block chờ Queue; repeat có thể bỏ khi đầy |
| game_engine.c | `can_place` kiểm biên trước khi đọc mảng; `lock_piece` khóa và compact hàng từ dưới lên; `spawn_next` reset mốc rơi; `game_tick` tiến deadline |
| game_fsm.c | `handle_button` diễn giải nút theo một trong tám trạng thái; bỏ repeat trong menu |
| game_internal.h | Trạng thái engine riêng cho GameTask; Display không được đọc trực tiếp |
| task_game.c | Giới hạn 16 sự kiện/lượt, bỏ sự kiện tồn khi đổi màn; publish bản sao và token; bảo vệ sao chép bằng critical section |
| hal_gpio.c / board_pins.h | Đọc sáu nút active-low; BSP dẫn tới pin mapping chính thức |
| hal_buzzer.c | PWM PSC=71 → 1 MHz; ARR=1 MHz/f−1, CCR≈50%; deadline kết thúc âm, không HAL_Delay |
| hal_oled.c | SSD1306 page mode, địa chỉ HAL 0x3C<<1; packet 129 byte; bus recovery bounded, không reset bus từ Task khác |
| ui_renderer.c | Font 5×7, framebuffer 1024 byte, bàn 3×3 pixel/ô, tám màn hình và next preview |
| task_display.c | Lấy token và snapshot, render/flush, giới hạn tốc độ 160 ms; backoff 500 ms khi lỗi |
| app_diagnostics.c | Heap/stack high-water và thời gian tối đa, xem trong Keil Watch; fault dừng có mã nguyên nhân |
| main.c | Chỉ cấu hình HAL/CubeMX, kiểm handles và gọi ba wrapper; không đặt engine vào main |

Điểm dùng level **trước** khi xóa hàng. Level hiển thị bắt đầu 1, không làm thay đổi độ khó đã chọn. Pause/resume và sinh khối mới bắt đầu một khoảng rơi đầy đủ. Xoay O giữ nguyên vị trí; wall kick chỉ dịch ngang, không có floor kick/SRS đầy đủ.

Không thêm Task UART riêng. USART1 hiện xuất log bằng ring buffer 512 byte và ngắt TXE; formatter nhỏ app_log không dùng printf. Xem docs/UART_DEBUG.md. Telemetry qua Watch vẫn được giữ.

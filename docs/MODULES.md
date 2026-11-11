# Bản đồ module và comment học code

Comment mới dùng tiếng Việt không dấu trong mã C để dễ hiển thị trên Keil 5. Tập trung giải thích quyền sở hữu, lý do thuật toán, timing và nhánh lỗi; không thêm comment học vào thư viện ST/CMSIS/FreeRTOS.

| Nhóm | Module | Người học chính | Nội dung cần đọc trong comment |
|---|---|---|---|
| Startup/tích hợp | `main.c`, `bsp/inc`, `bsp/src`, `bsp/startup` | Cả nhóm; TV1 quản lý cấu hình | Init → kernel → Queue → Task; GPIO buzzer HIGH sớm |
| Input | `task_input.c`, `joystick_filter.c`, `button_filter.c` | TV1 | Mã logic, calibration, hysteresis, debounce, pending/repeat |
| Driver input | `hal_gpio.c`, `hal_joystick.c` | TV1 | Active-low, ADC timeout, cặp mẫu hoàn chỉnh |
| Game | `task_game.c`, `game_engine.c`, `game_fsm.c` | TV2 | Engine owner, mask/collision/row clear, FSM/deadline, snapshot |
| Buzzer | `hal_buzzer.c` | TV2 | Module kích LOW, HIGH tắt, poll deadline |
| Display | `task_display.c`, `ui_renderer.c`, `hal_oled.c` | TV3 | Snapshot copy, token, pixel/page, I2C và retry |
| Chẩn đoán chung | `app_log.c`, `hal_uart.c`, `app_diagnostics.c` | Cả nhóm; TV1 tìm hiểu UART, TV2 timing/heap/stack | Ring buffer bảo vệ PRIMASK, ISR không gọi RTOS, mute fatal |
| Contract chung | `app_cfg.h`, `app_shared.h`, `app_ipc.h`, `task_*.h` | Cả nhóm | Byte/word, Queue item, snapshot API và wrapper |
| Thư viện nền | `middlewares/` | Cả nhóm hiểu vai trò, chưa cần học mọi hàm vendor | FreeRTOS scheduling, CMSIS wrapper, ST HAL peripheral API |

Xem [phân công](TEAM_ASSIGNMENT.md) và [hướng dẫn học chung](learning/00_KIEN_TRUC_CHUNG.md) trước khi đọc sâu Task của mình. Code và tài liệu cùng mô tả firmware hiện tại; tài liệu lịch sử được đặt ở `archive/` hoặc đánh dấu theo bản test cũ.

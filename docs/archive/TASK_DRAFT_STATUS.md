> Tài liệu lưu trữ từ bản nhập mã trước đây; đường dẫn và trạng thái bên dưới không dùng làm hướng dẫn hiện tại. Xem [cấu trúc mới](../REPO_STRUCTURE.md) và [phân công học](../TEAM_ASSIGNMENT.md).

# Lịch sử bản nháp

Tài liệu bên dưới mô tả các PR nhập nháp ban đầu, không phải trạng thái integration hiện tại. Các lỗi đã sửa và việc ghép module được ghi trong README.md, docs/MODULES.md và docs/test-results/SYSTEM_V1.md. Chưa có kiểm thử board.

# Ba Task: bản nháp để review, chưa tích hợp

Nguồn: task_drafts trong GOI_BAN_GIAO_CODEX_TETRIS.zip do người dùng cung cấp. Được người dùng cho phép đưa lên GitHub. Giữ nguyên nội dung bốn tệp C/header, chưa sửa thuật toán. Các PR riêng: API chung, InputTask, GameTask, DisplayTask.

## Phụ thuộc và vị trí

- Header dùng chung: app/inc/app_shared.h. PR header cần review trước ba PR Task.
- app/src/task_input.c -> feature/task-input.
- app/src/task_game.c -> feature/task-game.
- app/src/task_display.c -> feature/task-display.
- Project gốc: Teris/MDK-ARM/Teris.uvprojx trên feature/config-cubemx-oled.

Sau khi các PR được review và Leader merge, tích hợp trên integration/system-v1: thêm ../../app/inc vào include paths (tính từ Teris/MDK-ARM), thêm ../../app/src/task_*.c vào build. Trong USER CODE Includes của main.c dùng #include "app_shared.h". Thay vòng lặp trống của ba Start*Task bằng task_input_run(argument), task_game_run(argument), task_display_run(argument) tương ứng. Không định nghĩa lại wrappers, không tạo thêm Task/Queue, không gọi lại osKernelStart.

Hai Queue giữ nguyên: 16 x uint16_t mã nút và 1 x uint16_t token frame. Snapshot được bảo vệ bằng critical section. Ba Task ứng dụng; Idle/Timer service là tác vụ nội bộ kernel.

## Các mục phải sửa/kiểm chứng trước tích hợp

- Input: ba mẫu 10 ms có thể nhận trước đủ 30 ms; phím giữ khi khởi động; sự kiện bị bỏ khi Queue đầy.
- Game: xoay khối O bị dịch vị trí; HARD 250 ms bị lượng tử thành khoảng 260 ms với chu kỳ 20 ms và reset mốc bằng now; mốc rơi không reset khi khóa/sinh khối sau soft/hard drop. Chốt hệ số điểm tại mốc 10 hàng.
- Display: kiểm tra font, SSD1306 thật ở 0x3C, bố cục ô 3x3, lỗi I2C và bus recovery; đo thời gian flush ở 100 kHz.
- Tách engine/FSM/buzzer và OLED/renderer thành module khi hoàn thiện.
- Kiểm tra mọi Queue/Task handle, bổ sung chẩn đoán malloc failed/stack overflow.

## Mức kiểm chứng

Người dùng đã báo build bộ khung Teris gốc không lỗi trên Keil: Code=12288 RO-data=528 RW-data=164 ZI-data=13412. Không áp dụng kết quả đó cho ba Task này. Chưa có build Keil tích hợp, linker map tích hợp, log nạp ST-Link hoặc kiểm thử board của bản nháp. Cần đo heap tối thiểu còn lại, stack high-water, timing và chạy đủ FSM/gameplay trước release. Không tự merge develop/main/release.

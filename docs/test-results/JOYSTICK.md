> Báo cáo của mốc mã trước đây; đường dẫn mới và kết quả sau tổ chức lại xem [STRUCTURE_LEARNING.md](STRUCTURE_LEARNING.md).

# Kết quả kiểm thử Tetris Joystick — 2026-11-11

Nền: `36ea9cd444f3cb390b02e967306454244b80ea75`. Kết quả áp dụng cho bản tích hợp joystick; các báo cáo SYSTEM_V1/UART_DEBUG là lịch sử bản nền.

| Kiểm tra | Kết quả | Bằng chứng |
|---|---|---|
| Keil source/include/memory, số Tasks/Queues, ADC CubeMX/HAL | PASS | [JOYSTICK_PROJECT.txt](JOYSTICK_PROJECT.txt) |
| Game/FSM và render đủ 8 trạng thái | PASS | [JOYSTICK_HOST.txt](JOYSTICK_HOST.txt) |
| UART buffer/IRQ/formatter | PASS | [JOYSTICK_HOST.txt](JOYSTICK_HOST.txt) |
| Joystick calibration, noise, hysteresis, 4 hướng, diagonal lock, startup đang gạt, rearm sau ADC lỗi | PASS | [JOYSTICK_HOST.txt](JOYSTICK_HOST.txt) |
| ADC start/poll/stop lỗi ở từng trục; timeout hữu hạn; không trả nửa cặp | PASS với mock HAL | [JOYSTICK_HOST.txt](JOYSTICK_HOST.txt) |
| InputTask → Queue, debounce, rotate/SW chỉ một press khi giữ, nút cũ tắt, PAUSE ưu tiên, retry press khi đầy, bỏ repeat | PASS với mock ADC/RTOS | [JOYSTICK_HOST.txt](JOYSTICK_HOST.txt) |
| ARM GCC compile/link, tạo ELF và HEX | PASS, không warning | [JOYSTICK_ARM_BUILD.txt](JOYSTICK_ARM_BUILD.txt) |
| Keil Compiler 5 rebuild, nạp board, chiều joystick, ADC noise thực tế, timing/stack watermark | Chưa thực hiện | [Checklist board](../JOYSTICK.md#thử-trên-board) |

Host tests dùng C99, `-Wall -Wextra -Werror`, AddressSanitizer và UndefinedBehaviorSanitizer. Đã bổ sung kiểm tra LEFT tại PAUSED/WIN/GAME_OVER.

ARM GCC 12.2.1 với newlib; startup/linker/FreeRTOS GCC port bổ sung. Kết quả GNU size: text 23948, data 144, bss 15608 bytes. Flash text+data = **24092 / 65536 bytes**. Tổng RAM các section = **15752 / 20480 bytes**, gồm `.data` 144, `.bss` 14072 và `.heap_stack` 1536 (512 byte heap + 1024 byte MSP dự phòng). FreeRTOS heap 8192 byte đã nằm trong `.bss`; không cộng thêm lần nữa. Còn 4728 byte ngoài các vùng đã đặt; đây không phải số heap còn trống khi chạy.

Static stack-usage do GCC ghi: riêng hàm `task_input_run` 88 byte, `joystick_filter_step` 24 byte, `hal_joystick_read` 24 byte. Không dùng các số riêng hàm này làm high-water mark; cần đo lại chuỗi gọi thực tế trên kit. InputTask vẫn được cấp 768 byte.

Lệnh tái lập từ root repo:

```sh
python3 tests/check_project.py
./tests/run_host.sh
python3 tools/build_gcc.py
```

Nếu giải nén Debian toolchain vào thư mục riêng thay vì cài hệ thống, script build hỗ trợ `ARM_GCC_PREFIX=/path/to/usr/bin/arm-none-eabi-` và `ARM_NEWLIB_ROOT=/path/to/usr/lib/arm-none-eabi`.

> Báo cáo của mốc mã trước đây; đường dẫn mới và kết quả sau tổ chức lại xem [STRUCTURE_LEARNING.md](STRUCTURE_LEARNING.md).

# System-v1: kết quả kiểm chứng phần mềm, 10/11/2026

## Đã thực hiện

- `tests/check_project.py`: PASS. Mọi source/include Keil tồn tại, không trùng source; memory 64 KiB Flash/20 KiB SRAM; đúng ba osThreadNew, hai osMessageQueueNew, một osKernelStart; wrappers đã nối module; không HAL_Delay trong app.
- `tests/run_host.sh`: PASS với GCC host, AddressSanitizer và UndefinedBehaviorSanitizer. Kiểm tra debounce 30 ms, bounce, repeat, wrap uint32_t; FSM và help/pause/retry; EASY/NORMAL/HARD, deadline và reset mốc spawn; xoay O, bốn lần xoay I, biên trái; clear 1–4 hàng, điểm trước mốc level, WIN/GameOver; render tám state. Thêm 100.000 bước input giả ngẫu nhiên với kiểm tra bất biến board và state, không phát hiện lỗi sanitizer.
- ARM cross-build và link: PASS, GNU Arm Embedded GCC 12.2.1 (Debian 12.2.rel1-1), Cortex-M3/Thumb, -Os, newlib-nano; không warning/error trong log. Dùng HAL/CMSIS/FreeRTOS và ứng dụng thật; chỉ startup/linker/port được thay bằng bản dành cho GCC trong build bổ sung.
- Kiểm ELF: vector table 59 entries/236 byte ở Flash; SP đầu 0x20005000; Reset, SVC, PendSV, SysTick và TIM2 trỏ đúng handler liên kết. Linker kiểm giới hạn RAM và Flash.

## Bộ nhớ của bản ARM GCC bổ sung

| Mục | Byte |
|---|---:|
| text | 19.644 |
| data | 136 |
| bss (gồm vùng reserve heap/stack startup) | 14.984 |
| Flash = text + data | 19.780 / 65.536 |
| RAM = data + bss | 15.120 / 20.480 |
| RAM còn lại theo linker | 5.360 |

Heap FreeRTOS 8192 byte nằm trong bss; stack ba Task ứng dụng cấp phát từ heap này, không cộng hai lần. Idle/Timer service dùng stack tĩnh. Dung lượng dự trữ không chứng minh stack/heap runtime đạt tiêu chí.

Một số frame stack tĩnh GCC: task_game_run 40 byte, publish_snapshot 240 byte, task_display_run 248 byte, oled_flush 160 byte. Đây là từng frame, không phải tổng call chain hoặc high-water runtime, và không áp dụng trực tiếp cho compiler Keil.

## Chưa thực hiện

- Chưa build bản tích hợp bằng Arm Compiler/Keil µVision. Bộ khung trước đây được người dùng báo build không lỗi (Code=12288 RO=528 RW=164 ZI=13412); không dùng số đó làm kết quả của bản tích hợp.
- Chưa nạp board/ST-Link; chưa đo nút, PWM, I2C, OLED/font, gravity jitter, heap/stack high-water, WCET và chạy lâu.
- Host tests không mô phỏng HAL, scheduler, I2C/PWM hoặc bus recovery. I2C recovery và fault path cần kiểm thử board.

Kết luận: nguồn tích hợp đã hoàn chỉnh để nhóm mở Keil, review và thử board. Chưa phải release đã nghiệm thu. Ghi bằng chứng vào bảng docs/BOARD_TEST.md trước khi chốt release/main.

## Đối chiếu cấu trúc PDF

Đã di chuyển source vào bsp/hal/middlewares/app và main.c ở gốc; bổ sung app_cfg.h, app_ipc.h, task_*.h. Chạy lại project checker, host tests ASan/UBSan và ARM GCC build sau khi di chuyển: PASS, không warning/error. Cấu trúc Keil và đường dẫn include/source đã kiểm tra; chưa chạy µVision/board.

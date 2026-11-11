# Kiểm chứng tổ chức lại cấu trúc và tài liệu học — 2026-11-11

Mốc trước thay đổi: `9e271d675e30d49db392e812602382d5aeb67e03` (đã sửa buzzer kích LOW). Bản này chuyển vị trí file, cập nhật đường dẫn Keil/GCC/CubeMX, thêm comment và tài liệu học; giữ hành vi firmware.

| Kiểm tra | Kết quả |
|---|---|
| `python3 tests/check_project.py` | PASS: source/include/startup tồn tại, không trùng; 64 KB Flash/20 KB RAM, đúng 3 application Tasks/2 Queues/1 kernel start |
| Cấu hình ADC/UART/buzzer | PASS: ADC PA0/PA1 12 MHz, calibration; USART1 ISR; PB6 GPIO HIGH khi tắt |
| Cây thư mục | PASS: BSP/HAL/middlewares/app, startup và vendor ở vị trí mới, không còn bản `Teris/` hoặc `hal/vendor/` |
| `./tests/run_host.sh` | PASS cả 6 executable: game/FSM/UI, UART, joystick filter, ADC mock, InputTask mock và buzzer mock, có ASan/UBSan |
| ARM GCC 12.2.1 build/link | PASS, không warning; text 22876, data 144, bss 15536 byte, trong giới hạn STM32F103C8 |
| So sánh binary Flash trước/sau | `objcopy -O binary` rồi `cmp`: giống từng byte |
| Vendor | 132 file ST HAL sau chuyển thư mục có Git blob hash giống trước; giữ nguyên license/nội dung |
| Tài liệu | Các link file nội bộ trong README, docs hiện hành và tài liệu học đều trỏ tới file tồn tại |
| Gitignore | Keil output `MDK-ARM/Tetris_Joystick/` được loại bỏ khỏi Git |
| Keil Compiler 5 / board thật | Chưa rebuild/nạp/đo ở môi trường này; cần nhóm thực hiện checklist board |

SHA-256 ảnh binary Flash trước và sau: `93d2cdbd8452fe71d2cb1ebe87b8b03bcecb63ff4d092f855b2a501eb9586943`. So sánh này không dùng ELF debug (đường dẫn debug và số dòng có thể thay đổi) và không thay thế nghiệm thu phần cứng.

Lệnh kiểm tra:

```sh
python3 tests/check_project.py
./tests/run_host.sh
python3 tools/build_gcc.py
```

Trong môi trường thực hiện, GCC/newlib được giải nén vào thư mục riêng và dùng biến `ARM_GCC_PREFIX`, `ARM_NEWLIB_ROOT` mà script hỗ trợ. Keil vẫn dùng port RVDS/ARM_CM3 và startup ARMASM trong BSP; GCC dùng port/startup/linker bổ sung ở tools.

Tài liệu học gồm phần kiến trúc chung, ba hướng dẫn riêng, bài thực hành và checklist hỏi chéo. Đây là tài liệu để nhóm học và tự chứng minh kiến thức, không phải xác nhận các thành viên đã hoàn thành bài học hoặc đã kiểm thử kit.

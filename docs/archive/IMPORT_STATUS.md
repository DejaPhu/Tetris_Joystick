> Tài liệu lưu trữ từ bản nhập mã trước đây; đường dẫn và trạng thái bên dưới không dùng làm hướng dẫn hiện tại. Xem [cấu trúc mới](../REPO_STRUCTURE.md) và [phân công học](../TEAM_ASSIGNMENT.md).

# Cập nhật integration

Project Keil hiện đã được cấu hình memory/include và thêm nguồn ứng dụng. Ba wrappers đã nối module; xem README.md và docs/BOARD_TEST.md. Ghi chú bên dưới là lịch sử nhập nguồn gốc. Chưa chạy Keil/board cho bản tích hợp.

# Trạng thái nhập nguồn Teris gốc

Nguồn: Teris.zip do người dùng đính kèm. Giữ Core, Drivers (bao gồm CMSIS/Include), Middlewares, startup và project Keil gốc. Không dùng gói Keil_Teris_Git_Import.zip đã lọc. Giữ file Teris.ioc hiện có vì cấu hình tương đương bản gốc; không nhập Teris.uvoptx là cấu hình người dùng. Thư viện vendor và license được giữ lại.

## Mức kiểm chứng

Người dùng báo bộ khung build trên Keil không lỗi, với Code=12288, RO-data=528, RW-data=164, ZI-data=13412. Flash tính toán: 12980 byte; RAM: 13576 byte. Đây là kết quả do người dùng cung cấp sau khi kiểm tra cấu hình Target trên máy của họ, không phải log build được chạy lại cho commit này.

Project .uvprojx nhập từ ZIP vẫn giữ nguyên. Các trường OnChipMemories trong file gốc bằng 0; trước khi build cần kiểm tra Target: IROM1 start 0x08000000 size 0x10000, IRAM1 start 0x20000000 size 0x5000, IROM1 Startup. Kiểm tra compiler tương thích FreeRTOS port RVDS/ARM_CM3; không tự đổi toolchain.

Ba Start*Task trong main.c vẫn là vòng lặp trống. Chưa tích hợp các Task nháp; chưa xác nhận OLED, nút, PWM, gameplay hoặc heap/stack trên board. Không tạo thêm Task/Queue.

Sau review, PR nền này có thể được Leader merge vào develop. Không tự merge main/release.

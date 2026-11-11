> Tài liệu lưu trữ từ bản nhập mã trước đây; đường dẫn và trạng thái bên dưới không dùng làm hướng dẫn hiện tại. Xem [cấu trúc mới](../REPO_STRUCTURE.md) và [phân công học](../TEAM_ASSIGNMENT.md).

# Mở project sau khi tổ chức lại nguồn

Nguồn đã nhập đầy đủ và bố trí theo docs/REPO_STRUCTURE.md. Mở MDK-ARM/Teris.uvprojx từ checkout đầy đủ, không copy lại Core/Drivers/Middlewares từ ZIP vào Teris vì sẽ tạo bản trùng. main.c nằm ở gốc repository; BSP ở bsp/, ST HAL ở hal/vendor/, CMSIS và FreeRTOS ở middlewares/. Xem README.md gốc và docs/BOARD_TEST.md.

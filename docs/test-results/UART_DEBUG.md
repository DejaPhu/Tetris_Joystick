> Báo cáo của mốc mã trước đây; đường dẫn mới và kết quả sau tổ chức lại xem [STRUCTURE_LEARNING.md](STRUCTURE_LEARNING.md).

# UART debug: kiểm chứng phần mềm, 11/11/2026

- Project checker PASS: mọi nguồn/include Keil, đúng 3 Tasks, 2 Queues, 1 kernel start; USART1_IRQHandler chỉ được định nghĩa một lần và gọi driver debug.
- Host test game hiện có PASS (ASan/UBSan), bao gồm 100.000 bước input kiểm bất biến.
- tests/test_uart.c PASS (ASan/UBSan): vòng TXE, sức chứa 511 byte, bỏ nguyên dòng khi đầy, quay vòng 1.000 lượt, phục hồi PRIMASK đang bằng 1, formatter timestamp/UINT32_MAX/tên event giới hạn, fatal flush hữu hạn khi TXE bị kẹt.
- ARM GCC 12.2.1 build/link Cortex-M3 PASS, không warning/error. Vector USART1 ở index 53 trong vector table 59 entry trỏ tới ISR mới, không phải Default_Handler.
- Size: text=20924, data=144, bss=15512. Flash=text+data=21068/65536 byte; RAM=data+bss=15656/20480 byte, còn 4824 byte theo linker. Heap RTOS và vùng reserve startup đã nằm trong số RAM, không cộng lần nữa. Đây là build GCC bổ sung, không phải số của Keil.
- Một số frame stack GCC: app_log 128 byte, number 40 byte. Cần đo high-water thực tế của cả call chain trên board; không tăng heap/stack khi chưa có dữ liệu.

Chưa chạy µVision/Arm Compiler 5 cho thay đổi này. Chưa có kết quả serial terminal/logic analyzer, đo latency critical section/IRQ, high-water runtime hoặc chơi lâu trên kit. Không đánh dấu các mục phần cứng là PASS. Hướng dẫn nối dây, giải mã log và test PB10: docs/UART_DEBUG.md.

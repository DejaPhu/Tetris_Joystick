> Bản joystick: xem [đấu dây, module và checklist phần cứng](JOYSTICK.md) và [kết quả kiểm thử mới](test-results/JOYSTICK.md). Nội dung bên dưới được giữ từ bản nền.

# Checklist kiểm thử (để thành viên tự thực hiện sau)

- [ ] STM32CubeIDE: F103C8Tx, clock 72MHz, SWD, TIM2 timebase.
- [ ] OLED SSD1306 nhận I2C1 remap PB8/PB9 và hiện đúng.
- [ ] Buzzer TIM4_CH1 PB6 tạo được âm không chặn scheduler.
- [ ] 6 nút active-low đọc đúng, đã debounce.
- [ ] USART1 log được.
- [ ] Đúng ba Task ứng dụng; không tạo trùng ở CubeMX.
- [ ] xInputQueue và xFrameQueue hoạt động.
- [ ] FSM menu, difficulty, pause, retry, YOU WIN và GAME_OVER đúng.
- [ ] 3 độ khó chạy khác nhau; điều kiện điểm thắng đúng.
- [ ] Kiểm tra heap, stack high-water, lỗi overflows và test trên board.

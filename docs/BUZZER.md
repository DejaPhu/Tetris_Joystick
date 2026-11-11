# Module buzzer active kích LOW (`低电平触发`)

PB6 → chân IO của module, GND chung, VCC theo thông số module; tín hiệu IO phải tương thích logic 3.3V. LOW = kêu, HIGH = tắt. Bản PWM cũ đặt duty 0 để tắt, tức giữ PB6 LOW, khiến module này kêu liên tục.

Bản sửa dùng PB6 GPIO push-pull, không khởi tạo TIM4 hoặc dùng PWM trên chân này. Module tự tạo cao độ, nên `tone(hz, ms)` chỉ dùng `hz != 0` để bật trong thời lượng `ms`. `hz == 0` hoặc `ms == 0` tắt ngay. Không còn đổi được cao độ như buzzer thụ động. GameTask gọi `tone_poll()` mỗi vòng: âm được tắt ở lần poll sau deadline (có thể trễ khoảng một chu kỳ GameTask, tối đa ngủ 20 ms, cộng thời gian xử lý).

PB6 được preload HIGH trước khi đổi sang output, ngay sau HAL_Init, trước cấu hình clock và các ngoại vi khác. `buzzer_init`, hết hạn, hủy âm, `Error_Handler` và `app_fault` đều đặt HIGH. `app_fault` tắt âm trước khi ghi log và dừng. Không có Task/Queue mới hay HAL_Delay.

Trong lúc reset, bootloader hoặc trước khi firmware cấu hình GPIO, chân MCU có thể ở trạng thái input. Nếu module vẫn kêu ngắn lúc reset, thêm điện trở pull-up khoảng 4.7–10 kΩ từ IO/PB6 lên **3.3V**, bảo đảm module nhận HIGH ở 3.3V. Phần mềm không thể bảo đảm mức chân trong thời gian MCU đang reset; cũng không tự tắt được nếu CPU bị treo trước lần poll tiếp theo.

Kiểm thử ngày 2026-11-11:

- Host tests với ASan/UBSan: HIGH lúc init/tắt, LOW lúc bật, hết hạn, hủy, duration 0, retrigger và tick wrap đều PASS. Các test game, UART và joystick cũng PASS.
- `python3 tests/check_project.py`: PASS, gồm tham chiếu Keil/Tasks/Queues/ADC và PB6 GPIO HIGH.
- ARM GCC compile/link: PASS, không warning; text 22876, data 144, bss 15536 byte (GNU size). Build trong giới hạn Flash/RAM STM32F103C8.
- Chưa rebuild bằng Keil hoặc đo trên module thật.

Sau nạp: reset và để yên phải im; thao tác game có âm ngắn rồi im; nhấn nhiều lần không kẹt âm. Có thể đo PB6: idle khoảng 3.3V, khi kêu gần 0V. Nếu vẫn kêu khi PB6 HIGH, kiểm tra lại đúng IO/GND và mức điện áp module.

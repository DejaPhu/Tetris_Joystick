# Pin mapping — Tetris Joystick / STM32F103C8T6

| Kết nối | Chân MCU | Cấu hình |
|---|---|---|
| Joystick VRx | PA0 | ADC1_IN0, analog, sampling 55.5 cycles |
| Joystick VRy | PA1 | ADC2_IN1, analog, sampling 55.5 cycles |
| Joystick SW | PB10 | GPIO Input Pull-up, active-low |
| Joystick VCC / GND | 3.3V / GND | VRx/VRy phải trong 0–3.3V |
| PAUSE | PA8 | GPIO Input Pull-up, nút xuống GND |
| ROTATE tùy chọn | PB14 | GPIO Input Pull-up, nút xuống GND |
| OLED SCL / SDA | PB8 / PB9 | I2C1 remap, pull-up lên 3.3V |
| Buzzer active kích LOW | PB6 → IO | GPIO push-pull; HIGH tắt, LOW kêu |
| UART TX / RX | PA9 / PA10 | USART1 115200, 8N1 |
| ST-Link SWDIO / SWCLK | PA13 / PA14 | SWD |
| HAL Timebase | TIM2 | SysTick dành cho FreeRTOS |
| Nút cũ LEFT / RIGHT / DOWN | PB12 / PB13 / PB15 | Vẫn pull-up; không đọc mặc định |

Không cấp joystick bằng 5V vì các ngõ ADC chỉ nhận tới VDDA (3.3V). Có thể bật lại ba nút cũ bằng `JOYSTICK_LEGACY_BUTTONS=1`. Bảng thao tác và build trong [README](../README.md); chỉnh chiều và kiểm tra phần cứng trong [JOYSTICK.md](JOYSTICK.md).

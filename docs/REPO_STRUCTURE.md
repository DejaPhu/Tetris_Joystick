# Cấu trúc theo QuyTrinh-Git-2.pdf

Áp dụng sơ đồ thư mục ở mục 2, bước 1 của PDF cho repo **Tetris_Joystick**. `mcu-project` là tên minh họa trong tài liệu, không phải yêu cầu đổi tên repository. Các file build/IDE phụ được đặt riêng để giữ rõ bốn lớp chính.

```text
Tetris_Joystick/
├── bsp/                              # Cấu hình chip/board và pin mapping
│   ├── inc/                          # main.h, board_pins.h, HAL/FreeRTOS config
│   ├── src/                          # IRQ, MSP, system, HAL tick TIM2
│   └── startup/startup_stm32f103xb.s   # Startup Keil cho STM32F103C8
├── hal/                              # Driver do nhóm viết
│   ├── inc/                          # API GPIO, ADC joystick, OLED, buzzer, UART
│   └── src/                          # Triển khai API ngoại vi
├── middlewares/                      # Thư viện nền giữ nguyên nguồn/license
│   ├── CMSIS/
│   ├── FreeRTOS/Source/               # Kernel, CMSIS-RTOS2, port và heap
│   └── STM32F1xx_HAL_Driver/          # ST HAL vendor, tách khỏi driver của nhóm
├── app/                              # Đúng ba application Tasks và IPC contract
│   ├── inc/                          # task_*.h, app_cfg.h, app_ipc.h, snapshot
│   └── src/                          # Tasks, game/FSM, filter, UI, diagnostics
├── main.c                            # Init phần cứng, tạo Queue/Task, start kernel
├── Tetris_Joystick.ioc                # Cấu hình CubeMX, một người quản lý
├── MDK-ARM/Tetris_Joystick.uvprojx     # Project Keil µVision 5 / Compiler 5
├── docs/                             # Kiến trúc, phân công, hướng dẫn học
│   ├── learning/                     # Phần chung + chuyên sâu cho ba thành viên
│   ├── test-results/                 # Bằng chứng kiểm tra phần mềm
│   └── archive/                      # Tài liệu nhập nguồn cũ, chỉ để tra lịch sử
├── tests/                            # Host tests và mock; không link vào firmware
├── tools/                            # ARM GCC build bổ sung, linker và port GCC
├── .gitignore
└── README.md
```

| Vị trí cũ | Vị trí hiện tại |
|---|---|
| `Teris/MDK-ARM/Teris.uvprojx` | `MDK-ARM/Tetris_Joystick.uvprojx` |
| `Teris/MDK-ARM/startup_stm32f103xb.s` | `bsp/startup/startup_stm32f103xb.s` |
| `Teris/Teris.ioc` | `Tetris_Joystick.ioc` |
| `hal/vendor/STM32F1xx_HAL_Driver/` | `middlewares/STM32F1xx_HAL_Driver/` |
| Tài liệu import và Task nháp cũ | `docs/archive/` |

Không giữ bản sao mã cũ ở `Teris/` hoặc `hal/vendor/`. Các thư mục phụ `MDK-ARM`, `tests`, `tools`, `docs` phục vụ build/học và không tạo thêm lớp runtime.

## Cách áp dụng ví dụ trong PDF

PDF minh họa `task_sensor_create`, `app_ipc_init`, `xTaskCreate` và `vTaskStartScheduler`. Firmware này dùng API CMSIS-RTOS2 trên FreeRTOS: `main.c` tạo đúng 2 Queue bằng `osMessageQueueNew`, 3 Task bằng `osThreadNew`, rồi `osKernelStart`. Các wrapper `Start*Task` gọi `task_*_run`; không cần thêm hàm create hoặc `app_ipc.c` chỉ để chép nguyên ví dụ. `app/inc/app_ipc.h` là nơi khai báo handles dùng chung, handles được định nghĩa/tạo một lần trong `main.c`.

## Duy trì cấu trúc khi dùng CubeMX

Chỉ TV1/Leader quản lý `.ioc`, pin, clock và FreeRTOS config. CubeMX có thể sinh cây `Core/Drivers/Middlewares` mặc định; hãy generate ở thư mục tạm và đối chiếu cấu hình vào cây hiện tại, không generate đè checkout. Giữ nguyên wrapper Task, USART1 ISR tùy chỉnh, PB6 buzzer active-low và đường dẫn Keil. `tests/check_project.py` kiểm tra nguồn, include, startup, bộ nhớ, Queue/Task và cấu hình pin cần thiết.

Mở project từ đường dẫn **mới**; Keil output nằm trong `MDK-ARM/Tetris_Joystick/` và được `.gitignore` loại bỏ. Thư viện ST/CMSIS/FreeRTOS giữ nguyên nội dung và license.

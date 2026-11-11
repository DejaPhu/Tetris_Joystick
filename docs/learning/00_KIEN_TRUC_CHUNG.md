# 00 — Cả nhóm phải nắm kiến trúc chung

Đọc theo thứ tự: [README](../../README.md) → [sơ đồ kiến trúc](../ARCHITECTURE.md) → [app_cfg.h](../../app/inc/app_cfg.h) → [app_shared.h](../../app/inc/app_shared.h) → [main.c](../../main.c). Chưa cần hiểu từng thuật toán Tetris trước khi hiểu dữ liệu đi qua ba Task thế nào.

## 1. Giải thích dự án trong một phút

STM32F103C8 đọc joystick hai trục và các nút. InputTask biến điện áp thành sự kiện điều khiển. GameTask nhận sự kiện, cập nhật trạng thái menu/game và khối gạch, đồng thời điều khiển tiếng báo. GameTask công bố một ảnh chụp dữ liệu; DisplayTask sao chép ảnh đó, vẽ pixel rồi gửi sang OLED. UART chỉ phục vụ chẩn đoán.

BSP chứa pin/clock/IRQ, HAL chứa driver nhóm viết, middlewares chứa ST HAL/CMSIS/FreeRTOS, app chứa Task và logic. `main.c` nối các phần lúc khởi động. Đường dẫn file không quyết định Task runtime: một Task có thể gọi nhiều module hỗ trợ mà vẫn là một Task.

## 2. Trình tự khởi động

1. Startup Keil ở `bsp/startup/` thiết lập môi trường CPU rồi tới main qua runtime C.
2. `HAL_Init`, cấu hình GPIO sớm: PB6 được preload HIGH trước khi thành output để tắt buzzer kích LOW.
3. Cấu hình clock 72 MHz, I2C1, USART1, ADC1/ADC2; hardware calibration ADC. HAL có timebase TIM2.
4. `osKernelInitialize`, tạo hai Queue và ba Task, kiểm tra kết quả cấp phát.
5. `osKernelStart`: scheduler bắt đầu chạy các wrapper; mỗi wrapper gọi đúng hàm `task_*_run`.

Không gọi lại create Task/Queue trong `task_*_run`. Ví dụ `task_sensor_create` trong PDF là minh họa cho dự án khác; firmware này đã có CMSIS-RTOS2 wrappers.

## 3. Tham số RTOS hiện tại

| Application Task | Priority CMSIS | Stack cấp phát | Nhịp xử lý |
|---|---|---|---|
| InputTask | Normal3 (27) | 768 byte = 192 word | `vTaskDelayUntil`, 10 ms |
| GameTask | Normal2 (26) | 1280 byte = 320 word | Ngủ tối đa 20 ms, thức theo deadline rơi |
| DisplayTask | Normal1 (25) | 1536 byte = 384 word | 160 ms; khi lỗi OLED nghỉ 500 ms trước thử lại |

Số lớn hơn là ưu tiên cao hơn. Task ưu tiên cao phải nhường CPU bằng chờ/ngủ, không chạy vòng lặp bận vô hạn. Các con số là cấu hình, chưa phải đo timing thực tế. Display dùng HAL I2C đồng bộ có timeout, nhưng không khóa ngắt toàn bộ lúc truyền; Task ưu tiên cao vẫn có thể chạy.

CMSIS `stack_size` dùng **byte**, `.ioc` dùng số word 32-bit trong bảng Task. FreeRTOS heap hiện 8192 byte dùng để cấp các đối tượng/stack động; khác với heap C dự phòng trong linker. Phải phân biệt `xPortGetFreeHeapSize()` với RAM chip còn trống. `uxTaskGetStackHighWaterMark()` báo số word stack nhỏ nhất từng còn, nhân 4 để đổi sang byte trên Cortex-M3.

“Ba Task” là ba Task ứng dụng. FreeRTOS còn Idle và Timer service nội bộ theo cấu hình. SysTick phục vụ scheduler; TIM2 phục vụ `HAL_GetTick`, không sử dụng TIM4 cho buzzer nữa.

## 4. Hai Queue và một snapshot

| Đường truyền | Dung lượng | Ý nghĩa |
|---|---|---|
| Input → Game: `xInputQueueHandle` | 16 × 2 byte | Một sự kiện điều khiển, không phải mẫu ADC |
| Game → Display: `xFrameQueueHandle` | 1 × 2 byte | Một token báo có dữ liệu mới |
| Game → Display: snapshot | Một `GameSnapshot_t` trong RAM | Dữ liệu hình ảnh/game mới nhất, copy qua API |

Queue sao chép giá trị `uint16_t`, nên biến code local có thể hết scope sau khi gửi. Không đổi Queue sang chứa frame hoặc gửi địa chỉ biến local nếu chưa thống nhất lại toàn bộ contract.

Ví dụ: gạt phải đủ debounce → code `0x0002` → Queue input → FSM đang PLAYING gọi `try_move(1,0)` → GameTask tạo snapshot → gửi token `1` → DisplayTask copy snapshot và vẽ. Nếu đang menu, cùng code phải nghĩa là OK. Buzzer xuất hiện theo sự kiện game, không phụ thuộc DisplayTask.

## 5. Vì sao dữ liệu không bị ghi đè lẫn nhau?

GameTask là nơi duy nhất ghi `Engine_t`. Display không đọc trực tiếp `game`, chỉ copy `published` trong critical section ngắn. Producer và consumer dùng cùng cơ chế bảo vệ nên không có nửa frame cũ/nửa frame mới. Copy xong phải ra khỏi critical section trước khi vẽ hoặc I2C.

I2C1 chỉ DisplayTask dùng, ADC chỉ InputTask đọc, nên hiện không cần mutex cho hai ngoại vi này. Nếu thêm Task dùng cùng bus thì phải thiết kế lại quyền sở hữu hoặc mutex. `volatile` không thay mutex/critical section. UART có nhiều producer nên bảo vệ ring buffer bằng PRIMASK, khôi phục đúng trạng thái ngắt trước đó; ISR chỉ lấy byte, không gọi RTOS API.

## 6. Pin và mức logic cả nhóm phải nhớ

| Thiết bị | Pin / quy ước |
|---|---|
| VRx / VRy | PA0 ADC1 / PA1 ADC2, cấp joystick 3.3V |
| SW / PAUSE / ROTATE tùy chọn | PB10 / PA8 / PB14, active-low, pull-up |
| OLED | PB8 SCL / PB9 SDA, I2C1 remap, địa chỉ 7-bit 0x3C |
| Buzzer `低电平触发` | PB6, LOW kêu / HIGH tắt, module tự tạo cao độ |
| UART TX / RX | PA9 / PA10, 115200 8N1, TTL 3.3V |

## 7. Tự kiểm tra kiến thức chung

Mỗi người tự vẽ sơ đồ và trả lời không nhìn tài liệu:

1. Vì sao InputTask không tự di chuyển gạch, DisplayTask không sửa `game.board`?
2. Một lần gạt phải đi qua những hàm/lớp nào trước khi thành pixel OLED?
3. Queue input đầy xử lý thế nào? Queue frame đầy có làm mất trạng thái mới nhất không?
4. Vì sao token `uint16_t` không thể được xem như `GameSnapshot_t`?
5. Khi I2C truyền chậm, tại sao không nên bọc nó trong critical section?
6. Phân biệt priority, chu kỳ, stack byte, stack word và FreeRTOS heap.
7. SysTick, TIM2 và PB6 buzzer đang làm gì? Vì sao LOW không phải trạng thái tắt?
8. Một hàm có tên `task_*_run` có tự tạo Task không? Task thực sự được tạo ở đâu?
9. Build ARM GCC/host PASS chứng minh được gì, chưa chứng minh được gì về kit/Keil?
10. Nếu muốn thay Queue item hoặc chân ADC, cần các thành viên review những nơi nào?

Sau khi trả lời được phần chung, mỗi người chuyển sang hướng dẫn Task được phân công. Kết quả build không thay cho việc hiểu code hoặc đo trên phần cứng.

# Contract chung giữa ba Task

Các định nghĩa chung nằm trong `app/inc/app_shared.h`, `app_cfg.h`, `app_ipc.h`; wrappers/handles nằm ở `main.c`.

| API/tài nguyên | Producer / caller | Consumer / owner | Dữ liệu / quy ước |
|---|---|---|---|
| `task_input_run` | Wrapper `StartInputTask` | InputTask | Chỉ xử lý input, không tự tạo Task |
| `xInputQueueHandle` | InputTask gửi, không chờ | GameTask nhận, không chờ | 16 phần tử, mỗi phần tử `uint16_t` |
| `task_game_run` | Wrapper `StartGameTask` | GameTask | Chủ sở hữu engine và FSM |
| `app_game_copy_snapshot` | DisplayTask gọi | GameTask cung cấp | Copy `GameSnapshot_t`; trả 0 nếu chưa có frame |
| `xFrameQueueHandle` | GameTask gửi token | DisplayTask nhận token | 1 phần tử `uint16_t`; token không chứa snapshot |
| `task_display_run` | Wrapper `StartDisplayTask` | DisplayTask | Render/cập nhật OLED, không sửa game |
| `tone(hz, ms)` | GameTask | Driver buzzer | Module active-low: hz khác 0 bật âm cố định, ms là thời lượng; 0 tắt |
| `app_log` | Các Task | UART ring buffer/IRQ | Ba số unsigned + tên sự kiện; đầy thì bỏ dòng, không chờ |

Mã input: 1 LEFT, 2 RIGHT, 3 ROTATE/UP, 4 DOWN, 5 PAUSE, 6 DROP/SW. Byte thấp là mã, bit 8 `BUTTON_REPEAT_FLAG` là repeat. Ví dụ RIGHT repeat = `0x0102`, không phải mã nút 258 độc lập. Các bit khác không dùng.

InputTask giữ tối đa một press chờ mỗi mã khi Queue đầy; repeat có thể bị bỏ và đếm. PAUSE được thử gửi trước trong mỗi lượt, không chen vào đầu Queue đã có dữ liệu. Khi đổi state, GameTask bỏ các input còn tồn trong Queue để tránh áp dụng phím cũ vào màn mới.

Snapshot chứa cells (đã chèn khối đang rơi), score, lines, level, next piece, menu index, state, difficulty. Snapshot mới nhất thay thế snapshot cũ; nếu Queue token đã đầy, không cần thêm token. Display chỉ lấy bản mới nhất khi tới lượt vẽ; không có bảo đảm vẽ mọi state trung gian.

`game_internal.h` là riêng cho GameTask/engine/FSM; Input/Display không include hoặc truy cập `Engine_t`. Không gửi con trỏ biến local qua Queue. Không tạo lại handles trong từng Task hoặc thêm một bản `main.c` khác.

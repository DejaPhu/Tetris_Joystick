# Phân công học dự án cho 3 người

Mỗi người phụ trách **đúng một application Task**. Mọi người học phần kiến trúc chung trước, sau đó chịu trách nhiệm giải thích, sửa lỗi và kiểm thử Task của mình. Driver/thuật toán dưới đây là phần hỗ trợ cho Task đó, không phải Task mới.

| Thành viên | Task duy nhất | Phạm vi chuyên sâu | Nhánh làm việc |
|---|---|---|---|
| Người 1 — TV1 | **InputTask** | ADC joystick, GPIO, calibration, debounce, repeat, gửi Queue; hỗ trợ cấu hình board/UART | `feature/task-input` |
| Người 2 — TV2 | **GameTask** | Nhận input, FSM, engine Tetris, điểm/độ khó, snapshot, buzzer active-low | `feature/task-game` |
| Người 3 — TV3 | **DisplayTask** | Nhận token, copy snapshot, render framebuffer, SSD1306/I2C, phục hồi bus | `feature/task-display` |

TV1 kiêm người quản lý CubeMX và tích hợp, không có thêm Task thứ tư. `main.c`, `app_cfg.h`, `app_ipc.h`, `app_shared.h` và cấu hình bộ nhớ là phần **cả nhóm phải hiểu**; thay đổi interface cần hai người còn lại review. TV2/TV3 đề xuất thay đổi pin/clock, TV1 thực hiện và kiểm tra chung.

## Tài liệu cần học

1. Cả ba: [kiến trúc và kiến thức bắt buộc](learning/00_KIEN_TRUC_CHUNG.md), [interface](INTERFACES.md), [cấu trúc thư mục](REPO_STRUCTURE.md).
2. TV1: [InputTask](learning/01_INPUT_TASK.md).
3. TV2: [GameTask](learning/02_GAME_TASK.md).
4. TV3: [DisplayTask](learning/03_DISPLAY_TASK.md).
5. Cả ba: [quy trình Git](GIT_WORKFLOW.md), [kế hoạch kiểm tra chéo](learning/04_HOC_VA_KIEM_TRA_CHEO.md).

## Đầu ra mỗi người phải chuẩn bị

| Đầu ra | Yêu cầu cụ thể |
|---|---|
| Sơ đồ Task | Đầu vào, từng bước xử lý, đầu ra, thời gian chờ và nhánh lỗi |
| Giải thích code | Tự lần từ hàm `task_*_run` đến driver/thuật toán, chỉ được biến nào là riêng/chung |
| Bài thực hành | Làm bài trong hướng dẫn riêng trên nhánh của mình, ghi thay đổi và bằng chứng test |
| Demo | Thực hiện tình huống bình thường và tình huống lỗi; phân biệt mock test với test board |
| Review | Review một PR của người khác; nêu được ảnh hưởng tới Queue/snapshot hoặc timing |

## Tiêu chí “nắm rõ”

Không tính hoàn thành chỉ bằng việc đọc tài liệu hoặc test PASS. Mỗi người cần tự giải thích kiến trúc chung, mô tả Task mình không nhìn tài liệu, xác định đúng file cần sửa khi có lỗi và trình bày bằng chứng kiểm thử. Hai người còn lại hỏi chéo theo checklist. Nếu chưa giải thích được một ý, quay lại đúng đoạn code để học rồi kiểm tra lại.

Chưa gán tên thật vì nhóm chưa cung cấp; thay TV1/TV2/TV3 bằng tên thành viên trong bảng sau khi nhóm chốt. Việc phân công giúp xác định trách nhiệm, không thay thế việc cả nhóm phải hiểu hệ thống.

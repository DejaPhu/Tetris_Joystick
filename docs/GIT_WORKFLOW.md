# Quy trình Git cho nhóm 3 người

Áp dụng mô hình nhánh trong `QuyTrinh-Git-2.pdf` cho repo `DejaPhu/Tetris_Joystick`. Nhóm dùng toàn bộ cây nguồn trên mỗi nhánh, không tách thành ba bản firmware hoặc ba bản `main.c` riêng.

| Nhánh | Mục đích |
|---|---|
| `main` | Mốc mã dùng chung; thay đổi tiếp theo qua review/tích hợp. Chưa gắn tag release trước khi thử kit |
| `develop` | Nền phát triển chung cho các nhánh cá nhân |
| `feature/task-input` | TV1: InputTask và driver/filter liên quan |
| `feature/task-game` | TV2: GameTask, engine/FSM, buzzer |
| `feature/task-display` | TV3: DisplayTask, UI và OLED |
| `integration/structure-learning-v1` | Bản tổ chức lại cấu trúc và tài liệu học ban đầu |
| `integration/<phien-ban>` | Các lần ghép và kiểm tra hệ thống sau này |

Ba nhánh feature được khởi tạo cùng một mốc với develop để mỗi người đều có toàn hệ thống và học kiến trúc chung. Sau khi làm bài, chỉ commit phần thuộc phạm vi của mình; interface dùng chung cần cả nhóm thống nhất.

## Bắt đầu phần việc

```sh
git clone https://github.com/DejaPhu/Tetris_Joystick.git
cd Tetris_Joystick
git fetch origin
git switch --track origin/feature/task-input
# TV2 đổi thành feature/task-game; TV3 đổi thành feature/task-display.
```

Nếu nhánh đã có local, dùng `git switch feature/task-input` thay cho tạo tracking lần nữa.

## Một vòng phát triển

1. Cập nhật feature của mình từ develop; commit nhỏ, mô tả hành vi thay đổi.
2. Chạy kiểm tra source/project, host tests và build phù hợp; nếu chưa test board, ghi rõ.
3. Push feature và mở PR vào develop. TV2 review TV1, TV3 review TV2, TV1 review TV3.
4. Leader tích hợp sau review; nếu interface thay đổi thì cả ba cùng xác nhận.
5. Tạo integration từ develop để thử toàn hệ thống, đo heap/stack/timing, ghi bằng chứng.
6. Khi được nhóm nghiệm thu trên phần cứng, đưa kết quả về develop rồi merge vào main, mới cân nhắc tag release.

```sh
git fetch origin
git switch feature/task-input
git rebase origin/develop
python3 tests/check_project.py
./tests/run_host.sh
# Build Keil hoặc ARM GCC và ghi lại phạm vi kiểm thử.
git add app/src/task_input.c
git commit -m "feat(task-input): explain concrete input behavior change"
git push origin feature/task-input
```

Nếu đã push các commit rồi rebase, lịch sử bị thay đổi: chỉ dùng `--force-with-lease` trên nhánh cá nhân đã thống nhất; không force-push main/develop hoặc nhánh chung. Khi conflict, hiểu và sửa hai phía, không chọn toàn bộ một phía một cách máy móc.

## Quy tắc dùng chung

- Một người (TV1) quản lý CubeMX/pin/clock/FreeRTOS config. Generate ở thư mục tạm rồi review vào cây nguồn đã tách lớp.
- Các thành viên không tự thêm Task, tạo Queue hoặc sửa main để nối tắt logic; thay đổi tích hợp phải review.
- Task dùng delay/chờ RTOS, không dùng HAL_Delay để chờ theo thời gian. HAL polling có timeout được giữ trong driver; phải hiểu giới hạn thời gian của từng lệnh.
- ISR không gọi API Task-context của RTOS; nếu sau này cần trao đổi từ ISR thì chọn API FromISR phù hợp. USART1 ISR hiện không gọi RTOS API.
- Không commit build outputs hoặc cấu hình IDE cá nhân. Không sửa thư viện vendor chỉ để thêm comment học tập.
- Quy trình review là thỏa thuận nhóm; bản này chưa bật branch protection trên GitHub. Người có quyền push vẫn có thể thay đổi nhánh, vì vậy nhóm cần thực hiện đúng thỏa thuận.

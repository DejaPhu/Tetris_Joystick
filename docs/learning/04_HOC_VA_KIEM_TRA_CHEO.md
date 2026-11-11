# 04 — Lộ trình học và kiểm tra chéo

Đây là thứ tự học đề xuất, không phải cam kết mọi người sẽ nắm vững chỉ sau một số buổi. Chỉ chuyển bước khi tự giải thích và demo được yêu cầu ở bước đó.

| Chặng | Cả nhóm làm gì | Bằng chứng hoàn thành |
|---|---|---|
| 1. Nắm kiến trúc | Mỗi người đọc phần 00, tự vẽ sơ đồ, lần một event từ joystick tới OLED | Ba sơ đồ do từng người tự vẽ + trả lời phần chung |
| 2. Đọc Task riêng | Mỗi người lần code theo hướng dẫn 01/02/03, đánh dấu input/output, biến riêng/chung, nhịp và nhánh lỗi | Một sơ đồ Task, bảng file và danh sách câu hỏi chưa hiểu |
| 3. Thực hành độc lập | Làm bài trên nhánh feature, chạy mock tests/build, ghi các thay đổi | Diff nhỏ + mô tả tác dụng + host log; ghi rõ phần chưa thử kit |
| 4. Dạy lại hai bạn | Mỗi người trình bày kiến trúc chung rồi đi sâu Task mình, chạy một case thường và một case lỗi | Hai bạn còn lại dùng câu hỏi ở tài liệu để kiểm tra, ghi điểm chưa đạt |
| 5. Ghép và thử kit | Review interface, dùng nhánh integration, kiểm tra UART/stack/heap/timing/pin thực tế | Checklist board với ảnh/log/firmware SHA; chưa có thì không ghi PASS |

## Review vòng tròn

- TV2 review InputTask của TV1: kiểm tra mã input/repeat, Queue đầy và ý nghĩa mã tại FSM.
- TV3 review GameTask của TV2: kiểm tra snapshot/token và giới hạn critical section.
- TV1 review DisplayTask của TV3: kiểm tra timing, không tranh ADC/game, lỗi I2C không làm kẹt input.

Người review không cần viết lại module của bạn nhưng phải hiểu interface và tác động lên toàn hệ thống. Quyền phụ trách Task không có nghĩa giữ kiến thức riêng.

## Mẫu kiểm tra miệng

Mỗi người có thể trình bày khoảng 5 phút phần chung, 10 phút Task riêng rồi trả lời tình huống do hai bạn đặt ra. Thời lượng chỉ để luyện trình bày, không phải giới hạn việc học.

| Tình huống | Người giải thích chính | Hai người còn lại cần hiểu |
|---|---|---|
| Cần đứng yên nhưng gạch tự dịch | TV1 | Raw ADC, tâm, hysteresis và input log liên quan thế nào |
| Giữ SW bị hard drop liên tục | TV1, TV2 cùng lần ranh giới | Phân biệt press/repeat và hành vi FSM |
| Pause lâu rồi resume rơi ngay | TV2 | Deadline và quyền sửa game |
| Buzzer kêu khi không có event | TV2 | LOW kích, HIGH tắt, đường khởi động/fatal |
| OLED đen nhưng game/log vẫn chạy | TV3 | Địa chỉ/remap/init/flush/retry, lỗi màn hình không phải treo game |
| Thay frame Queue size cho bằng snapshot | Cả ba | Vì sao vi phạm contract và phải review toàn bộ đường producer/consumer |

## Checklist mỗi người tự đánh dấu

- [ ] Tôi tự vẽ và giải thích được cả ba Task, hai Queue và snapshot.
- [ ] Tôi chỉ được đúng nơi tạo Task/Queue và nơi khai báo API dùng chung.
- [ ] Tôi giải thích được priority/stack/heap và hai nguồn tick.
- [ ] Tôi lần được từng nhánh chính của Task mình, gồm dữ liệu vào/ra và lỗi.
- [ ] Tôi biết test nào đang có, test đó chứng minh điều gì và không chứng minh điều gì.
- [ ] Tôi có một bài thực hành và giải thích được diff do chính mình làm.
- [ ] Tôi được hai bạn hỏi chéo và đã sửa các điểm chưa hiểu.
- [ ] Tôi phân biệt rõ kết quả mock, ARM GCC, Keil và board thực tế.

Nếu thiếu một ô, ghi lại việc cần học thêm; không xác nhận “đã nắm rõ” thay cho người học.

## Test chung trước khi tích hợp

```sh
python3 tests/check_project.py
./tests/run_host.sh
python3 tools/build_gcc.py  # cần arm-none-eabi-gcc và newlib trong PATH
```

Dùng Keil `MDK-ARM/Tetris_Joystick.uvprojx`, Arm Compiler 5/C99 để rebuild trên máy nhóm và nạp ST-Link. Các bài thực hành là đề bài cho nhóm; repo không tuyên bố các thành viên đã làm xong hoặc đã đo phần cứng.

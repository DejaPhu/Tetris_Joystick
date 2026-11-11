# Kế hoạch kiểm tra cho ba thành viên

| Lớp kiểm tra | Ai thực hiện chính | Phạm vi | Trạng thái hiện tại |
|---|---|---|---|
| Project paths | Cả nhóm, TV1 tổng hợp | Keil include/source/startup, MCU memory, Tasks/Queues/ADC/buzzer | Script PASS; xem báo cáo mới |
| Input mock | TV1 | Calibration, debounce, repeat, ADC lỗi và Queue đầy | Host tests PASS |
| Game/buzzer mock | TV2 | Engine/FSM/điểm/gravity/snapshot contract, buzzer mức logic/deadline | Host tests PASS cho engine/FSM/UI/buzzer; snapshot cần đọc/review và demo tổng |
| Renderer/Display | TV3 | Render các state trên host; pixel/layout/I2C retry trên kit | Host render safety PASS; hình ảnh/bus cần đo |
| ARM cross-build | Cả nhóm | Source/linker trong 64 KB Flash và 20 KB RAM | ARM GCC PASS |
| Keil / board | Cả nhóm theo Task | Compiler 5, nạp kit, dây, hướng, timing, heap/stack, chạy lâu | Chưa thực hiện tại môi trường này |

Chạy chung:

```sh
python3 tests/check_project.py
./tests/run_host.sh
python3 tools/build_gcc.py
```

[Báo cáo tổ chức lại](test-results/STRUCTURE_LEARNING.md), [checklist board](BOARD_TEST.md), [kiểm tra kiến thức và review chéo](learning/04_HOC_VA_KIEM_TRA_CHEO.md). Mỗi lần sửa chức năng cần ghi rõ test đã chạy và giới hạn của test; không ghi chung “PASS tất cả” khi chỉ có mock.

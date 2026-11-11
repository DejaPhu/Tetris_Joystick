# FSM tích hợp

States: WELCOME -> MAIN_MENU -> DIFFICULTY -> PLAYING.
PLAYING -> PAUSED -> PLAYING/MAIN_MENU.
PLAYING -> WIN (điểm >= WIN_SCORE), hoặc GAME_OVER (không sinh được gạch).
WIN/GAME_OVER -> RETRY hoặc MAIN_MENU.

Nút trong menu: ROTATE = lên, DOWN = xuống, DROP = OK, PAUSE = Back.
Nút trong game: LEFT/RIGHT = dịch, ROTATE = xoay, DOWN = soft drop, DROP = hard drop, PAUSE = tạm dừng.

Độ khó đề xuất: EASY 800 ms/ô; NORMAL 500 ms/ô; HARD 250 ms/ô.
Điểm YOU WIN: 1000; đã chốt cho bản tích hợp.

Chỉ GameTask xử lý chuyển state; DisplayTask chỉ hiển thị.

MAIN_MENU có HELP, HELP dùng LEFT/PAUSE/DROP về menu. PAUSED có Resume/Restart/Main Menu. WIN được xét trước spawn; Retry giữ độ khó và reset board/score.

Bản joystick: gạt lên = ROTATE, xuống = DOWN, SW = DROP. Phải = OK, trái = Back; tại PAUSED, trái hoặc PAUSE trở về PLAYING và đặt lại deadline rơi. Tại WIN/GAME_OVER, trái hoặc PAUSE về MAIN_MENU. Repeat chỉ có hiệu lực khi PLAYING.

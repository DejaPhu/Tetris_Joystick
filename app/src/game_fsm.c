/* Only GameTask calls this FSM; InputTask sends physical button codes. */
#include "game_internal.h"
uint8_t handle_button(uint16_t raw)
{
    /* Cung ma phim nhung hanh dong phu thuoc state: SW la OK trong menu,
     * hard drop trong PLAYING. Tach byte ma va bit repeat truoc khi xu ly. */
    ButtonCode_t key = (ButtonCode_t)(raw & BUTTON_CODE_MASK);
    uint8_t repeated = ((raw & BUTTON_REPEAT_FLAG) != 0U);
    /* Menu chi di mot muc moi lan gat; repeat chi co hieu luc khi dang choi. */
    if (repeated && game.state != STATE_PLAYING) return 0U;

    switch(game.state) {
    case STATE_WELCOME:
        if (key == BTN_DROP || key == BTN_RIGHT) {
            game.state = STATE_MAIN_MENU;
            game.menu_index=0U; return 1U;
        }
        return 0U;
    case STATE_MAIN_MENU:
        if(key==BTN_ROTATE) {game.menu_index=(game.menu_index+1U)%2U;return 1U;}
        if(key==BTN_DOWN)   {game.menu_index=(game.menu_index+1U)%2U;return 1U;}
        if(key==BTN_PAUSE || key==BTN_LEFT) {game.state=STATE_WELCOME;return 1U;}
        if(key==BTN_DROP || key==BTN_RIGHT) {
            game.state = game.menu_index?STATE_HELP:STATE_DIFFICULTY;
            game.menu_index=0U; return 1U;
        }
        return 0U;
    case STATE_HELP:
        if(key==BTN_PAUSE || key==BTN_LEFT || key==BTN_DROP) {
            game.state=STATE_MAIN_MENU;game.menu_index=0U; return 1U;
        }
        return 0U;
    case STATE_DIFFICULTY:
        if(key==BTN_ROTATE) {game.menu_index=(game.menu_index+2U)%3U;return 1U;}
        if(key==BTN_DOWN) {game.menu_index=(game.menu_index+1U)%3U;return 1U;}
        if(key==BTN_PAUSE || key==BTN_LEFT) {
            game.state=STATE_MAIN_MENU;game.menu_index=0U;return 1U;
        }
        if(key==BTN_DROP || key==BTN_RIGHT) {
            game.difficulty=(Difficulty_t)game.menu_index;
            start_game();return 1U;
        }
        return 0U;
    case STATE_PLAYING:
        if(key==BTN_LEFT) return try_move(-1,0);
        if(key==BTN_RIGHT) return try_move(1,0);
        if(key==BTN_ROTATE && !repeated) {
            if (try_rotate()) {tone(900U,35U);return 1U;}
            return 0U;
        }
        if(key==BTN_DOWN) {
            if(!try_move(0,1)) lock_piece();
            return 1U;
        }
        if(key==BTN_DROP && !repeated) {
            while(try_move(0,1)) {} /* Maximum 20 rows: finite and nonblocking. */
            lock_piece();return 1U;
        }
        if(key==BTN_PAUSE && !repeated) {
            game.state=STATE_PAUSED;game.menu_index=0U;return 1U;
        }
        return 0U;
    /* Khi resume phai dat lai last_fall_tick, neu khong khoi se roi ngay
     * do tinh ca thoi gian da dung trong man PAUSED. */
    case STATE_PAUSED:
        if(key==BTN_ROTATE) {game.menu_index=(game.menu_index+2U)%3U;return 1U;}
        if(key==BTN_DOWN) {game.menu_index=(game.menu_index+1U)%3U;return 1U;}
        if(key==BTN_PAUSE || key==BTN_LEFT) {
            game.state=STATE_PLAYING;game.last_fall_tick=app_now_ms();return 1U;
        }
        if(key==BTN_DROP || key==BTN_RIGHT) {
            if(game.menu_index==0U) {
                game.state=STATE_PLAYING;game.last_fall_tick=app_now_ms();
            } else if(game.menu_index==1U) start_game();
            else {game.state=STATE_MAIN_MENU;game.menu_index=0U;}
            return 1U;
        }
        return 0U;
    case STATE_WIN:
    case STATE_GAME_OVER:
        if(key==BTN_ROTATE || key==BTN_DOWN) {
            game.menu_index^=1U;return 1U;
        }
        if(key==BTN_PAUSE || key==BTN_LEFT) {
            game.state=STATE_MAIN_MENU;game.menu_index=0U;return 1U;
        }
        if(key==BTN_DROP || key==BTN_RIGHT) {
            if(game.menu_index==0U) start_game();
            else {game.state=STATE_MAIN_MENU;game.menu_index=0U;}
            return 1U;
        }
        return 0U;
    default: return 0U;
    }
}

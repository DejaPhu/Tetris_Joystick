/* Task 2: owns engine/FSM and buzzer. Queue 2 carries tokens, not frame structs. */
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "game_internal.h"
#include "hal_buzzer.h"
#include "app_diagnostics.h"
#include "app_log.h"
#include "hal_uart.h"
/* TV2 - GAME: duy nhat Task nay sua Engine_t. published la ban sao
 * cho TV3 doc; khong tra con tro den du lieu game dang thay doi. */
static GameSnapshot_t published;
static uint8_t frame_available;
uint32_t app_now_ms(void) { return HAL_GetTick(); }
static void publish_snapshot(void)
{
    GameSnapshot_t frame;
    uint8_t r,c;
    uint16_t token=1U;
    static uint32_t last_state=0xffffffffUL,last_score=0xffffffffUL;
    if(last_state!=(uint32_t)game.state || last_score!=game.score) {
        app_log("GAME",game.state,game.score,game.difficulty);
        last_state=game.state;last_score=game.score;
    }
    memset(&frame,0,sizeof(frame));
    /* Ban luu game.board chi chua cac khoi da khoa; chen khoi dang roi
     * vao frame de ve, khong ghi nguoc vao board. */
    memcpy(frame.cells,game.board,sizeof(game.board));
    if(game.state==STATE_PLAYING || game.state==STATE_PAUSED) {
        for(r=0;r<4U;r++) for(c=0;c<4U;c++) {
            if (occupied(game.mask,r,c)) {
                int8_t x=(int8_t)(game.piece_x+c);
                int8_t y=(int8_t)(game.piece_y+r);
                if(x>=0&&x<(int8_t)TETRIS_COLS &&
                   y>=0&&y<(int8_t)TETRIS_ROWS)
                    frame.cells[(uint8_t)y][(uint8_t)x]=game.current_piece+1U;
            }
        }
    }
    frame.score=game.score;
    frame.lines=game.lines;
    frame.level=(uint8_t)(game.lines/10U + 1U);
    frame.menu_index=game.menu_index;
    frame.state=game.state;
    frame.difficulty=game.difficulty;
    frame.next_piece=game.next_piece;
    /* Small bounded copy (~220 bytes) protected from DisplayTask read. */
    /* Chi bao ve phep copy ngan. Khong ve/I2C/log trong critical section. */
    taskENTER_CRITICAL();
    published=frame;
    frame_available=1U;
    taskEXIT_CRITICAL();
    /* Queue capacity 1: if already full, the latest snapshot is still stored. */
    (void)osMessageQueuePut(xFrameQueueHandle,&token,0U,0U);
}

uint8_t app_game_copy_snapshot(GameSnapshot_t *out)
{
    uint8_t ready;
    if (out == NULL) return 0U;
    taskENTER_CRITICAL();
    /* Reader dung cung critical section voi writer de khong doc nua frame. */
    ready=frame_available;
    if(ready) *out=published;
    taskEXIT_CRITICAL();
    return ready;
}


void task_game_run(void *argument) {
    (void)argument;
    uart_debug_init();
    app_log("BOOT",115200U,3U,2U);
    game_init(app_now_ms());
    if (!buzzer_init()) app_fault(APP_FAULT_PWM);
    publish_snapshot();
    for (;;) {
        uint16_t raw;
        uint8_t changed=0U, count=0U;
        uint32_t began=app_now_ms();
        /* Xu ly toi da 16 input moi vong de van danh thoi gian cho gravity,
         * tat buzzer den han va cho cac Task khac chay. */
        while (count++ < 16U && osMessageQueueGet(xInputQueueHandle,&raw,NULL,0U)==osOK) {
            GameState_t before=game.state;
            if (!(raw & BUTTON_REPEAT_FLAG)) app_log("KEY_RX",raw,game.state,game.score);
            if (handle_button(raw)) changed=1U;
            if (game.state != before) {
                /* Do not apply old queued gameplay presses to a new menu/round. */
                while (osMessageQueueGet(xInputQueueHandle,&raw,NULL,0U)==osOK) {}
                break;
            }
        }
        if (game_tick(app_now_ms())) changed=1U;
        /* Buzzer active-low tu tao am; chi kiem tra han, khong delay cho het am. */
        tone_poll();
        if (changed) publish_snapshot();
        app_measure_game(app_now_ms()-began);
        /* Wake early for HARD's 250-ms deadline; normal maximum sleep is 20 ms. */
        osDelay(pdMS_TO_TICKS(game_wait_ms(app_now_ms())));
    }
}

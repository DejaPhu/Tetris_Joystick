#ifndef APP_SHARED_H
#define APP_SHARED_H

/* Shared API contract for the three application Tasks and helper modules.
 * Matches CMSIS-RTOS2 queues in Tetris_Joystick.ioc:
 * 16 x uint16_t input events, 1 x uint16_t frame notification. */

#include <stdint.h>
#include "cmsis_os2.h"

#include "app_cfg.h"
#include "app_ipc.h"
typedef enum {
    BTN_NONE = 0,
    BTN_LEFT = 1,
    BTN_RIGHT = 2,
    BTN_ROTATE = 3,
    BTN_DOWN = 4,
    BTN_PAUSE = 5,
    BTN_DROP = 6
} ButtonCode_t;

/* Logical controls: joystick UP maps to ROTATE, SW maps to DROP.
 * Stored in one uint16_t queue item: bit 8 marks auto-repeat. */
#define BUTTON_REPEAT_FLAG 0x0100U
#define BUTTON_CODE_MASK   0x00FFU

typedef enum {
    STATE_WELCOME = 0,
    STATE_MAIN_MENU,
    STATE_HELP,
    STATE_DIFFICULTY,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_WIN,
    STATE_GAME_OVER
} GameState_t;

typedef enum {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_NORMAL = 1,
    DIFFICULTY_HARD = 2
} Difficulty_t;

typedef struct {
    uint8_t cells[TETRIS_ROWS][TETRIS_COLS]; /* 0 empty, 1..7 occupied */
    uint32_t score;
    uint16_t lines;
    uint8_t next_piece;
    uint8_t level;
    uint8_t menu_index;
    GameState_t state;
    Difficulty_t difficulty;
} GameSnapshot_t;

/* Called only in the existing CubeMX-generated Start*Task wrappers. */
#include "task_input.h"
#include "task_game.h"
#include "task_display.h"

/* Called by DisplayTask only. Copies a snapshot without exposing game globals.
 * Returns 1 once the first frame is ready, else 0. */
uint8_t app_game_copy_snapshot(GameSnapshot_t *out);

#endif /* APP_SHARED_H */

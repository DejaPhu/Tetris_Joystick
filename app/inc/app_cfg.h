#ifndef APP_CFG_H
#define APP_CFG_H
/* Periods in milliseconds; CubeMX task stack sizes in bytes. */
#define TETRIS_COLS          10U
#define TETRIS_ROWS          20U
#define TETRIS_WIN_SCORE     1000UL
#define TETRIS_INPUT_PERIOD_MS   10U
#define TETRIS_GAME_PERIOD_MS    20U
#define TETRIS_DISPLAY_PERIOD_MS 160U /* OLED 100-kHz I2C: full frame ~100 ms */

/* CMSIS-RTOS2 stack_size tinh bang BYTE; cau hinh .ioc nhap so WORD 32-bit.
 * Chi mot nguoi quan ly cau hinh va dong bo hai noi khi thay doi. */
#define APP_INPUT_STACK_BYTES (192U * 4U)
#define APP_GAME_STACK_BYTES (320U * 4U)
#define APP_DISPLAY_STACK_BYTES (384U * 4U)
#define APP_INPUT_PRIORITY osPriorityNormal3
#define APP_GAME_PRIORITY osPriorityNormal2
#define APP_DISPLAY_PRIORITY osPriorityNormal1
#endif

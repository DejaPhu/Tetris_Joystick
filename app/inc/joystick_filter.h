#ifndef JOYSTICK_FILTER_H
#define JOYSTICK_FILTER_H
#include <stdint.h>
/* Direction bits match the first four hal_button_pressed indices. */
#define JOY_LEFT  1U
#define JOY_RIGHT 2U
#define JOY_UP    4U
#define JOY_DOWN  8U
typedef struct {
    uint32_t sum_x, sum_y;
    uint16_t center_x, center_y, min_x, max_x, min_y, max_y;
    uint8_t samples, ready, armed, direction;
} JoystickFilter;
void joystick_filter_init(JoystickFilter *j);
/* One sample pair each input tick. Invalid samples disarm until neutral again. */
uint8_t joystick_filter_step(JoystickFilter *j,uint16_t x,uint16_t y,uint8_t valid);
#endif

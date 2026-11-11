#ifndef HAL_JOYSTICK_H
#define HAL_JOYSTICK_H
#include <stdint.h>
/* InputTask alone owns reads. Returns 0 on either ADC error, no partial pair. */
uint8_t hal_joystick_read(uint16_t *x,uint16_t *y);
#endif

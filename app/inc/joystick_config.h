#ifndef JOYSTICK_CONFIG_H
#define JOYSTICK_CONFIG_H
/* ADC 12-bit, Vref = 3.3 V. Adjust orientation after mounting the module. */
#ifndef JOYSTICK_SWAP_XY
#define JOYSTICK_SWAP_XY 0
#endif
#ifndef JOYSTICK_INVERT_X
#define JOYSTICK_INVERT_X 0
#endif
#ifndef JOYSTICK_INVERT_Y
#define JOYSTICK_INVERT_Y 0
#endif
/* Default: X low = left, Y low = up. Only PAUSE, ROTATE and SW remain active. */
#ifndef JOYSTICK_LEGACY_BUTTONS
#define JOYSTICK_LEGACY_BUTTONS 0
#endif
#define JOYSTICK_CENTER_DEFAULT 2048U
#define JOYSTICK_CENTER_MIN     1500U
#define JOYSTICK_CENTER_MAX     2600U
#define JOYSTICK_CAL_SAMPLES    32U
#define JOYSTICK_CAL_SPREAD     120U
#define JOYSTICK_ENTER          700
#define JOYSTICK_RELEASE        400
#endif

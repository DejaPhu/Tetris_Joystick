#ifndef HAL_BUZZER_H
#define HAL_BUZZER_H
#include <stdint.h>
uint8_t buzzer_init(void);
/* Active-low module: hz!=0 enables its fixed pitch for ms; 0 mutes.
 * GameTask calls tone_poll every loop; no blocking delay. */
void tone(uint16_t hz, uint32_t ms);
void tone_poll(void);
#endif

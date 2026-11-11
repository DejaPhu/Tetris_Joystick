#include <assert.h>
#include <stdio.h>
#include "main.h"
#include "hal_buzzer.h"
static uint32_t now;
static GPIO_PinState level;
uint32_t HAL_GetTick(void) {return now;}
void HAL_GPIO_WritePin(void *port,uint16_t pin,GPIO_PinState state) {
 assert(port==BUZZER_GPIO_Port && pin==BUZZER_Pin);level=state;
}
int main(void) {
 level=GPIO_PIN_RESET;assert(buzzer_init());assert(level==GPIO_PIN_SET);
 tone(900,35);assert(level==GPIO_PIN_RESET);
 now=34;tone_poll();assert(level==GPIO_PIN_RESET);
 now=35;tone_poll();assert(level==GPIO_PIN_SET);
 tone(900,35);tone(0,100);assert(level==GPIO_PIN_SET);
 tone(900,35);tone(900,0);assert(level==GPIO_PIN_SET);
 now=0xfffffff0U;tone(1000,32);now=15;tone_poll();assert(level==GPIO_PIN_RESET);
 now=16;tone_poll();assert(level==GPIO_PIN_SET);
 now=100;tone(900,30);now=120;tone(1000,50);
 now=130;tone_poll();assert(level==GPIO_PIN_RESET);
 now=170;tone_poll();assert(level==GPIO_PIN_SET);
 tone(900,35);assert(buzzer_init());assert(level==GPIO_PIN_SET);
 puts("PASS: active-low buzzer init/off HIGH, on LOW, deadline, zero duration, cancel, retrigger, tick wrap");
}

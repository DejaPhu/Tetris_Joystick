/* Active buzzer module: LOW = sound, HIGH = silent. No external audio PWM. */
#include "main.h"
#include "hal_buzzer.h"
static uint32_t sound_deadline;
static uint8_t sound_active;
uint8_t buzzer_init(void) {
    tone(0U,0U);
    return 1U;
}
void tone(uint16_t hz,uint32_t ms) {
    /* The module generates its own pitch; retain hz as the existing API's
     * enable/mute argument. Zero duration must never latch the sound on. */
    if (!hz || !ms) {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
        sound_active=0U;
        return;
    }
    /* Signed deadline comparisons support intervals below 2^31 ms. */
    if(ms>0x7fffffffUL)ms=0x7fffffffUL;
    sound_deadline=HAL_GetTick()+ms;
    sound_active=1U;
    HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}
void tone_poll(void) {
    if(sound_active && (int32_t)(HAL_GetTick()-sound_deadline)>=0)
        tone(0U,0U);
}

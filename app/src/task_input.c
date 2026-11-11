/* Task 1: ADC joystick + active-low switches -> uint16_t events. */
#include "main.h"
#include "hal_gpio.h"
#include "hal_joystick.h"
#include "joystick_filter.h"
#include "joystick_config.h"
#include "app_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_shared.h"
#include "button_filter.h"
#include "app_diagnostics.h"
typedef struct { uint8_t code; ButtonFilter filter; uint8_t pending; } Button;
static Button buttons[]={
 {BTN_LEFT,{0},0},
 {BTN_RIGHT,{0},0},
 {BTN_ROTATE,{0},0},
 {BTN_DOWN,{0},0},
 {BTN_PAUSE,{0},0},
 {BTN_DROP,{0},0}
};
void task_input_run(void *argument) {
    TickType_t last=xTaskGetTickCount();
    uint32_t raw_sampled=0U;
    JoystickFilter joystick;
    joystick_filter_init(&joystick);
    (void)argument;
    for (;;) {
        uint32_t i,now=HAL_GetTick();
        uint16_t x=0U,y=0U;
        uint8_t valid=hal_joystick_read(&x,&y);
        uint8_t direction=joystick_filter_step(&joystick,x,y,valid);
        /* Retry PAUSE first when capacity returns; queued data is never reset here. */
        for (i=0;i<6U;i++) {
            uint32_t index=(i==0U)?4U:((i<=4U)?i-1U:5U);
            Button *b=&buttons[index];
            uint8_t repeat=(b->code==BTN_LEFT||b->code==BTN_RIGHT||b->code==BTN_DOWN);
            uint8_t pressed=0U;
            /* PAUSE, optional ROTATE and SW (former DROP pin) stay digital. */
            if(index>=4U || index==2U || JOYSTICK_LEGACY_BUTTONS)
                pressed=hal_button_pressed((uint8_t)index);
            if(index<4U && (direction&(1U<<index)))pressed=1U;
            uint8_t event=button_filter_step(&b->filter,
                pressed,repeat,now);
            uint16_t code=b->code;
            if (event==1U) {
                if (b->pending) app_diagnostics.input_dropped++;
                b->pending=1U;
                app_log("KEY_PRESS",b->code,index,now);
            }
            if (b->pending) {
                if (osMessageQueuePut(xInputQueueHandle,&code,0U,0U)==osOK) {
                    b->pending=0U;app_log("KEY_ENQ",code,index,0U);
                }
                else app_diagnostics.input_queue_full++;
            } else if (event==2U) {
                code|=BUTTON_REPEAT_FLAG;
                if (osMessageQueuePut(xInputQueueHandle,&code,0U,0U)!=osOK)
                    app_diagnostics.input_dropped++;
            }
        }
        if ((uint32_t)(now-raw_sampled)>=1000U) {
            raw_sampled=now;
            app_log("JOY_RAW",x,y,valid);
            app_log("JOY_CENTER",joystick.center_x,joystick.center_y,joystick.armed);
        }
        vTaskDelayUntil(&last,pdMS_TO_TICKS(TETRIS_INPUT_PERIOD_MS));
    }
}

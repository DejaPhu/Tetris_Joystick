/* TV3 - DISPLAY: chu so huu I2C1. Chi doc snapshot, khong sua Engine_t. */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_shared.h"
#include "hal_oled.h"
#include "ui_renderer.h"
#include "app_diagnostics.h"
void task_display_run(void *argument)
{
    const TickType_t period=pdMS_TO_TICKS(TETRIS_DISPLAY_PERIOD_MS);
    TickType_t last=xTaskGetTickCount();
    /* Token chi bao co du lieu moi; noi dung frame nam trong published. */
    uint16_t token;
    GameSnapshot_t frame;
    uint8_t initialized=0U;
    uint8_t refresh_pending=1U;
    (void)argument;

    for (;;) {
        if(!initialized) {
            initialized=oled_init();
            if(initialized) refresh_pending=1U;
        }
        /* Lay token khong cho. Neu flush loi, refresh_pending van giu lai
         * de thu frame moi nhat sau khi khoi phuc bus. */
        if(osMessageQueueGet(xFrameQueueHandle,&token,NULL,0U)==osOK)
            refresh_pending=1U;

        if(initialized && refresh_pending && app_game_copy_snapshot(&frame)) {
            /* Copy snapshot da xong truoc khi render va truyen I2C cham.
             * pixels tro vao framebuffer tinh, khong can cap phat heap. */
            const uint8_t *pixels=ui_render(&frame);
            uint32_t began=HAL_GetTick();
            uint8_t ok=oled_flush(pixels);
            app_measure_display(HAL_GetTick()-began,ok);
            if(ok) refresh_pending=0U;
            else initialized=0U; /* retry initialization on I2C error */
        }
        /* Mat OLED khong duoc giu CPU: bus clear huu han, ngu 500 ms roi thu lai.
         * Khi hoat dong binh thuong, ve theo chu ky 160 ms. */
        if (!initialized) { app_measure_display(0U,0U); oled_recover(); osDelay(pdMS_TO_TICKS(500U)); last=xTaskGetTickCount(); }
        else vTaskDelayUntil(&last,period);
    }
}

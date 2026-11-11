/* Inspect app_diagnostics in the Keil Watch window; no fourth application Task. */
#include "main.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_diagnostics.h"
#include "app_log.h"
#include "hal_uart.h"
#include "hal_buzzer.h"
/* Chan doan la du lieu quan sat; volatile khong phai co che khoa du lieu.
 * Moi nhom chi so co Task ghi rieng; UART log chi lay mau de debug. */
volatile AppDiagnostics app_diagnostics;
extern osThreadId_t InputTaskHandle,GameTaskHandle,DisplayTaskHandle;
void app_fault(AppFault reason) {
    tone(0U,0U); /* Silence before logging/halting, even if a beep was active. */
    app_diagnostics.fault=reason;
    app_log("FAULT",reason,0U,0U);
    uart_debug_panic_flush(); /* Best effort; Watch remains authoritative on fatal errors. */
    __disable_irq();
    for (;;) { /* Break here in ST-Link to inspect fault. */ }
}
void app_measure_game(uint32_t duration) {
    static uint32_t sampled;
    uint32_t now=HAL_GetTick();
    if (duration>app_diagnostics.game_max_ms) app_diagnostics.game_max_ms=duration;
    if ((uint32_t)(now-sampled)<1000U) return;
    sampled=now;
    app_diagnostics.heap_free=xPortGetFreeHeapSize();
    app_diagnostics.heap_min=xPortGetMinimumEverFreeHeapSize();
    app_diagnostics.input_stack_free_words=uxTaskGetStackHighWaterMark((TaskHandle_t)InputTaskHandle);
    app_diagnostics.game_stack_free_words=uxTaskGetStackHighWaterMark((TaskHandle_t)GameTaskHandle);
    app_diagnostics.display_stack_free_words=uxTaskGetStackHighWaterMark((TaskHandle_t)DisplayTaskHandle);
    app_log("HEAP",app_diagnostics.heap_free,app_diagnostics.heap_min,0U);
    app_log("STACK_WORDS",app_diagnostics.input_stack_free_words,app_diagnostics.game_stack_free_words,app_diagnostics.display_stack_free_words);
    app_log("INPUT_QUEUE",app_diagnostics.input_queue_full,app_diagnostics.input_dropped,0U);
    app_log("TIMING_OLED",app_diagnostics.game_max_ms,app_diagnostics.display_max_ms,app_diagnostics.oled_errors);
    { uint32_t lost,bytes; uart_debug_stats(&lost,&bytes);app_log("UART_TX",lost,bytes,0U); }
}
void app_measure_display(uint32_t duration,uint8_t ok) {
    if (duration>app_diagnostics.display_max_ms) app_diagnostics.display_max_ms=duration;
    if (!ok) app_diagnostics.oled_errors++;
}

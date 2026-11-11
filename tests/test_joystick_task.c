#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include "main.h"
#include "task.h"
#include "app_shared.h"
#include "app_diagnostics.h"
static jmp_buf done;
static uint32_t now;
static unsigned count;
static uint16_t events[20];
static uint32_t timestamps[20];
volatile AppDiagnostics app_diagnostics;
osMessageQueueId_t xInputQueueHandle=(void *)1,xFrameQueueHandle;
TickType_t xTaskGetTickCount(void) { return now; }
uint32_t HAL_GetTick(void) { return now; }
void vTaskDelayUntil(TickType_t *last,TickType_t period) {
 *last+=period;now=*last;if(now>=3000)longjmp(done,1);
}
void app_log(const char *event,uint32_t a,uint32_t b,uint32_t c) {
 (void)event;(void)a;(void)b;(void)c;
}
uint8_t hal_button_pressed(uint8_t index) {
 if(index==0)return 1; /* disconnected legacy LEFT cannot affect joystick mode */
 if(index==4)return now>=2130 && now<2200;
 if(index==5)return now>=2100 && now<2600;
 if(index==2)return now>=2700 && now<2800;
 return 0;
}
uint8_t hal_joystick_read(uint16_t *x,uint16_t *y) {
 *x=*y=2048;
 if(now==400)*x=0; /* 10ms glitch rejected by debounce */
 if(now>=500 && now<1160)*x=4095;
 if((now>=1200 && now<1300)||(now>=1400 && now<2000))*y=0;
 return now!=1040;
}
osStatus_t osMessageQueuePut(osMessageQueueId_t queue,const void *item,uint8_t priority,uint32_t timeout) {
 assert(queue==xInputQueueHandle && priority==0 && timeout==0);
 if((now>=530 && now<560)||(now>=830 && now<860)||(now>=2130 && now<2180))return osErrorResource;
 assert(count<20);events[count]=*(const uint16_t *)item;timestamps[count++]=now;return osOK;
}
int main(void) {
 static const uint16_t expected[]={BTN_RIGHT,BTN_RIGHT|BUTTON_REPEAT_FLAG,BTN_RIGHT|BUTTON_REPEAT_FLAG,
 BTN_ROTATE,BTN_ROTATE,BTN_PAUSE,BTN_DROP,BTN_ROTATE};
 static const uint32_t times[]={560,930,1030,1230,1430,2180,2180,2730};
 unsigned i;
 if(!setjmp(done))task_input_run(NULL);
 assert(count==sizeof expected/sizeof expected[0]);
 for(i=0;i<count;i++){assert(events[i]==expected[i]);assert(timestamps[i]==times[i]);}
 assert(app_diagnostics.input_dropped==1 && app_diagnostics.input_queue_full>=3);
 puts("PASS: InputTask analog-to-queue, glitch rejection, ADC disarm, legacy disabled, rotate/SW once, PAUSE priority, pending press retry, dropped repeats");
 return 0;
}

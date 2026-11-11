#include <assert.h>
#include <stdio.h>
#include "main.h"
#include "hal_joystick.h"
ADC_HandleTypeDef hadc1={0},hadc2={1};
static unsigned fail_axis,fail_phase,starts,polls,stops;
static HAL_StatusTypeDef result(ADC_HandleTypeDef *a,unsigned phase) {
 return a->index==fail_axis && phase==fail_phase?HAL_TIMEOUT:HAL_OK;
}
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *a) { starts++;return result(a,1); }
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *a,uint32_t timeout) {
 assert(timeout==1U);polls++;return result(a,2);
}
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *a) { return a->index?3000:1000; }
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *a) { stops++;return result(a,3); }
int main(void) {
 unsigned axis,phase;uint16_t x=0,y=0;
 fail_phase=0;assert(hal_joystick_read(&x,&y));assert(x==1000 && y==3000);
 assert(starts==2 && polls==2 && stops==2);
 for(axis=0;axis<2;axis++)for(phase=1;phase<=3;phase++) {
  fail_axis=axis;fail_phase=phase;stops=starts=polls=0;x=111;y=222;
  assert(!hal_joystick_read(&x,&y));assert(x==111 && y==222);
  assert(stops==axis+1);assert(starts==axis+1);
 }
 assert(!hal_joystick_read(NULL,&y));assert(!hal_joystick_read(&x,NULL));
 puts("PASS: ADC pair, bounded timeout, start/poll/stop failure on either axis, no partial output");
 return 0;
}

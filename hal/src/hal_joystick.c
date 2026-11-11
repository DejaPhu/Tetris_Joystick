#include "main.h"
#include "hal_joystick.h"
extern ADC_HandleTypeDef hadc1,hadc2;
static uint8_t read_axis(ADC_HandleTypeDef *adc,uint16_t *value) {
    HAL_StatusTypeDef status=HAL_ADC_Start(adc);
    if(status==HAL_OK)status=HAL_ADC_PollForConversion(adc,1U);
    if(status==HAL_OK)*value=(uint16_t)HAL_ADC_GetValue(adc);
    if(HAL_ADC_Stop(adc)!=HAL_OK)status=HAL_ERROR;
    return status==HAL_OK;
}
uint8_t hal_joystick_read(uint16_t *x,uint16_t *y) {
    uint16_t vx,vy;
    if(!x || !y)return 0U;
    if(!read_axis(&hadc1,&vx) || !read_axis(&hadc2,&vy))return 0U;
    *x=vx;*y=vy;return 1U;
}

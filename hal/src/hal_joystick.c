#include "main.h"
#include "hal_joystick.h"
/* TV1: ADC duoc khoi tao/calibrate trong main; chi InputTask doc hai handle. */
extern ADC_HandleTypeDef hadc1,hadc2;
static uint8_t read_axis(ADC_HandleTypeDef *adc,uint16_t *value) {
    /* Poll toi da theo timeout 1 ms; luon thu Stop, ke ca khi Start/Poll loi. */
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
    /* Chi cong bo cap hoan chinh: neu Y loi thi X moi cung khong duoc dung. */
    *x=vx;*y=vy;return 1U;
}

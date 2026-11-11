#include <stdint.h>
#define BUZZER_GPIO_Port ((void *)1)
#define BUZZER_Pin 64U
typedef enum { GPIO_PIN_RESET,GPIO_PIN_SET } GPIO_PinState;
void HAL_GPIO_WritePin(void *port,uint16_t pin,GPIO_PinState state);
uint32_t HAL_GetTick(void);

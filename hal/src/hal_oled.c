/* SSD1306 page addressing; HAL expects the 7-bit address shifted left once. */
#include <string.h>
#include "main.h"
#include "hal_oled.h"
extern I2C_HandleTypeDef hi2c1;
#define OLED_WIDTH 128U
#define OLED_I2C_ADDR (0x3CU << 1)
/* TV3: 0x00 bao byte lenh, 0x40 bao byte du lieu cho SSD1306.
 * HAL I2C yeu cau dia chi 0x3C dich trai 1 bit; khong dich hai lan. */
static HAL_StatusTypeDef oled_cmd(uint8_t cmd)
{
    uint8_t packet[2]={0x00U,cmd};
    return HAL_I2C_Master_Transmit(&hi2c1,OLED_I2C_ADDR,packet,2U,150U);
}

uint8_t oled_init(void)
{
    /* SSD1306 128x64, internal charge-pump, PAGE addressing mode. */
    static const uint8_t commands[]={
        0xAE,0xD5,0x80,0xA8,0x3F,0xD3,0x00,0x40,
        0x8D,0x14,0x20,0x02,0xA1,0xC8,0xDA,0x12,
        0x81,0x8F,0xD9,0xF1,0xDB,0x40,0xA4,0xA6,
        0x2E,0xAF
    };
    uint32_t i;
    for(i=0;i<sizeof(commands);i++)
        if(oled_cmd(commands[i])!=HAL_OK) return 0U;
    return 1U;
}

uint8_t oled_flush(const uint8_t *framebuffer)
{
    uint8_t page,packet[1U+OLED_WIDTH];
    packet[0]=0x40U;  /* data control byte */
    /* Moi page chua 128 cot x 8 pixel = 128 byte; 8 page tao 128x64.
     * I2C duoc goi ngoai critical section, co timeout va tra loi cho Task. */
    for(page=0;page<8U;page++) {
        if(oled_cmd((uint8_t)(0xB0U|page))!=HAL_OK) return 0U;
        if(oled_cmd(0x00U)!=HAL_OK) return 0U;
        if(oled_cmd(0x10U)!=HAL_OK) return 0U;
        memcpy(&packet[1],&framebuffer[page*OLED_WIDTH],OLED_WIDTH);
        if(HAL_I2C_Master_Transmit(&hi2c1,OLED_I2C_ADDR,
                                   packet,sizeof(packet),200U)!=HAL_OK)
            return 0U;
    }
    return 1U;
}


/* Bounded I2C bus clear; DisplayTask alone owns the bus. External pull-ups required. */
static void bus_pause(void) { volatile uint32_t n; for(n=0;n<120U;n++) __NOP(); }
void oled_recover(void) {
    GPIO_InitTypeDef gpio={0}; uint8_t n;
    (void)HAL_I2C_DeInit(&hi2c1);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_SET);
    gpio.Pin=GPIO_PIN_8|GPIO_PIN_9;gpio.Mode=GPIO_MODE_OUTPUT_OD;
    gpio.Speed=GPIO_SPEED_FREQ_HIGH;gpio.Pull=GPIO_NOPULL;HAL_GPIO_Init(GPIOB,&gpio);
    if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)==GPIO_PIN_SET) {
        for(n=0;n<9U && HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)==GPIO_PIN_RESET;n++) {
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);bus_pause();
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);bus_pause();
        }
        /* STOP: release SDA after SCL. Never drive either signal high actively. */
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);bus_pause();
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);bus_pause();
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);bus_pause();
    }
    (void)HAL_I2C_Init(&hi2c1); /* MSP restores PB8/PB9 alternate function/remap. */
}

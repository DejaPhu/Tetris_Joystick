/* Fixed storage, no extra RTOS Queue/Task. Producers copy short complete lines
 * with interrupts masked; TXE IRQ drains one byte per entry at 115200 baud. */
#include "main.h"
#include "hal_uart.h"
#define TX_SIZE 512U
static char tx[TX_SIZE];
static volatile uint16_t head,tail;
static volatile uint32_t dropped,sent;
static uint8_t ready;
static uint16_t next(uint16_t p) { return (uint16_t)((p+1U)%TX_SIZE); }
void uart_debug_init(void) {
    uint32_t mask=__get_PRIMASK();
    __disable_irq();
    USART1->CR1 &= ~USART_CR1_TXEIE;
    head=tail=0U; dropped=sent=0U;ready=1U;
    HAL_NVIC_SetPriority(USART1_IRQn,6U,0U);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __set_PRIMASK(mask);
}
uint8_t uart_debug_write(const char *data,uint16_t length) {
    uint16_t free_bytes,i,pos;
    uint32_t mask;
    if (!data || !length) return 0U;
    /* Nhieu Task co the ghi log: bao ve ca dong ngan bang PRIMASK,
     * phuc hoi dung mask cu thay vi bat ngat vo dieu kien. */
    mask=__get_PRIMASK();__disable_irq();
    free_bytes=(uint16_t)((tail+TX_SIZE-head-1U)%TX_SIZE);
    /* All-or-nothing lines; never wait when the ring is full. */
    if (!ready || length>128U || length>free_bytes) {
        dropped++;__set_PRIMASK(mask);return 0U;
    }
    pos=head;
    for(i=0;i<length;i++) {tx[pos]=data[i];pos=next(pos);}
    head=pos;
    USART1->CR1 |= USART_CR1_TXEIE;
    __set_PRIMASK(mask);return 1U;
}
/* ISR khong goi RTOS API; moi ngat TXE lay mot byte tu ring buffer. */
void uart_debug_irq(void) {
    if ((USART1->CR1 & USART_CR1_TXEIE) && (USART1->SR & USART_SR_TXE)) {
        if(tail!=head) {USART1->DR=(uint8_t)tx[tail];tail=next(tail);sent++;}
        if(tail==head) USART1->CR1 &= ~USART_CR1_TXEIE;
    }
}
void uart_debug_stats(uint32_t *lost,uint32_t *bytes) {
    uint32_t mask=__get_PRIMASK();__disable_irq();
    *lost=dropped;*bytes=sent;__set_PRIMASK(mask);
}
void uart_debug_panic_flush(void) {
    uint32_t budget=2500000UL,mask=__get_PRIMASK();
    __disable_irq();
    while(ready && tail!=head && budget--) uart_debug_irq();
    __set_PRIMASK(mask);
}

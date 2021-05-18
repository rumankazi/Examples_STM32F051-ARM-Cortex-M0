#ifndef INC_DRIVERS_USART_DRIVER_H_
#define INC_DRIVERS_USART_DRIVER_H_
#include <stdint.h>

void usart1_init(void);
void usart1_rcv_interrupt_en(void);
void usart1_putch(char data);
void usart1_puts(char* str);
void usart1_puti(int32_t data, uint8_t digits);

void usart2_init(void);
void usart2_rcv_interrupt_en(void);
void usart2_putch(char data);
void usart2_puts(char* str);
void usart2_puti(int32_t data, uint8_t digits);
char usart2_getch(void);
void usart2_flush_queue(void);
#endif /* INC_DRIVERS_USART_DRIVER_H_ */

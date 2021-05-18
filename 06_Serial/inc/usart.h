/*
 * usart_driver.h
 *
 *  Created on: Mar 22, 2015
 *      Author: Bhavin
 */

#ifndef INC_DRIVERS_USART_DRIVER_H_
#define INC_DRIVERS_USART_DRIVER_H_

void usart1_init(void);
void usart1_rcv_interrupt_en();
void usart1_putch(char data);
void usart1_puts(char* str);
void usart1_puti(int32_t data, uint8_t digits);

#endif /* INC_DRIVERS_USART_DRIVER_H_ */

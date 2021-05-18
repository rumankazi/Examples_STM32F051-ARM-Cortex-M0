#include "stm32f0xx_conf.h"

#include "lcd.h"
#include "delay.h"
#include "util.h"
#include "usart.h"

#define TIMER_TICK_HZ 1000u

volatile unsigned long timer_tick = 0;

void SysTick_Handler(void)
{
  if(timer_tick > 0)
	  --timer_tick;
}

int main(void)
{	       
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 	// enable the clock to GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 	// enable the clock to GPIOC

	// Put PORTC.8 in output mode
	GPIOC->MODER |= (1 << 16);

	// Put PORTC.9 in output mode
	GPIOC->MODER |= (1 << 18);

	// Put PORTA.0 in input mode
	GPIOA->MODER &= ~(3 << 0);

	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	
	// Initialize the lcd	
	lcd_init();
  usart1_init();
  
	lcd_puts("   WSN KIT  ");	
	lcd_gotoxy(1, 0);
	lcd_puts("    USART TEST   ");  
  delay_ms(2000);
  
  // Continuously send data on the usart
	while(1)
	{        
    usart1_puts("USART1 TEST\r\n");
    delay_ms(1000);
	}
}

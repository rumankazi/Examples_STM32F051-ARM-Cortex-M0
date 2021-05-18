#include "stm32f0xx_conf.h"
#include "stm32f0xx_dac.h"
#include "lcd.h"
#include "delay.h"
#include "util.h"

#define TIMER_TICK_HZ 1000u

volatile unsigned long timer_tick = 0;

void SysTick_Handler(void)
{
  if(timer_tick > 0)
	  --timer_tick;
}

void dac_init()
{
  GPIO_InitTypeDef GPIO_init_structure;
  DAC_InitTypeDef DAC_init_structure;

  // Enable the DAC interface clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  // DAC pins configuration
  // 1. Enable the clock for the DAC pin (PA.4)  
  // 2. Enable alternate function for PA.4
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_init_structure.GPIO_Pin = GPIO_Pin_4;
  GPIO_init_structure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_init_structure);
  
  // Configure the DAC channel
  DAC_init_structure.DAC_Trigger = DAC_Trigger_None;
  DAC_init_structure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_init_structure);

  // Enable the DAC channel
  DAC_Cmd(DAC_Channel_1, ENABLE);
}

int main(void)
{	      
  int32_t dac_data = 0;
  int16_t inc_dir = 1;
  char buffer[16];
  
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
  dac_init();
  	  
	lcd_puts("  WSN KIT  ");	
	lcd_gotoxy(1, 0);
	lcd_puts("    DAC TEST    ");  
  delay_ms(2000);
  
  lcd_clear();
  lcd_puts("DAC Value (PA4): ");
  
  // Generate a step wave on DAC output whose amplitude is
  // proportional to the voltage on ADC input.
	while(1)
	{            
    DAC_SetChannel1Data(DAC_Align_12b_R, dac_data);
    DAC_SoftwareTriggerCmd(0, ENABLE);
    dac_data += (100 * inc_dir);
    if(dac_data > 4095)
    {
      dac_data = 4095;
      inc_dir = -1;
    }
    else if(dac_data < 0)
    {
      dac_data = 0;
      inc_dir = 1;
    }
    
   // int_to_str(dac_data, 5, buffer, sizeof(buffer));
  //  lcd_gotoxy(1, 0);
 //   lcd_puts(buffer);
 //   delay_ms(1000);    
	}

}

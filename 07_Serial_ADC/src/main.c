#include "stm32f0xx_conf.h"
#include "stm32f0xx_adc.h"

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

void adc_init()
{
  GPIO_InitTypeDef GPIO_init_structure;
  ADC_InitTypeDef  ADC_init_structure;

  // Enable the ADC interface clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  // ADC pins configuration
  // 1. Enable the clock for the ADC GPIOs
  // 2. Configure these ADC pins in analog mode using GPIO_Init();  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_init_structure.GPIO_Pin = GPIO_Pin_0;
  GPIO_init_structure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_init_structure);

  // Configure the ADC conversion resolution, data alignment, external
  // trigger and edge, scan direction and Enable/Disable the continuous mode
  // using the ADC_Init() function.
  ADC_init_structure.ADC_Resolution = ADC_Resolution_12b;
  ADC_init_structure.ADC_ContinuousConvMode = ENABLE;
  ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    
  ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_init_structure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_init_structure);

  // Calibrate ADC before enabling
  ADC_GetCalibrationFactor(ADC1);
  // Activate the ADC peripheral using ADC_Cmd() function.
  ADC_Cmd(ADC1, ENABLE);

  // Wait until ADC enabled
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);
  
  // Confgure 
  ADC_ChannelConfig(ADC1, ADC_Channel_7, ADC_SampleTime_28_5Cycles);
  // Start the first conversion
  ADC_StartOfConversion(ADC1);
}

int main(void)
{	       
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
  usart1_init();
  adc_init();
  
	lcd_puts("   WSN KIT  ");	
	lcd_gotoxy(1, 0);
	lcd_puts("  ADC USART TEST ");  
  delay_ms(2000);
  
  lcd_clear();
  lcd_puts("Temperature (PA7):");
  
  // Continuously send data on the usart
	while(1)
	{       
    uint16_t adc_value;       
    adc_value = (ADC_GetConversionValue(ADC1)-70)/10;
    
    int_to_str(adc_value, 5 /*num of digits*/, buffer, sizeof(buffer));
    lcd_gotoxy(1, 0);
    lcd_puts(buffer);
    
    usart1_puts(buffer);
    usart1_puts("\r\n");
    delay_ms(1000);
        
	}
}

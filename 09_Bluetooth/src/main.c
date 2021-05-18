#include "stm32f0xx_conf.h"
#include "stm32f0xx_adc.h"

#include "delay.h"
#include "util.h"
#include "usart.h"

#define TIMER_TICK_HZ 1000u

#define WSN_PORT GPIOC
#define WSN_GPS_PIN 2
#define WSN_WIFI_PIN 3
#define WSN_BT_PIN 4
#define WSN_ZBEE_PIN 5

volatile unsigned long timer_tick = 0;

void SysTick_Handler(void)
{
  if(timer_tick > 0)
	  --timer_tick;
}

__inline void wsn_select_gps()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_GPS_PIN);
}

__inline void wsn_select_wifi()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_WIFI_PIN);
}

__inline void wsn_select_bt()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_BT_PIN);
}

__inline void wsn_select_zbee()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_ZBEE_PIN);
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
  
  // Make PORTC.4,.5,.6,.7 as output for select line
  WSN_PORT->MODER |= ((1 << (WSN_GPS_PIN * 2)) | (1 << (WSN_WIFI_PIN * 2)) | 
                      (1 << (WSN_BT_PIN * 2))  | (1 << (WSN_ZBEE_PIN * 2)));
  
  // Pullup mux pins
  WSN_PORT->PUPDR |= ((1 << (WSN_GPS_PIN * 2)) | (1 << (WSN_WIFI_PIN * 2)) | 
                      (1 << (WSN_BT_PIN * 2))  | (1 << (WSN_ZBEE_PIN * 2)));
                      
	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	
  usart1_init();
  usart2_init();
  adc_init();
  
  // Select BT for usart2 on wsn channel mux
  wsn_select_bt();
  
	usart1_puts("WSN KIT\r\n");	
  usart2_puts("WSN KIT\r\n");	
	
	usart1_puts("Bluetooth TEST\r\n");
  usart2_puts("Blueetooth TEST\r\n");
  delay_ms(1000);
  
  // Continuously send data on the usart
	while(1)
	{       
    uint16_t adc_value;       
    adc_value = (ADC_GetConversionValue(ADC1)-70)/10;
    
    int_to_str(adc_value, 5 /*num of digits*/, buffer, sizeof(buffer));
    
    usart1_puts("Temperature (BT):");
    usart1_puts(buffer);
    usart1_puts("\r\n");
    
    usart2_puts("Temperature (BT):");
    usart2_puts(buffer);
    usart2_puts("\r\n");
    delay_ms(1000);
        
	}
}

#include "stm32f0xx_conf.h"
#include "stm32f0xx_adc.h"
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
	//GPIOA->MODER &= ~(3 << 0);
 
	
	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	
	// Initialize the lcd	
	lcd_init();
  
  // Initialie adc for PA.0
  adc_init();  
  	  
	lcd_puts("  WSN KIT  ");	
	lcd_gotoxy(1, 0);
	lcd_puts("   Sensor Test   ");  
  delay_ms(2000);
  
  lcd_clear();
  
	
		
	while(1)
	{     
    uint16_t adc_value;   
  /*  
		// Confgure Pin A.7 LM 35 Temperature Sensor %%%%%%%%%%%%%%%%%%%%
  ADC_ChannelConfig(ADC1, ADC_Channel_7, ADC_SampleTime_28_5Cycles);
  // Start the first conversion
  ADC_StartOfConversion(ADC1);
    lcd_gotoxy(0, 0);
	 lcd_puts("TEMPERATURE: ");

		adc_value = (ADC_GetConversionValue(ADC1)-70)/10;
     
		// To convert adc value to string
     int_to_str(adc_value, 5, buffer, sizeof(buffer));
     lcd_gotoxy(1, 0);
     lcd_puts(buffer);
     delay_ms(1000);
	       
*/

		
	    //	 Pin A.5 DISTANCE (SHARP) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		  ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_28_5Cycles);
      // Start the first conversion
      ADC_StartOfConversion(ADC1);
			lcd_gotoxy(0, 0);
      lcd_puts("DISTANCE (cm): ");
      adc_value = 27- (ADC_GetConversionValue(ADC1))/100; 
      int_to_str(adc_value, 5, buffer, sizeof(buffer));
      lcd_gotoxy(1, 0);
      lcd_puts(buffer);
      delay_ms(100);
          			
			
		
		
		
/*
			//Pin A.6 Optocoupler %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			ADC_ChannelConfig(ADC1, ADC_Channel_6, ADC_SampleTime_28_5Cycles);
			// Start the first conversion
			ADC_StartOfConversion(ADC1);

			if ((ADC_GetConversionValue(ADC1)) >= 3000)
       { 
				 lcd_clear();
			   lcd_puts("OPTO:");
				 lcd_gotoxy(1, 0);
				 lcd_puts("OBJECT PRESENT");
			 }
      else
			{
				lcd_clear();
			  lcd_puts("OPTO:");
				lcd_gotoxy(1, 0);	
				lcd_puts("   NO OBJECT  ");
			}
			    delay_ms(500);
	*/
	
	/*
	    // Pin A.1 LDR %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		  ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_28_5Cycles);
			// Start the first conversion
			ADC_StartOfConversion(ADC1);
			lcd_gotoxy(0, 0);
			lcd_puts("Darkness: ");

			adc_value = (ADC_GetConversionValue(ADC1));
      int_to_str(adc_value, 5, buffer, sizeof(buffer));
      
			lcd_gotoxy(1, 0);
      lcd_puts(buffer);
      delay_ms(300);
	    
	*/

/*
				// Pin A.0 MICROPHONE & SPEAKER INTERFCAE %%%%%%%%%%%%%%%%%%%%%%%%%%%
			   int16_t dac_data = 0;
				 			 
			   lcd_puts("MIC & SPK:");	
       //DAC SPEAKER PA.4
  			 dac_init();
			 
			  // ADC MIC: PA.0
				 ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_1_5Cycles);
			  // Start the first conversion
			   ADC_StartOfConversion(ADC1);
			
				while(1)
	    {      
						adc_value = (ADC_GetConversionValue(ADC1)); 
			      dac_data = adc_value;
				
           DAC_SetChannel1Data(DAC_Align_12b_R, dac_data);
           DAC_SoftwareTriggerCmd(0, ENABLE);
   
			}
	*/		
	  
		
		
   /* 	//Pin A.11 MAGNETIC  SENSOR  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //(USED FOR Pin A.11 MAGNETIC SENSOR) Put PORTA.11 in input mode
	     
			 GPIOA->MODER &= ~(3 << 22);
	   
		 if ((GPIOA->IDR & 0x0800) !=0x0000)
		{
				lcd_clear();
			  lcd_puts("MAGNETIC:");
				lcd_gotoxy(1, 0);	
			lcd_puts("M: NO DANGER");
	      delay_ms(1000);	
		}
		else
		{
		  	lcd_clear();
			  lcd_puts("MAGNETIC:");
				lcd_gotoxy(1, 0);	
			lcd_puts("M: DANGER");			
        delay_ms(1000);		
		}
				
		*/
		 
		 
	}

}

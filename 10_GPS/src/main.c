#include "stm32f0xx_conf.h"

#include "delay.h"
#include "util.h"
#include "usart.h"
#include "at_cmd.h"
#include "gps.h"

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

/* Example program for interfacing STM32F051 with SIM908 module for GPS
 */
int main(void)
{	       
  char latitude[32], longitude[32];
  uint32_t cnt;
  
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
  
  WSN_PORT->PUPDR |= ((1 << (WSN_GPS_PIN * 2)) | (1 << (WSN_WIFI_PIN * 2)) | 
                      (1 << (WSN_BT_PIN * 2))  | (1 << (WSN_ZBEE_PIN * 2)));

	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	
  usart1_init();
  usart2_init();
  
  // Select GPS for usart2 on wsn channel mux
  wsn_select_gps();
  
	usart1_puts("STM32F051\r\n");	
	usart1_puts("GPS TEST\r\n");
  delay_ms(1000);
  
  if(gps_init() == GPS_ERROR)
  {
    usart1_puts("Error initializing GPS\r\n");
    while(1);
  }
  
  // Continuously send data on the usart
  usart1_puts("GPS init done\r\n");
  delay_ms(100);
  
  usart1_puts("Trying to acquire GPS");
  cnt = 0;
  while(is_gps_acquired() == GPS_ERROR && cnt < 12)
  {
    delay_ms(5000);
    usart1_putch('.');
    ++cnt;
  }
  
  usart1_puts("\r\n");
  if(cnt == 24)
  {
    usart1_puts("Could not acquire GPS\r\n");
  }
  else
  {
    usart1_puts("GPS acquired\r\n");
  }
  
	if(gps_get_lat_lon(latitude, sizeof(latitude), longitude, sizeof(longitude)) == GPS_ERROR)
  {
    usart1_puts("Error getting lat and long\r\n");
    while(1);
  }
  usart1_puts("lat=");
  usart1_puts(latitude);
  usart1_puts("\r\nlong=");
  usart1_puts(longitude);
  usart1_puts("\r\n");
}

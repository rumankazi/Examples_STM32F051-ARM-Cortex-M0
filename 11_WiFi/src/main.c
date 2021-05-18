#include "stm32f0xx_conf.h"

#include "delay.h"
#include "util.h"
#include "usart.h"
#include "at_cmd.h"
#include "wifi.h"
#include "string.h"

#include <stdio.h>

#define TIMER_TICK_HZ 1000u

#define WSN_PORT GPIOC
#define WSN_GPS_PIN 2
#define WSN_WIFI_PIN 3
#define WSN_BT_PIN 4
#define WSN_ZBEE_PIN 5

#define GOOGLE_API_IP "216.58.196.10"
#define ADDR_TAG "<formatted_address>"

const char* http_req = "GET /maps/api/geocode/xml?latlng=%s,%s HTTP/1.1\r\n" 
                       "Host: maps.googleapis.com\r\n\r\n";

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

/* Example program for interfacing STM32F051 with ESP8266 module
 * The code connects to a WIFI hotspot and does a address lookup
 * for a given latitude and longitude. It uses google map api for the lookup.
 */
int main(void)
{
  char *lat = "18.9132062";
  char *lon = "72.8152144";
  char ssid[32];
  char pwd[32];
  uint32_t cnt;
  uint32_t i;
  uint8_t is_match = 0;
  uint32_t tag_size = sizeof(ADDR_TAG) - 1;
  char rcv_buf[512];
  
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
  
  // Select BT for usart2 on wsn channel mux
  wsn_select_wifi();
  
	usart1_puts("STM32F051\r\n");	
	usart1_puts("Wifi TEST\r\n");
  delay_ms(1000);
  
  if(wifi_init() == WIFI_ERROR)
  {
    usart1_puts("Wifi init error\r\n");
    while(1);
  }
  
  usart1_puts("Wifi init done\r\n");
  usart1_puts("Enter router id: ");
  cnt = 0;
  do
  {
    ssid[cnt] = usart1_getch();
    usart1_putch(ssid[cnt]);
  }while(ssid[cnt++] != '\n');
  ssid[cnt - 2] = 0;
  
  usart1_puts("\r\nEnter password: ");
  cnt = 0;
  do
  {
    pwd[cnt] = usart1_getch();
    usart1_putch('*');
  }while(pwd[cnt++] != '\n');
  pwd[cnt - 2] = 0;
  
  usart1_puts("\r\n");
  
  if(wifi_connect_router(ssid, pwd) == WIFI_ERROR)
  {
    usart1_puts("Error connecting to router\r\n");
    while(1);
  }
  usart1_puts("Connected to ");
  usart1_puts(ssid);
  usart1_puts("\r\n");
  
  if(wifi_open_conn("TCP", GOOGLE_API_IP, "80") == WIFI_ERROR)
  {
    usart1_puts("Error opening port to "GOOGLE_API_IP);
    while(1);
  }
  usart1_puts("Connection to "GOOGLE_API_IP" opened\r\n");
  
  sprintf(rcv_buf, http_req, lat, lon);
  wifi_send_req(rcv_buf, strlen(rcv_buf));
  
  // handle response  
  while(is_match == 0)
  {
    rcv_buf[0] = rcv_buf[1];
    rcv_buf[1] = usart2_getch();
    if(rcv_buf[0] == '?' && rcv_buf[1] == '>')
    {
      is_match = 1;
    }
  }
  
  cnt = 0;
  // arbritrary limit
  while(cnt < sizeof(rcv_buf))
  {
    // TODO: This is bad. main should not know about usart directly.    
    rcv_buf[cnt++] = usart2_getch();    
    if(cnt >= tag_size)
    {
      if(!strncmp(&rcv_buf[cnt - tag_size], ADDR_TAG, tag_size))
      {
        i = 0;
        do
        {
          rcv_buf[i] = usart2_getch();
        }while(rcv_buf[i++] != '<');
        rcv_buf[i - 1] = 0;
        break;
      }
    }
  }
  
  if(cnt < sizeof(rcv_buf))
  {
    usart1_puts("Address:\r\n");
    usart1_puts(rcv_buf);
  }
  else
  {
    usart1_puts("Error getting addr\r\n");
  }
  while(1);
}

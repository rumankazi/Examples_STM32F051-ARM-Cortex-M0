#include "stm32f0xx_conf.h"

#include "at_cmd.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

#define AT_BUFFER_SIZE 128

uint8_t at_buffer[AT_BUFFER_SIZE];

int8_t at_send_dummy()
{
  usart2_puts("\r\n\r\n");
  return AT_SUCCESS;
}

int8_t at_send_receive(char* send_buf, char* rcv_buf, uint32_t rcv_buf_size)
{
  uint32_t cnt = 0;
  char tmp_buf[4];
  uint8_t is_match = 0;
  int32_t send_size = strlen(send_buf);
  int32_t i = 0;
    
  usart2_flush_queue();
  usart2_puts("AT+");
  usart2_puts(send_buf);
  usart2_puts("\r\n");
  
  while(!is_match && cnt < (send_size + 50))
  {
    tmp_buf[cnt % sizeof(tmp_buf)] = usart2_getch();    
    i = (cnt + 1) % sizeof(tmp_buf);    
    ++cnt;
    if(tmp_buf[i] != '\r') continue;
    
    i = (i + 1) % sizeof(tmp_buf);
    if(tmp_buf[i] != '\n') continue;
    
    i = (i + 1) % sizeof(tmp_buf);
    if(tmp_buf[i] != '\r') continue;
    
    i = (i + 1) % sizeof(tmp_buf);
    if(tmp_buf[i] != '\n') continue;        
    
    is_match = 1;
  }
  if(cnt == (send_size + 10))
  {
    // This means that we did not get a \r\n\r\n reply even after all
    // of cmd was sent. 
    return AT_ERROR;
  }
  
  cnt = 0;
  while(cnt < rcv_buf_size)
  {
    rcv_buf[cnt] = usart2_getch();    
    if(cnt >= 1)
    {
      if(!strncmp(&(rcv_buf[cnt - 1]), "OK", 2))
      {
        return AT_SUCCESS;
      }      
    }
    
    if(cnt >= 4)
    {
      if(!strncmp(&(rcv_buf[cnt - 4]), "ERROR", 5))
      {
        return AT_ERROR;
      }      
    }
    ++cnt;
  }
  return AT_ERROR;
}

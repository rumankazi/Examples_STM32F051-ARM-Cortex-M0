#include "stm32f0xx_conf.h"

#include "zigbee.h"
#include "usart.h"
#include "delay.h"
#include "at_cmd.h"

#define TIMEOUT 4

int8_t zigbee_init(char* name)
{
  int8_t reply;
  delay_ms(1000);
  at_direct_send("+++");
  delay_ms(1000);
  if(at_check_ok(TIMEOUT) == AT_ERROR)
  {
    return ZIGBEE_ERROR;
  }
  
  at_direct_send("ATNI");
  at_direct_send(name);
  at_direct_send("\r");
  reply = at_check_ok(TIMEOUT);
  at_direct_send("ATCN\r");
  if(reply == AT_ERROR)
  {    
    return ZIGBEE_ERROR;
  }  
  return ZIGBEE_SUCCESS;
}

int8_t zigbee_connect(char* node_name)
{
  int8_t reply;
  delay_ms(1000);
  at_direct_send("+++");
  delay_ms(1000);
  if(at_check_ok(TIMEOUT) == AT_ERROR)
  {
    return ZIGBEE_ERROR;
  }
  
  at_direct_send("ATDN");
  at_direct_send(node_name);
  at_direct_send("\r");
  reply = at_check_ok(TIMEOUT);
  at_direct_send("ATCN\r");
  if(reply == AT_ERROR)
  {    
    return ZIGBEE_ERROR;
  }  
  return ZIGBEE_SUCCESS;
}

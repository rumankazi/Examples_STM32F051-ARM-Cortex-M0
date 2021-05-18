#include "stm32f0xx_conf.h"

#include "wifi.h"
#include "at_cmd.h"
#include "delay.h"
#include "util.h"
#include "usart.h"

#include <string.h>
#include <stdlib.h>

int8_t wifi_init()
{
  char rcv_buf[128];
  
  at_send_receive("RST", rcv_buf, sizeof(rcv_buf));
  delay_ms(3000);
  
  /* Put in station mode */
  if(at_send_receive("CWMODE=1",rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return WIFI_ERROR;
  }
  
  /* Put in multiple connection mode */
  if(at_send_receive("CIPMUX=1",rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return WIFI_ERROR;
  }
  return WIFI_SUCCESS;
}

int8_t wifi_connect_router(char* ssid, char* password)
{
  char rcv_buf[128];
  const char* cmd = "CWJAP=\"";
  uint32_t ssid_size = strlen(ssid);
  uint32_t password_size = strlen(password);
  uint32_t cmd_size = strlen(cmd);
  char* buf = malloc(ssid_size + password_size + cmd_size + 6);
  
  strcpy(buf, cmd);
  strcpy(&buf[cmd_size], ssid);
  strcpy(&buf[cmd_size + ssid_size], "\",\"");
  strcpy(&buf[cmd_size + ssid_size + 3], password);
  strcpy(&buf[cmd_size + ssid_size + 3 + password_size], "\"\0");
  
  if(at_send_receive(buf, rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    free(buf);
    return WIFI_ERROR;
  }
  free(buf);
  return WIFI_SUCCESS;
}

int8_t wifi_open_conn(char* proto, char* ip, char* port)
{
  char rcv_buf[128];
  
  uint32_t cmd_size = sizeof("CIPSTART=4,");
  uint32_t proto_size = strlen(proto);
  uint32_t ip_size = strlen(ip);
  uint32_t port_size = strlen(port);
  
  uint32_t size = cmd_size + proto_size + ip_size + port_size + 8;
  
  char* buf = malloc(size);
  
  strcpy(buf, "CIPSTART=4,\"");
  strcpy(&buf[cmd_size ], proto);
  
  buf[cmd_size + proto_size] = '"';
  buf[cmd_size + proto_size + 1] = ',';
  buf[cmd_size + proto_size + 2] = '"';
  strcpy(&buf[cmd_size + proto_size + 3], ip);
  
  buf[cmd_size + proto_size + ip_size + 3] = '"';
  buf[cmd_size + proto_size + ip_size + 4] = ',';
  strcpy(&buf[cmd_size + proto_size + ip_size + 5], port);
  
  if(at_send_receive(buf, rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    free(buf);
    return WIFI_ERROR;
  }
  free(buf);
  return WIFI_SUCCESS;
}

int8_t wifi_send_req(const char* req, uint32_t size)
{  
  char* buf = malloc(sizeof("AT+CIPSEND=4,") + 10);
  
  strcpy(buf, "AT+CIPSEND=4,");
  int_to_str(size, 4, &buf[13], 10);
  buf[17] = '\r';
  buf[18] = '\n';
  buf[19] = 0;
  
  at_direct_send(buf);
  delay_ms(100);
  at_direct_send(req);
    
  free(buf);
  
  return WIFI_SUCCESS;
}

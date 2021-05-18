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

int8_t wifi_send_req(uint8_t conn_no, const char* req, uint32_t size)
{  
  char buf[32];// = malloc(sizeof("AT+CIPSEND=4,") + 10);
  
  //strcpy(buf, "AT+CIPSEND=1,");
	memcpy(buf, "AT+CIPSEND=1,", 13);
	buf[11] = conn_no;
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

int8_t wifi_setup_server(uint16_t port_no)
{
	char rcv_buf[128];
	char* buf;
	
	if(at_send_receive("CWMODE=2",rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return WIFI_ERROR;
  }
	
	if(at_send_receive("CIPMUX=1",rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return WIFI_ERROR;
  }
	
	buf = malloc(sizeof("AT+CIPSERVER=1,80\r\n") + 5 + 3);
	strcpy(buf, "AT+CIPSERVER=1,80\r\n");
	/*
	int_to_str(port_no, 5, &buf[15], 6);
	buf[20] = '\r';
	buf[21] = '\n';
	buf[22] = 0;
	*/
	at_direct_send(buf);
	free(buf);
	
	return WIFI_SUCCESS;
}

int8_t wifi_get_ip(char* buf, uint32_t buf_size)
{
	char rcv_buf[128];
	uint8_t loc;
	uint8_t cnt;
	
	if(at_send_receive("CIFSR", rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
	{
		return WIFI_ERROR;
	}
	
	loc = 0;
	while(loc < sizeof(rcv_buf))
	{
		if(!strncmp("APIP", &rcv_buf[loc], 4))
		{
			break;
		}
	}
	
	if(loc == sizeof(rcv_buf))
	{
		return WIFI_ERROR;
	}
	
	cnt = 0;
	loc += 6;
	while(rcv_buf[loc] != '"' && cnt < buf_size)
	{
		buf[cnt] = rcv_buf[loc];
		cnt++;
		loc++;
	}
	buf[cnt] = 0;
	return WIFI_SUCCESS;
}

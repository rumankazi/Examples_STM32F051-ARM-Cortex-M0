#ifndef _WIFI_H
#define _WIFI_H
#include <stdint.h>

#define WIFI_ERROR 0
#define WIFI_SUCCESS 1

int8_t wifi_init(void);
int8_t wifi_open_conn(char* proto, char* ip, char* port);
int8_t wifi_send_req(const char* req, uint32_t size);
int8_t wifi_connect_router(char* ssid, char* password);

#endif

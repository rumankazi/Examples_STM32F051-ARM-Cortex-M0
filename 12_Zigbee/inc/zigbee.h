#ifndef _ZIGBEE_H
#define _ZIGBEE_H
#include <stdint.h>

#define ZIGBEE_SUCCESS 1
#define ZIGBEE_ERROR 0

int8_t zigbee_init(char* name);
int8_t zigbee_connect(char* node_name);

#endif


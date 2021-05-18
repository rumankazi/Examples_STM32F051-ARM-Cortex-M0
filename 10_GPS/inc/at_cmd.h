#ifndef _AT_CMD_H
#define _AT_CMD_H
#include <stdint.h>

#define AT_SUCCESS 1
#define AT_ERROR 0

int8_t at_send_dummy(void );
int8_t at_send_receive(char* send_buf, char* rcv_buf, uint32_t rcv_buf_size);

#endif

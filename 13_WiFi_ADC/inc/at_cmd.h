#ifndef _AT_CMD_H
#define _AT_CMD_H
#include <stdint.h>

#define AT_SUCCESS 1
#define AT_ERROR 0

int8_t at_send_dummy(void );
int8_t at_send_receive(char* send_buf, char* rcv_buf, uint32_t rcv_buf_size);
int8_t at_direct_send(const char* data);
// Waits until you get a \r\n\r\n
int8_t at_wait_till_double_newline(uint32_t max_size);

#endif

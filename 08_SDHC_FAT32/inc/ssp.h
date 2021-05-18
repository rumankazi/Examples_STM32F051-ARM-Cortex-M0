#ifndef __SSP_H__
#define __SSP_H__
#include <stdint.h>

uint32_t spi1_init(void);
void spi1_send(uint8_t *buf, uint32_t length);
void spi1_receive(uint8_t *buf, uint32_t length);
uint8_t spiSendRcv(uint8_t outgoing);

#endif  /* __SSP_H__ */




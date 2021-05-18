#include "util.h"

void int_to_str(uint32_t num, uint32_t digits, char* buffer, uint32_t buffer_size)
{
  uint32_t cnt = digits - 1;
  uint32_t divisor = 1;
  
  while(cnt--)
  {
    divisor *= 10;
  }
  
  for(cnt = 0; cnt < digits && cnt < (buffer_size - 1); ++cnt, divisor /= 10)
  {
    buffer[cnt] = (num / divisor) + 0x30;
    num %= divisor;
  }
  
  buffer[cnt] = 0; // null terminate the string;
}

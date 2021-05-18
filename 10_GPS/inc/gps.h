#ifndef _GPS_H
#define _GPS_H
#include <stdint.h>

#define GPS_ERROR 0
#define GPS_SUCCESS 1

int8_t gps_init(void );
int8_t gps_get_lat_lon(char* lat, uint32_t lat_size, char* lon, uint32_t lon_size);
int8_t is_gps_acquired(void);
#endif

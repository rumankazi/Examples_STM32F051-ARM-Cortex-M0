#include "stm32f0xx_conf.h"
#include "gps.h"
#include "at_cmd.h"
#include <string.h>

int8_t gps_init()
{
  char rcv_buf[64];
	
  // Flush out any previous partial command
  at_send_receive("\r\n\r\n", rcv_buf, sizeof(rcv_buf));
  
  if(at_send_receive("CGPSPWR=1", rcv_buf, sizeof(rcv_buf)) == AT_ERROR ||
     at_send_receive("CGPSRST=0", rcv_buf, sizeof(rcv_buf)) == AT_ERROR ||
     at_send_receive("CGPSIPR=9600", rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return GPS_ERROR;
  }
  return GPS_SUCCESS;
}

int8_t gps_get_lat_lon(char* lat, uint32_t lat_size, char* lon, uint32_t lon_size)
{
  char rcv_buf[128];	
  int32_t cnt = 0, i = 0;
  
  // Flush out any previous partial command
  at_send_receive("\r\n\r\n", rcv_buf, sizeof(rcv_buf));
  
  if(at_send_receive("CGPSINF=0", rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return GPS_ERROR;
  }
  //  Response format for INF=0 is as follows:
  // mode,longitude,lattitude,altitude,UTC_Time,TTFF,num,speed,course
  
  // Get till the first ','
  while(rcv_buf[cnt++] != ',');
  
  // Get the string till next comma, that will be the longitude
  i = 0;
  while(rcv_buf[cnt] != ',' && i < (lon_size - 1))
  {
    lon[i++] = rcv_buf[cnt++];
  }
  lon[i] = 0;
  
  // Get the string till next comma, that will be the latitude
  i = 0;
  ++cnt;
  while(rcv_buf[cnt] != ',' && i < (lat_size - 1))
  {
    lat[i++] = rcv_buf[cnt++];
  }
  lat[i] = 0;
  
  return GPS_SUCCESS;
}

int8_t is_gps_acquired()
{
  char rcv_buf[128];	
  
  // Flush out any previous partial command
  at_send_receive("\r\n\r\n", rcv_buf, sizeof(rcv_buf));
  
  if(at_send_receive("CGPSSTATUS?", rcv_buf, sizeof(rcv_buf)) == AT_ERROR)
  {
    return GPS_ERROR;
  }
  
  // Possible values are:
  // Location Unknown
  // Location Not Fix
  // Location 2D Fix
  // Location 3D Fix
  // We check for 3 characters starting from the word after "Location"
  if(!strncmp(&(rcv_buf[22]), "Unk", 3) || !strncmp(&(rcv_buf[22]), "Not", 3))
  {
    return GPS_ERROR;
  }
  if(!strncmp(&(rcv_buf[22]), "2D ", 3) || !strncmp(&(rcv_buf[22]), "3D ", 3))
  {
    return GPS_SUCCESS;
  }
  
  return GPS_ERROR;
}

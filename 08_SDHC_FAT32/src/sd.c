/*****************************************************************************\
 *              efs - General purpose Embedded Filesystem library              *
 *          --------------------- -----------------------------------          *
 *                                                                             *
 * Filename : sd.c                                                             *
 * Revision : Initial developement                                             *
 * Description : This file contains the functions needed to use efs for        *
 *               accessing files on an SD-card.                                *
 *                                                                             *
 * This program is free software; you can redistribute it and/or               *
 * modify it under the terms of the GNU General Public License                 *
 * as published by the Free Software Foundation; version 2                     *
 * of the License.                                                             *
 *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * As a special exception, if other files instantiate templates or             *
 * use macros or inline functions from this file, or you compile this          *
 * file and link it with other works to produce a work based on this file,     *
 * this file does not by itself cause the resulting work to be covered         *
 * by the GNU General Public License. However the source code for this         *
 * file must still be made available in accordance with section (3) of         *
 * the GNU General Public License.                                             *
 *                                                                             *
 * This exception does not invalidate any other reasons why a work based       *
 * on this file might be covered by the GNU General Public License.            *
 *                                                                             *
 *                                                    (c)2006 Lennart Yseboodt *
 *                                                    (c)2006 Michael De Nil   *
 \*****************************************************************************/

/*****************************************************************************/
#include <stdio.h>
#include "ssp.h"
#include "sd.h"
#include "usart.h"
/*****************************************************************************/
//#define UART_DBG
unsigned char SD_version, cardType, SDHC_flag,sd_buf[512];

uint8_t sd_init()
{
  unsigned char i, response;
  unsigned int retry=0 ;

#ifdef UART_DBG
  unsigned char buf[64];
#endif

  uint8_t ucTemp;
  SD_SELECTION_CLR;
  for(i=0;i<50;i++)
    ucTemp=spiSendRcv(0xff);   //80 clock pulses spent before sending the first command

  SD_SELECTION_SET;
  for(i=0;i<5;i++)
    ucTemp=spiSendRcv(0xff);   //80 clock pulses spent before sending the first command
  SD_SELECTION_CLR;

  do
  {

    sd_Command(0,0,0); //send 'reset & go idle' command
    response=sd_Resp8b(1);
    retry++;
    if(retry>0x20) 
    {
#ifdef UART_DBG
      sprintf((char *)buf,"\n\r %02x Card Failed to init",response);
      usart1_puts((char *)buf);
#endif
      return 1;   //time out, card not detected
    }

  } while(response != 0x01);

  SD_SELECTION_SET;
  ucTemp=spiSendRcv(0xff);
  ucTemp=spiSendRcv(0xff);
#ifdef UART_DBG
  usart1_puts("\n\rCard initialized in spi mode");
#endif

  retry = 0;

  SD_version = 2; //default set to SD compliance with ver2.x; 
  //this may change after checking the next command
  do
  {
    sd_Command(8,0x0000,0x01AA); //Check power supply status, mendatory for SDHC card
    response=sd_Resp8b(1);
    retry++;
    if(retry>0x5) 
    {
      SD_version = 1;
#ifdef UART_DBG
      sprintf((char *)buf,"\n\r %02x SD Version 1",response);
      usart1_puts((char *)buf);
#endif

      cardType = 1;
      break;
    } //time out

  }while(response != 0x01);

  retry = 0;

  do
  {
    sd_Command(55,0,0); //CMD55, must be sent before sending any ACMD command
    response=sd_Resp8b(1);
    sd_Command(41,0x4000,0x0000); //ACMD41
    response=sd_Resp8b(1);

    retry++;
    if(retry>0xfe) 
    {
      //TX_NEWLINE;
#ifdef UART_DBG
      sprintf((char *)buf,"\n\r %02x ACMD41 failed",response);
      usart1_puts((char *)buf);
#endif
      return 2;  //time out, card initialization failed
    } 

  }while(response != 0x00);


  retry = 0;
  SDHC_flag = 0;

  if (SD_version == 2)
  { 
    do
    {
      sd_Command(58,0,0);
      response=spiSendRcv(0xff); //cant use sd_resp8b because dont want to make SEL pin high yet
      response=spiSendRcv(0xff); //cant use sd_resp8b because dont want to make SEL pin high yet
#ifdef UART_DBG
      sprintf((char *)buf,"%02x CMD58 resp",response);
      usart1_puts((char *)buf);
#endif
      retry++;
      if(retry>0xfe) 
      {

        cardType = 0;
        break;
      } //time out

    }while(response != 0x00);

    response=spiSendRcv(0xff);
    SD_SELECTION_SET;
    if(response&0x40)
      SDHC_flag=1;
    else
      SDHC_flag=0;

    if(SDHC_flag == 1) 
      cardType = 2;
    else 
      cardType = 3;
  }

  //SD_sendCommand(CRC_ON_OFF, OFF); //disable CRC; deafault - CRC disabled in SPI mode
  //SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512; default size is 512


  return 0; //successful return
}

uint8_t sd_is_sdhc()
{
  return SDHC_flag;
}
/*****************************************************************************/

void sd_Command(uint8_t cmd, uint16_t paramx, uint16_t paramy)
{
  spiSendRcv(0xff);
  SD_SELECTION_CLR;
  spiSendRcv(0x40 | cmd);
  spiSendRcv((uint8_t) (paramx >> 8)); /* MSB of parameter x */
  spiSendRcv((uint8_t) (paramx)); /* LSB of parameter x */
  spiSendRcv((uint8_t) (paramy >> 8)); /* MSB of parameter y */
  spiSendRcv((uint8_t) (paramy)); /* LSB of parameter y */

  if(cmd==8)
    spiSendRcv(0x87);
  else
    spiSendRcv(0x95); /* Checksum (should be only valid for first command (0) */

  //	spiSendRcv(0xff); /* eat empty command - response */
}
/*****************************************************************************/

uint8_t sd_Resp8b(uint8_t sel)
{
  uint8_t i;
  uint8_t resp;

  /* Respone will come after 1 - 8 pings */
  for(i=0;i<8;i++){
    resp = spiSendRcv(0xff);
    if(resp != 0xff)
      return(resp);
  }

  if(sel)
    SD_SELECTION_SET;	
  return(resp);
}
/*****************************************************************************/

uint16_t sd_Resp16b(uint8_t sel)
{
  uint16_t resp;

  resp = ( sd_Resp8b(0) << 8 ) & 0xff00;
  resp |= spiSendRcv(0xff);

  if(sel)
    SD_SELECTION_SET;
  return(resp);
}
/*****************************************************************************/

void sd_Resp8bError(uint8_t value)
{
  switch(value)
  {
    case 0x40:
      usart1_puts("\nArgument out of bounds.\n");
      break;
    case 0x20:
      usart1_puts("\nAddress out of bounds.\n");
      break;
    case 0x10:
      usart1_puts("\nError during erase sequence.\n");
      break;
    case 0x08:
      usart1_puts("\nCRC failed.\n");
      break;
    case 0x04:
      usart1_puts("\nIllegal command.\n");
      break;
    case 0x02:
      usart1_puts("\nErase reset (see SanDisk docs p5-13).\n");
      break;
    case 0x01:
      usart1_puts("\nCard is initialising.\n");
      break;
    default:
      usart1_puts("\nUnknown error.\n");
      break;
  }
}
/*****************************************************************************/

int8_t sd_State()
{
  int16_t value;

  sd_Command(13, 0, 0);
  value=sd_Resp16b(1);

  switch(value)
  {
    case 0x000:
      return(1);
      //	break;
    case 0x0001:
      //			DBG((TXT("Card is Locked.\n")));
      break;
    case 0x0002:
      //			DBG((TXT("WP Erase Skip, Lock/Unlock Cmd Failed.\n")));
      break;
    case 0x0004:
      //			DBG((TXT("General / Unknown error -- card broken?.\n")));
      break;
    case 0x0008:
      //			DBG((TXT("Internal card controller error.\n")));
      break;
    case 0x0010:
      //			DBG((TXT("Card internal ECC was applied, but failed to correct the data.\n")));
      break;
    case 0x0020:
      //			DBG((TXT("Write protect violation.\n")));
      break;
    case 0x0040:
      //			DBG((TXT("An invalid selection, sectors for erase.\n")));
      break;
    case 0x0080:
      //			DBG((TXT("Out of Range, CSD_Overwrite.\n")));
      break;
    default:
      if(value>0x00FF)
        sd_Resp8bError((uint8_t) (value>>8));
      else
        //				DBG((TXT("Unknown error: 0x%x (see SanDisk docs p5-14).\n"),value));
        break;
  }
  return(-1);
}
/*****************************************************************************/

/* ****************************************************************************
 * WAIT ?? -- FIXME
 * CMDWRITE
 * WAIT
 * CARD RESP
 * WAIT
 * DATA BLOCK OUT
 *      START BLOCK
 *      DATA
 *      CHKS (2B)
 * BUSY...
 */

int8_t sd_writeSector(uint32_t address, uint8_t* buf)
{
  uint8_t cardresp;
  uint8_t firstblock;
  uint8_t c;
  uint32_t i,*retAdd,rt=0;
  uint32_t place;
  uint8_t retry=5;
  //	unsigned char buf[32];

  retAdd=&rt;

  /*DBG((TXT("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
  //place=512*address;
  if(SDHC_flag)
    place=address;
  else
    place=address*512;

  do
  {
    sd_Command(CMDWRITE, (uint16_t) (place >> 16), (uint16_t) place);
    cardresp=sd_Resp8b(1); /* Card response */ 
    retry--;
  }
  while(cardresp && retry);

  if(cardresp)
  {
    usart1_puts("\n\r cardresp error");
    return -1;
  }

  SD_SELECTION_CLR;

  firstblock=spiSendRcv(0xfe); 

  for(i=0;i<512;i++){
    c = spiSendRcv(buf[i]);
  }

  /* Checksum (2 byte) - ignore for now */
  spiSendRcv(0xff);
  spiSendRcv(0xff);
  spiSendRcv(0xff);
  c=0xFF;
  while((spiSendRcv(0xff)!=0xff)&&c--){
    // Removed NOP 
    for(i=0;i<0xFF;i++);
    //uart0_putc('v');
  }
  SD_SELECTION_SET;

  return 0;
}
/*****************************************************************************/

/* ****************************************************************************
 * WAIT ?? -- FIXME
 * CMDCMD
 * WAIT
 * CARD RESP
 * WAIT
 * DATA BLOCK IN
 * 		START BLOCK
 * 		DATA
 * 		CHKS (2B)
 */

int8_t sd_readSector(uint32_t address, uint16_t len)
{
  uint8_t cardresp;
  uint8_t firstblock;
  uint8_t c;
  uint16_t fb_timeout=0xffff;
  uint32_t i,*retAdd,rt=0;
  uint32_t place;
  uint8_t retry=5;
//  unsigned char buf[32];

  retAdd=&rt;

  /*DBG((TXT("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
  //place=512*address;
  if(SDHC_flag)
    place=address;
  else
    place=address*512;

  do
  {
    sd_Command(CMDREAD, (uint16_t) (place >> 16), (uint16_t) place);
    cardresp=sd_Resp8b(1); /* Card response */ 
    retry--;
  }
  while(cardresp && retry);

  if(cardresp)
  {
    usart1_puts("\n\r cardresp error");
    return -1;
  }


  //cardresp=spiSendRcv(0xff);

  /* Wait for startblock */
  //	do
  //		firstblock=sd_Resp8b(0); 
  //	while(firstblock==0xff && fb_timeout--);

  SD_SELECTION_CLR;

  do
    firstblock=spiSendRcv(0xff); 
  while(firstblock==0xff && fb_timeout--);

  //sprintf((char *)buf,"\n\r%02x %02x\n\r",cardresp,firstblock);
  //usart1_puts((char *)buf);


  if(cardresp!=0x00 || firstblock!=0xfe){
    sd_Resp8bError(firstblock);
    SD_SELECTION_SET;
    return -1;
  }

  for(i=0;i<512;i++){
    c = spiSendRcv(0xff);
    if(i<len)
      sd_buf[i] = c;
  }

  /* Checksum (2 byte) - ignore for now */
  spiSendRcv(0xff);
  spiSendRcv(0xff);
  spiSendRcv(0xff);
  SD_SELECTION_SET;

  return 0;
}
/*****************************************************************************/


/*****************************************************************/
unsigned long sd_getDriveSize(void )
{
  uint8_t resp[16],retry=0,i=0;
  uint16_t READ_BL_LEN,C_SIZE_MULT;
  uint32_t C_SIZE;
  signed char rp;
  char buf[16];

  while(i<5)
  {
    sd_Command(9,0,0);
    do
    {	
      resp[0]=spiSendRcv(0xff);
      retry++;
    }while(resp[0]==0xff && retry<0x10);

    for(retry=0;retry<16;retry++)
    {
      resp[0]=spiSendRcv(0xff);
      if(resp[0]==0xfe)
        break;
    }

    if(resp[0]==0xfe)
      break;
    SD_SELECTION_SET;
    i++;
  }

  if(i>=5)
    return 0;
  //	retry=0;
  //	sprintf((char *)buf," %02x",resp[0]);
  //	usart1_puts((char *)buf);
  for(retry=0;retry<16;retry++)
  {
    resp[retry]=spiSendRcv(0xff);
#ifdef UART_DBG
    sprintf((char *)buf," %02x",resp[retry]);
    usart1_puts((char *)buf);
#endif
  }

  SD_SELECTION_SET;

  if(!(SDHC_flag))
  {
    READ_BL_LEN=(1<<(resp[5]&0x0F));
    //sprintf((char *)buf,"\n\r %d",READ_BL_LEN);
    //usart1_puts((char *)buf);
    C_SIZE = resp[6] & 0x03; // bits 1..0
    C_SIZE <<= 10;
    C_SIZE += (uint16_t)resp[7]<<2;
    C_SIZE += resp[8]>>6;

    i=resp[9] & 0x03;
    i <<= 1;
    i += resp[10] >> 7;

    C_SIZE_MULT = 1;
    C_SIZE_MULT <<= (2+i);

    if(READ_BL_LEN>512)
      rp=sd_setReadLen(512);

    if(rp==-1)
    {
      usart1_puts("\n\rError in setting block len");
    }

    return (C_SIZE_MULT*C_SIZE*READ_BL_LEN);

    //sprintf((char *)buf,"\n\r %d",C_SIZE_MULT);
    //usart1_puts((char *)buf);
  }
  else
  {
    READ_BL_LEN=(1<<(resp[5]&0x0F));
    sprintf((char *)buf,"\n\r %d bytes",READ_BL_LEN);
    usart1_puts((char *)buf);

    C_SIZE=0;
    C_SIZE=resp[7]&0x3F;
    C_SIZE<<=16;
    C_SIZE+= (uint32_t)resp[8]<<8;
    C_SIZE+= (uint32_t)resp[9];

    sprintf((char *)buf,"\n\r %d bytes",C_SIZE);
    usart1_puts((char *)buf);

    return (C_SIZE*524288);
  }

}

/******************************************************************************/
signed char sd_setReadLen(unsigned int len){

  unsigned char retry=0x05,cardresp;

  do
  {
    sd_Command(16, (uint16_t)(len >> 16), (uint16_t) len);
    cardresp=sd_Resp8b(1); /* Card response */ 

    retry--;
  }
  while(cardresp && retry);

  if(retry)
    return 0;
  else
    return -1;
}

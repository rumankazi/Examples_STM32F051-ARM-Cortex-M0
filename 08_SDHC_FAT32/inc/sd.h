/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  sd.h                                                            *
* Revision :  Initial developement                                            *
* Description : Headerfile for sd.c                                           *
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

#ifndef __SD_H_ 
#define __SD_H_ 
#include <stdint.h>
#include "stm32f0xx_conf.h"

#define SD_CARD (1 << 11);
#define SD_SELECTION_SET GPIOB->BSRR = SD_CARD
#define SD_SELECTION_CLR GPIOB->BRR = SD_CARD

#define	CMDREAD		       17
#define	CMDWRITE	       24
#define	CMDREADCSD       9

uint8_t  sd_init(void);
uint8_t sd_is_sdhc(void ); 
void sd_Command(uint8_t cmd, uint16_t paramx, uint16_t paramy);
uint8_t sd_Resp8b(uint8_t );
void sd_Resp8bError(uint8_t value);
uint16_t sd_Resp16b(uint8_t );
int8_t sd_State(void);
signed char sd_setReadLen(unsigned int );

int8_t sd_readSector(uint32_t address, uint16_t len);
int8_t sd_writeSector(uint32_t address, uint8_t* buf);
unsigned long sd_getDriveSize(void );


#endif

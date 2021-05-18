#include "stm32f0xx_conf.h"

#include <stdio.h>
#include "FAT/FAT.h"
#include "FAT/FAT32.h"
#include "sd.h"
#include "usart.h"

/// FAT16 to be included

#define FAT16 0
#define FAT32 1

extern uint8_t sd_buf[512];
extern unsigned char SD_version, cardType, SDHC_flag;
uint8_t FATType;

void FAT_size_to_str(unsigned long int sz, char* buffer, uint32_t buffer_size)
{
  float fsize=sz;
  uint8_t sizeUnit = 0;

  while(fsize>1000)
  {
    fsize=fsize/1024;
    sizeUnit++;
  }

  
  if(sizeUnit==1)
  {
    snprintf(buffer, buffer_size, "%.2f KB", fsize);
  }
  else if(sizeUnit==2)
  {
    snprintf(buffer, buffer_size, "%.2f MB", fsize);
  }
  else if(sizeUnit==3)
  {
    snprintf(buffer, buffer_size, "%.2f GB", fsize);
  }
}


signed char FAT_get_BS_data(void)
{
  signed char resp;
  struct MBR_struct *mbr;
  uint32_t firstSec=0,uliTemp;
  char buf[16];

  resp=sd_readSector(0,SD_SECSIZE); 
  if(resp==-1)
    return resp;
  
  if(sd_buf[0]!=0xE9 && sd_buf[0]!=0xEB)
  {
    if(sd_buf[510]==0x55 && sd_buf[511]==0xAA)
    {
      mbr=(struct MBR_struct *)sd_buf;   
      firstSec=(mbr->partitionData[11]<<24)|(mbr->partitionData[10]<<16)|(mbr->partitionData[9]<<8)|(mbr->partitionData[8]);
      resp=sd_readSector(firstSec,SD_SECSIZE);
    }
    else
      return -1;				//NOT BS nor MBR
  }

  if(sd_buf[0x52+3]=='3')
  {
    FATType=FAT32;
    usart1_puts("\n\rVOLUME Label: ");
    for(resp=0;resp<11;resp++)
      usart1_putch(sd_buf[0x47+resp]);

    usart1_puts("\n\rFAT Type:");
    for(resp=0;resp<8;resp++)
      usart1_putch(sd_buf[0x52+resp]);

    F32_getParameters(sd_buf,firstSec);
  }
  else
  {
    FATType=FAT16;    
    usart1_puts("\n\rVOLUME Label: ");
    for(resp=0;resp<11;resp++)
      usart1_putch(sd_buf[0x2b+resp]);

    usart1_puts("\n\rFAT Type:");
    for(resp=0;resp<8;resp++)
      usart1_putch(sd_buf[0x36+resp]);
    //F16_getParameters(sd_buf);
  }

  return 0;
}

/*****************************************************************/
signed char FAT_get_dir_struct(unsigned long clusterN ,unsigned char *strBuf)
{
  if(FATType==FAT32)
    return F32_getDirStruct(clusterN,strBuf);
}

/*****************************************************************/
unsigned long FAT_get_first_sector(unsigned long cluster)
{
  if(FATType==FAT32)
    return F32_getFirstSector(cluster);
}

/*****************************************************************/
unsigned long FAT_get_cluster_number(unsigned long cluster,unsigned char *nam,unsigned char fileOrDir)
{
  if(FATType==FAT32)
    return F32_getClusterNumber(cluster,nam,fileOrDir);
}

/*****************************************************************/
unsigned long FAT_get_next_cluster(unsigned long ccluster)
{

  if(FATType==FAT32)
    return F32_getNextCluster(ccluster);
}
/*****************************************************************/
unsigned long int FAT_get_free_cluster()
{
  if(FATType==FAT32)
    return F32_getFreeCluster();
}

/*****************************************************************/
signed char FAT_read_file(char *filename,unsigned long curCluster,unsigned int offset,unsigned char *dataBuf)
{
  if(FATType==FAT32)
    return F32_readFile(filename,curCluster,offset,dataBuf);
}

/*****************************************************************/

signed char FAT_write_file(char *filename,unsigned long curCluster,unsigned long int fileSize,unsigned char *data,unsigned char flags)
{
  if(FATType==FAT32)
    return F32_writeFile(filename,curCluster,fileSize,data,flags);
}

void writeTest()
{
  unsigned long int secNum;

  secNum=F32_getFirstSector(8);
  sd_writeSector(secNum,sd_buf);
  //uart0_dump(sd_buf,512,32);
  usart1_puts("\n\r");
  sd_readSector(secNum,512);
  //uart0_dump(sd_buf,512,32);
}

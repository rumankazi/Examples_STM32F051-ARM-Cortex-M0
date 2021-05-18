#include "stm32f0xx_conf.h"

#include "FAT/FAT16.h"
#include "FAT/FAT.h"

extern uint8_t sd_buf[512];
uint16_t bytesPerSec;
uint8_t sectorsPerCluster;
uint32_t rootDirCluster;
uint32_t FATTableSec,firstDataSector,fsize;

/*
   struct dirTab
   {
   unsigned char ent[16][32];

   };
   */
void F16_getParameters(unsigned char *bf)
{
  struct F16_BS_struct *bs;

  bs=(struct F16_BS_struct *)bf;

  //bytesPerSec=(bs->bytesPerSec[0])|(bs->bytesPerSec[1]<<8);

  bytesPerSec=bs->bytesPerSec;
  sectorsPerCluster=bs->sectorsPerCluster;

  FATTableSec=(bs->reservedSec)+bs->numberOfHidSec;

  firstDataSector = bs->reservedSec + (bs->sectorPerFAT * 2)+((bs->maxRootDir * 32)/bytesPerSec);

  rootDirCluster= bs->reservedSec + (bs->sectorPerFAT * 2) ;

  //sprintf((char *)buf,"\n\r%d %d %d",bs->numberOfHidSec,bs->reservedSec,bs->mediaDesc);
  //uart0_puts((char *)buf);

  //		resp=sd_readSector(firstSec+bs->secNumberOfFileSys,SD_SECSIZE);
  //		uart0_dump((char *)sd_buf,512,16);
  //		uliTemp=(sd_buf[0x1e8])|(sd_buf[0x1e8+1]<<8)|(sd_buf[0x1e8+2]<<16)|(sd_buf[0x1e8+3]<<24);

  //		uart0_puts("\n\rFree Space:");
  //		displaySize(uliTemp*sectorsPerCluster*512);


}

/************************************************************/
signed char F16_getDirStruct(unsigned long cluster)
{
  struct dirTab *rootTab;
  signed char resp;
  char buf[32];
  uint32_t i,j,size,rootDirSec,clusterNum;

  rootDirSec=getFirstSector(cluster);
  //	rootDirSec=(rootDirSec-firstDataSector)+(firstDataSector/512);
  //sprintf((char *)buf,"  %d",rootDirSec);
  //uart0_puts((char *)buf);
  resp=sd_readSector(rootDirSec,SD_SECSIZE);
  //	uart0_dump(sd_buf,512,16);
  if(resp==-1)
    return -1;

  rootTab=(struct dirTab *)sd_buf;
  uart0_puts("\n\r");
  for(i=1;i<16;i++)
  {
    if(rootTab->ent[i][0]!=0xE5 && rootTab->ent[i][0]!=0 && rootTab->ent[i][0x0b]!=0x0F)
    {
      for(j=0;j<0x0B;j++)
      {
        sprintf((char *)buf+j,"%c",rootTab->ent[i][j]);


      } 
      uart0_puts((char *)buf);
      uart0_puts("		");
      size=(rootTab->ent[i][0x1f]<<24)|(rootTab->ent[i][0x1e]<<16)|(rootTab->ent[i][0x1d]<<8)|(rootTab->ent[i][0x1c]);
      if(size!=0)
      {
        displaySize(size);
      }
      else
      {
        uart0_puts("dir");

      }

      clusterNum=(rootTab->ent[i][0x1a])|(rootTab->ent[i][0x1a+1]<<8)|(rootTab->ent[i][0x14]<<16)|(rootTab->ent[i][0x14+1]<<24);
      sprintf((char *)buf,"   %d",clusterNum);
      uart0_puts((char *)buf);
      uart0_puts("\n\r");

    }
    /*else if(rootTab->ent[i][6]==0x7E && rootTab->ent[i][7]==0x31)
      {
      for(j=0;j<16;j++)
      {
      if(rootTab->ent[j][0]==01)
      break;					
      }

      for(temp=1;temp<0x0b;temp+=2)
      {
      buf[temp-1]=
      }

      }
      */
  }

}

/***************************************************************************************/
unsigned long F16_getFirstSector(unsigned long clusterNumber)
{
  return (((clusterNumber - 2) * sectorsPerCluster) + firstDataSector);
}

/***************************************************************************************/

unsigned long F16_getClusterNumber(unsigned long curClstr,unsigned char *dirName,unsigned char fileOrDir)
{

  struct dirTab *rootTab;
  signed char resp,elen;
  char buf[32],ext[3];
  char extName[3];
  uint32_t i,j,rootDirSec,clusterNum,slen,nlen;

  rootDirSec=getFirstSector(curClstr);
  //	rootDirSec=(rootDirSec-firstDataSector)+(firstDataSector/512);
  //sprintf((char *)buf,"  %d",rootDirSec);
  //uart0_puts((char *)buf);
  uart0_puts("\n\rin getClusterNumber\n\r");

  resp=sd_readSector(rootDirSec,512);
  uart0_puts("\n\rin gcn\n\r");
  //uart0_dump(sd_buf,512,16);
  if(resp==-1)
  {	    	
    return -1;
  }

  rootTab=(struct dirTab *)sd_buf;
  //uart0_puts("\n\rin fat32");
  uart0_puts("\n\r");
  slen=strlen(dirName);
  nlen=strcspn(dirName,".");
  elen=slen-(nlen+1);

  if(elen>=1){
    for(resp=0;resp<3;resp++)
      extName[resp]=dirName[nlen+1+resp];
  }

  //	sprintf((char *)buf,"%d %d %d %s",slen,nlen,elen,extName);
  //	uart0_puts((char *)buf);

  for(i=1;i<16;i++)
  {
    if(rootTab->ent[i][0]!=0xE5 && rootTab->ent[i][0]!=0 && rootTab->ent[i][0x0b]!=0x0F)
    {
      for(j=0;j<0x08;j++)
      {
        sprintf((char *)buf+j,"%c",rootTab->ent[i][j]);


      } 
      for(j=0x08;j<0x0B;j++)
        sprintf((char *)ext+j-0x08,"%c",rootTab->ent[i][j]);
      //uart0_puts((char *)buf);
      //uart0_puts("		");
      fsize=(rootTab->ent[i][0x1f]<<24)|(rootTab->ent[i][0x1e]<<16)|(rootTab->ent[i][0x1d]<<8)|(rootTab->ent[i][0x1c]);


      if(!(strncmp(buf,dirName,nlen)) && ((fsize==0 && fileOrDir) || (fsize!=0 && !fileOrDir)))
      {	
        if(fileOrDir || (fsize!=0 && elen>0 && !(strcmp(ext,extName,elen))))
        {



          clusterNum=(rootTab->ent[i][0x1a])|(rootTab->ent[i][0x1a+1]<<8)|(rootTab->ent[i][0x14]<<16)|(rootTab->ent[i][0x14+1]<<24);
          //sprintf((char *)buf,"   %d",clusterNum);
          //uart0_puts((char *)buf);
          //uart0_puts("\n\r");
          return clusterNum;
        }
      }

    }
    /*else if(rootTab->ent[i][6]==0x7E && rootTab->ent[i][7]==0x31)
      {
      for(j=0;j<16;j++)
      {
      if(rootTab->ent[j][0]==01)
      break;					
      }

      for(temp=1;temp<0x0b;temp+=2)
      {
      buf[temp-1]=
      }

      }
      */
  }

  return 0;

}

/********************************************************************/
unsigned long F16_getNextCluster(unsigned long cCluster)
{
  signed char resp;
  unsigned long uliTemp;
  resp=sd_readSector(FATTableSec+(cCluster/128),SD_SECSIZE);
  if(resp==-1)
    return 0;

  uliTemp=(cCluster%128)*4;
  return ((sd_buf[uliTemp])|(sd_buf[uliTemp+1]<<8)|(sd_buf[uliTemp+2]<<16)|(sd_buf[uliTemp+3]<<24)); 	

}

/************************************************************************/
signed char F16_readFile(char *filename,unsigned long curCluster)
{
  unsigned long cnum,snum,bcount=0;
  unsigned short i,j;
  unsigned char dobreak=0,buf[16];
  signed char resp;

  cnum=F32_getClusterNumber(curCluster,filename,FILE);
  if(cnum<2)
    return -1;
  /* cnum=F32_getNextCluster(cnum);
  // cnum=1704136;
  sprintf((char *)buf,"%08x",cnum);
  uart0_puts((char *)buf);
  snum=F32_getFirstSector(cnum);
  sprintf((char *)buf,"  %ld",snum);
  uart0_puts((char *)buf);
  */
  //uart0_dump(sd_buf,512,16);


  do{
    snum=F32_getFirstSector(cnum);
    if(resp==-1)
      return -1;

    for(i=0;i<sectorsPerCluster && !dobreak;i++,snum++){
      resp=sd_readSector(snum,SD_SECSIZE);
      j=0;
      while(j<512){
        uart0_putc(sd_buf[j]);
        j++;
        bcount++;
        if(bcount>=fsize)
        {
          dobreak=1;
          break;
        }
      }


    }
    cnum=F32_getNextCluster(cnum);
  }while(cnum!=0x0fffffff && !dobreak);
  return  0;
}

/************************************************************************
  signed char F16_fillBuf(char *buf,unsigned long snum,unsigned char h){
  unsigned long cnum,bcount=0;
  unsigned short i,j;
  signed char resp;


  resp=sd_readSector(snum,SD_SECSIZE);
  if(resp==-1)
  return -1;
  if(!h){
  for(i=0;i<512;i++)
  buf[i]=sd_buf[i];
  }
  else{	
  for(i=0;i<512;i++)
  buf[i+512]=sd_buf[i];
  }
  return 0;

  }

//#define INC_CNT(X) {cnt=(cnt+X)&0x03FF};
/************************************************************************/
unsigned char F16_ascii2hex(unsigned char highNib,unsigned char lowNib){
  if(highNib<0x3a)
    highNib=highNib-0x30;
  else
    highNib=highNib-0x37;

  if(lowNib<0x3a)
    lowNib=lowNib-0x30;
  else
    lowNib=lowNib-0x37;

  return (highNib<<4)|(lowNib&0x0F);	

}
/**************************************************************************
  signed char F16_loadFile(char *filename,unsigned long cluster){
  unsigned long addr,data,snum;
  unsigned char recordType,buf[1024],cbuf[16];
  signed char byteCount;
//unsigned short cnt=0;
signed char resp;
struct {
unsigned int val:10;
unsigned int halfFlag:1;
}cnt;

cnt.val=0;
cnt.halfFlag=0;


cluster=F32_getClusterNumber(cluster,filename,FILE);
sprintf((char *)cbuf,"\n\r%ld %ld\n\r",cluster,snum);
uart0_puts((char *)cbuf);

if(cluster<2)
return -1;

snum=F32_getFirstSector(cluster);
cluster=F32_getNextCluster(cluster);
if(cluster!=0x0fffffff)
{
uart0_puts("\n\rError:File too large\n\r");
return -1;
}

resp=F32_fillBuf(buf,snum,0);
snum++;
resp=F32_fillBuf(buf,snum,1);
snum++;



while(1){
if(buf[cnt.val]!=':')
{
uart0_puts("\n\rError:Not a valid Intel Hex Format\n\r");				
return -1;
}
cnt.val++;

byteCount=ascii2hex(buf[cnt.val++],buf[cnt.val++]);
//INC_CNT(2);
addr&=0xFFFF0000;
addr|=(ascii2hex(buf[cnt.val++],buf[cnt.val++])<<8)|(ascii2hex(buf[cnt.val++],buf[cnt.val++]));
//cnt+=4;
recordType=ascii2hex(buf[cnt.val++],buf[cnt.val++]);
//cnt+=2;
if(recordType==5)
{
cnt.val+=12;
continue;
}
else if(recordType==4)
{
addr=(ascii2hex(buf[cnt.val++],buf[cnt.val++])<<24)|(ascii2hex(buf[cnt.val++],buf[cnt.val++])<<16);
if(addr<0x40000000)
{	
uart0_puts("\n\rError:Program Address Space not proper\n\r");
return -1;
}
//cnt+=8;
continue;
}
else if(recordType==01)
{
uart0_puts("\n\rFile Loaded\n\r");
return 0;
}
else if(recordType==0)
{
  //incmpltflag=1
  while(byteCount>0){
    data=(ascii2hex(buf[cnt.val++],buf[cnt.val++])<<24)|(ascii2hex(buf[cnt.val++],buf[cnt.val++])<<16)|(ascii2hex(buf[cnt.val++],buf[cnt.val++])<<8)|(ascii2hex(buf[cnt.val++],buf[cnt.val++]));
    __asm{
      STR data,[addr]
    }
    byteCount-=4;
    cnt.val+=4;			  	
    addr+=4;
  }

}
if(cnt.val>=512 && !cnt.halfFlag)
{
  cnt.halfFlag=1;
  resp=F32_fillBuf(buf,snum,(unsigned char)cnt.halfFlag);
  snum++;
  uart0_puts(" 0");
}
if(cnt.val<512 && cnt.halfFlag)
{
  cnt.halfFlag=0;
  resp=F32_fillBuf(buf,snum,(unsigned char)cnt.halfFlag);
  snum++;
  uart0_puts(" 1");
}
//if(snum

}
}
*/

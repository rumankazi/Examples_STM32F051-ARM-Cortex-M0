#include <LPC214X.h>
//#include <string.h>
#include "stdio.h"
#include "type.h"
#include "FAT32.h"
#include "FAT.h"



extern U8 sd_buf[512];
extern unsigned char SDHC_flag;
extern unsigned long int g_clk;
U16 bytesPerSec;
U8 sectorsPerCluster;
U32 sectorsPerFAT;
U32 rootDirCluster;
U32 FATTableSec,firstDataSector,fsize;


struct dirTab
{
	unsigned char ent[16][32];
	/*unsigned char ent[1,32];
	unsigned char ent[2,32];
	unsigned char ent03[32];
	unsigned char ent04[32];
	unsigned char ent05[32];
	unsigned char ent06[32];
	unsigned char ent07[32];
	unsigned char ent08[32];
	unsigned char ent09[32];
	unsigned char ent10[32];
	unsigned char ent11[32];
	unsigned char ent12[32];
	unsigned char ent13[32];
	unsigned char ent14[32];
	unsigned char ent15[32];
	*/
};

signed char lstrncmp(unsigned char *str1,unsigned char *str2,unsigned char len)
{
 	unsigned char cnt;
	for(cnt=0;cnt<len;cnt++)
		if(str1[cnt]!=str2[cnt])
			return str1[cnt]-str2[cnt];
	
	return 0;
}

/*******************************************************************
void F32_getParameters(unsigned char *bf,unsigned long firstSec)
{
 	struct F32_BS_struct *bs;
	char buf[64];

	bs=(struct F32_BS_struct *)bf;

	bytesPerSec=(bs->bytesPerSec[0])|(bs->bytesPerSec[1]<<8);
	sectorsPerFAT=bs->numberOfSecPerFAT;

	sectorsPerCluster=bs->sectorsPerCluster;
		
   		FATTableSec=(bs->reservedSec)+bs->numberOfHidSec;
		
		firstDataSector = bs->reservedSec + bs->numberOfHidSec+ (bs->numberOfFAT * bs->numberOfSecPerFAT);
		
		if(!SDHC_flag)
		{
			firstDataSector+=firstSec;
			FATTableSec+=firstSec;
		}

		rootDirCluster=bs->clusterNo;

/*  		sprintf((char *)buf,"\n\r%d %d %d %4x",bs->numberOfHidSec,bs->reservedSec,bs->mediaDesc,rootDirCluster);
		uart0_puts((char *)buf);
	
		sprintf((char *)buf,"\n\r%d %d %d\n",bs->numberOfFAT,bs->numberOfSecPerFAT,firstDataSector);
		uart0_puts((char *)buf);
	
	  	sprintf((char *)buf,"\n\r%d %d %d\n",sectorsPerCluster,bytesPerSec,FATTableSec);
		uart0_puts((char *)buf);
	*0/
	  
//		resp=sd_readSector(firstSec+bs->secNumberOfFileSys,SD_SECSIZE);
//		uart0_dump((char *)sd_buf,512,16);
//		uliTemp=(sd_buf[0x1e8])|(sd_buf[0x1e8+1]<<8)|(sd_buf[0x1e8+2]<<16)|(sd_buf[0x1e8+3]<<24);

//		uart0_puts("\n\rFree Space:");
//		displaySize(uliTemp*sectorsPerCluster*512);


}	  */

void F32_getParameters(unsigned char *bf,unsigned long firstSec)
{
 	//struct F32_BS_struct *bs;
	char buf[64];

	//bs=(struct F32_BS_struct *)bf;

	bytesPerSec=(bf[0x0B])|(bf[0x0B+1]<<8);
	sectorsPerCluster=bf[0x0D];	
	sectorsPerFAT=(bf[0x24])|(bf[0x24+1]<<8)|(bf[0x24+2]<<16)|(bf[0x24+3]<<24);

		
   	FATTableSec=((bf[0x0E])|(bf[0x0E+1]<<8))+((bf[0x1c])|(bf[0x1c+1]<<8)|(bf[0x1c+2]<<16)|(bf[0x1c+3]<<24)); //reserved sector + hidden sector
		
	firstDataSector = FATTableSec + (bf[0x10]*sectorsPerFAT); //resSector+hiddenSector+(no of FAT*sectorsPerFAT)
		
	if(!SDHC_flag)
	{
		firstDataSector+=firstSec;
		FATTableSec+=firstSec;
	}

		rootDirCluster=(bf[0x2C])|(bf[0x2C+1]<<8)|(bf[0x2C+2]<<16)|(bf[0x2C+3]<<24);

/*  		sprintf((char *)buf,"\n\r%d %d %d %4x",bs->numberOfHidSec,bs->reservedSec,bs->mediaDesc,rootDirCluster);
		uart0_puts((char *)buf);
	
		sprintf((char *)buf,"\n\r%d %d %d\n",bs->numberOfFAT,bs->numberOfSecPerFAT,firstDataSector);
		uart0_puts((char *)buf);
	
	  	sprintf((char *)buf,"\n\r%d %d %d\n",sectorsPerCluster,bytesPerSec,FATTableSec);
		uart0_puts((char *)buf);
	*/
	  
//		resp=sd_readSector(firstSec+bs->secNumberOfFileSys,SD_SECSIZE);
//		uart0_dump((char *)sd_buf,512,16);
//		uliTemp=(sd_buf[0x1e8])|(sd_buf[0x1e8+1]<<8)|(sd_buf[0x1e8+2]<<16)|(sd_buf[0x1e8+3]<<24);

//		uart0_puts("\n\rFree Space:");
//		displaySize(uliTemp*sectorsPerCluster*512);


}

#define ENTRY_LEN 15
signed char F32_getDirStruct(unsigned long cluster,unsigned char *strBuf)
{
	struct dirTab *rootTab;
	signed char resp,numberOfEntries=0;
	char buf[32];
	U32 i,j,size,rootDirSec,clusterNum,uliTemp;
	
	rootDirSec=F32_getFirstSector(cluster);
	//do{
		resp=sd_readSector(rootDirSec,SD_SECSIZE);
		//uart0_dump(sd_buf,512,16);
		if(resp==-1)
			return -1;

		rootTab=(struct dirTab *)sd_buf;
		
		for(i=1;i<16;i++)
		{
			if(rootTab->ent[i][0]!=0xE5 && rootTab->ent[i][0]!=0 && rootTab->ent[i][0x0b]!=0x0F)
			{
				size=(rootTab->ent[i][0x1f]<<24)|(rootTab->ent[i][0x1e]<<16)|(rootTab->ent[i][0x1d]<<8)|(rootTab->ent[i][0x1c]);				
				for(j=0;j<0x08;j++)
				{
					//sprintf((char *)buf+j,"%c",rootTab->ent[i][j]);			
					if(rootTab->ent[i][j]==0x20)
						break;
					*(strBuf+j+(numberOfEntries*ENTRY_LEN))=rootTab->ent[i][j];
				} 

				if(size)  //if it as file,then only care about extension
				{
					*(strBuf+j+(numberOfEntries*ENTRY_LEN))='.';
					j++;
					for(uliTemp=8;uliTemp<11;uliTemp++,j++)
					{
						*(strBuf+j+(numberOfEntries*ENTRY_LEN))=rootTab->ent[i][uliTemp];				 	
					}
				}

				if(size) 
					*(strBuf+14+(numberOfEntries*ENTRY_LEN))='F';	
				else
					*(strBuf+14+(numberOfEntries*ENTRY_LEN))='D';
				numberOfEntries++;
			}					  
			else if(rootTab->ent[i][0]==0)
			{
			 	break;
			}

		}

	//	rootDirSec++;	
	//}while(rootTab->ent[15][0]!=0);
	return numberOfEntries;

}

/***************************************************************************************/
unsigned long F32_getFirstSector(unsigned long clusterNumber)
{

  	//if(SDHC_flag)
		return (((clusterNumber - 2) * sectorsPerCluster) + firstDataSector);
	//else
	//	return ((clusterNumber*sectorsPerCluster));
}


/***************************************************************************************/

unsigned long F32_getClusterNumber(unsigned long curClstr,unsigned char *dirName,unsigned char fileOrDir)
{
 	struct dirTab *rootTab;
	signed char resp,elen;
	unsigned char buf[16],ext[3],ucTemp;
	char extName[3];
	U32 i,j,rootDirSec,clusterNum,slen,nlen;
		
	rootDirSec=F32_getFirstSector(curClstr);
	do{
		resp=sd_readSector(rootDirSec,512);
		if(resp==-1)
		{	    	
			return -1;
		}
   
		rootTab=(struct dirTab *)sd_buf;
		for(slen=0;slen<9;slen++)
		{
			if(dirName[slen]==0)
				break;
		}
		for(nlen=0;nlen<12;nlen++)		
		{
			if((fileOrDir && dirName[nlen]==0) || (!fileOrDir && dirName[nlen]=='.'))
				break;
		}
		//nlen=strcspn(dirName,".");
		if(fileOrDir)
		{
			elen=0;
		}
		else
		{
			elen=slen-(nlen+1);
		}

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
					//sprintf((char *)buf+j,"%c",rootTab->ent[i][j]);			
					*(buf+j)=rootTab->ent[i][j];
				 
				for(j=0x08;j<0x0B;j++)
					//sprintf((char *)ext+j-0x08,"%c",rootTab->ent[i][j]);
					*(ext+(j-8))=rootTab->ent[i][j];
			//uart0_puts((char *)buf);
			//uart0_puts("		");
				fsize=(rootTab->ent[i][0x1f]<<24)|(rootTab->ent[i][0x1e]<<16)|(rootTab->ent[i][0x1d]<<8)|(rootTab->ent[i][0x1c]);
			
						
				if(!(lstrncmp(buf,dirName,nlen)) && ((fsize==0 && fileOrDir) || (fsize!=0 && !fileOrDir)))
				{	
					if(fileOrDir || (fsize!=0 && elen>0 && !(lstrncmp(ext,extName,elen))))
					{				
						clusterNum=(rootTab->ent[i][0x1a])|(rootTab->ent[i][0x1a+1]<<8)|(rootTab->ent[i][0x14]<<16)|(rootTab->ent[i][0x14+1]<<24);					
						return clusterNum;
					}
				}
			
			}
		}
		rootDirSec++;
	}while(rootTab->ent[15][0]!=0);

	return 0;

}

/********************************************************************/
unsigned long F32_getNextCluster(unsigned long cCluster)
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
unsigned long int F32_getFreeCluster()
{
	signed char resp;
	unsigned int sectorCnt;
	unsigned char entryCnt,buf[16];
	unsigned long uliTemp;

    for(sectorCnt=0;sectorCnt<sectorsPerFAT;sectorCnt++)
	{
		resp=sd_readSector(FATTableSec+sectorCnt,SD_SECSIZE);
		if(resp==-1)
			return 0;
		//uart0_dump(sd_buf,512,32);
		for(entryCnt=0;entryCnt<512;entryCnt+=4)
		{
			uliTemp=(sd_buf[entryCnt])|(sd_buf[entryCnt+1]<<8)|(sd_buf[entryCnt+2]<<16)|(sd_buf[entryCnt+3]<<24);
			if((!uliTemp) && (sectorCnt || entryCnt>2))
			{
				return ((entryCnt/4)+sectorCnt*128);
			}
		}
	}
	
	return 0;
		 	
}

/************************************************************************/
signed char F32_readFile(char *filename,unsigned long curCluster,unsigned int offset,unsigned char *dataBuf)
{
	unsigned long cnum,snum,bcount=0;
	signed char resp;
		
	  cnum=F32_getClusterNumber(curCluster,filename,FILE);

	  if(cnum<2)
	  	return -1;
	  
	  snum=F32_getFirstSector(cnum);
	  if(resp==-1)
	  		return -1;
	  
	  resp=sd_readSector(snum+offset,SD_SECSIZE);
	for(;bcount<512;bcount++)
		dataBuf[bcount]=sd_buf[bcount];		  					
	  return  0;
}

/************************************************************************
signed char F32_fillBuf(char *buf,unsigned long snum,unsigned char h){
 unsigned long cnum,bcount=0;
	unsigned short i,j;
	signed char resp,cbuf[8];
		
	  
	resp=sd_readSector(snum,SD_SECSIZE);
	if(resp==-1)
	{
		uart0_puts("\n\rError in fullbuf");
		return -1;

	}

   //	sprintf((char *)cbuf,"%ld\n\r",snum);
//			uart0_puts((char *)cbuf);
   if(h){
	for(i=0;i<512;i++){
		buf[i]=sd_buf[i];
		}
//		uart0_puts("0-511\n\r");
//		uart0_dump(&buf,512,16);
   }
   else{	
	for(i=0;i<512;i++){
		buf[i+512]=sd_buf[i];
		}
//		uart0_puts("512-1023\n\r");
//		uart0_dump(&buf[512],512,16);
	
   }
	return 0;

}	*/

//#define INC_CNT(X) {cnt=(cnt+X)&0x03FF};
/************************************************************************/
unsigned char ascii2hex(unsigned char highNib,unsigned char lowNib){
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


signed char F32_loadFile(char *filename,unsigned long cluster){
 	unsigned long addr,data,snum,lineCount=0,base_snum;
	unsigned char recordType,buf[1024],cbuf[16],temp1,temp2;
	signed char byteCount;
	//unsigned short cnt=0;
	signed char resp;
	struct {
	 unsigned int val:10;
	 unsigned int halfFlag:1;
	 }cnt;

	 cnt.val=0;
	 cnt.halfFlag=1;

	 
    cluster=F32_getClusterNumber(cluster,filename,FILE);
		
	uart0_puts((char *)cbuf);

	if(cluster<2)
		return -1;
	
	snum=F32_getFirstSector(cluster);
	base_snum=snum;
	/*cluster=F32_getNextCluster(cluster);
	if(cluster!=0x0fffffff)
	{
	 	uart0_puts("\n\rError:File too large\n\r");
	 	return -1;
	}
	/* //comment ended here
//	sprintf((char *)cbuf,"\n\r%ld %ld\n\r",cluster,snum);

	resp=F32_fillBuf(buf,snum,1);
	snum++;
	resp=F32_fillBuf(buf,snum,0);
	snum++;
	
	cnt.halfFlag=1;
//	uart0_dump(buf,16,16);

 	
	while(1){
    //	uart0_dump(&buf[cnt.val],16,16);
//		sprintf((char *)cbuf," %d ",cnt.val);
//		uart0_puts((char *)cbuf);

	  	if(buf[cnt.val]!=':')
		{
		   uart0_puts("\n\rError:Not a valid Intel Hex Format\n\r");				
		   sprintf((char *)cbuf,"%d\n\r",cnt.val);
		   uart0_puts((char *)cbuf);
		   uart0_dump(&buf[cnt.val],16,16);
		   return -1;
		}
		cnt.val++;

	
		byteCount=ascii2hex(buf[cnt.val],buf[(cnt.val+1)&0x3FF]);
		cnt.val+=2;
		//INC_CNT(2);
		addr&=0xFFFF0000;
		addr|=(ascii2hex(buf[cnt.val],buf[(cnt.val+1)&0x3FF])<<8)|(ascii2hex(buf[(cnt.val+2)&0x3FF],buf[(cnt.val+3)&0x3FF]));
		cnt.val+=4;
		recordType=ascii2hex(buf[cnt.val],buf[(cnt.val+1)&0x3FF]);
	

		cnt.val+=2;

		 //sprintf((char *)cbuf,"r=%02x %02x %02x",recordType,temp1,temp2);
		 //  uart0_puts((char *)cbuf);
		//cnt+=2;
		if(recordType==5)
		{
		 	cnt.val+=12;
			
			continue;
		}
		else if(recordType==4)
		{
			addr=(ascii2hex(buf[cnt.val],buf[(cnt.val+1)&0x3FF])<<24)|(ascii2hex(buf[(cnt.val+2)&0x3FF],buf[(cnt.val+3)&0x3FF])<<16);
			if(addr<0x40000000)
			{	
				uart0_puts("\n\rError:Program Address Space not proper\n\r");
				return -1;
			}
			cnt.val+=8;
			continue;
		}
		else if(recordType==1)
		{
		 	uart0_puts("\n\rFile Loaded");
			uart0_puts("\n\rExecuting from RAM...\n\r");
			MEMMAP=0x00000002;
/*	 		VICIntEnable=1;
			WDMOD=0x00000003;
			WDTC=0x000000FF;
			WDFEED=0xAA;
			WDFEED=0x55;
  /* //comment ended here
  			F_dummy();
  
			return 0;
		}
		else if(recordType==0)
		{
			//incmpltflag=1
			while(byteCount>0){
		 	 data=(ascii2hex(buf[cnt.val],buf[(cnt.val+1)&0x3FF]))|(ascii2hex(buf[(cnt.val+2)&0x3FF],buf[(cnt.val+3)&0x3FF])<<8)|(ascii2hex(buf[(cnt.val+4)&0x3FF],buf[(cnt.val+5)&0x3FF])<<16)|(ascii2hex(buf[(cnt.val+6)&0x3FF],buf[(cnt.val+7)&0x3FF])<<24);
			__asm{
				STR data,[addr]
				}
			  byteCount-=4;
			  cnt.val+=8;			  	
			  addr+=4;
			  //sprintf((char *)cbuf,"%d",byteCount);
			  //uart0_puts((char *)cbuf);
			}
			cnt.val+=4;
		//	uart0_puts("\n\r");
			lineCount++;

	  	}
		if((cnt.val<600 && cnt.val>512) && cnt.halfFlag)
		{
			resp=F32_fillBuf(buf,snum,(unsigned char)cnt.halfFlag);
			cnt.halfFlag=0;		
			snum++;
			if((snum-base_snum)==sectorsPerCluster)
			{
				cluster=F32_getNextCluster(cluster);
				snum=F32_getFirstSector(cluster);
				base_snum=snum;
			}
		
			//uart0_puts(" 0");
		}
		if((cnt.val>0 && cnt.val<512) && !cnt.halfFlag)
		{
			resp=F32_fillBuf(buf,snum,(unsigned char)cnt.halfFlag);
			cnt.halfFlag=1;		
			snum++;
			if((snum-base_snum)==sectorsPerCluster)
			{
				cluster=F32_getNextCluster(cluster);
				snum=F32_getFirstSector(cluster);
				base_snum=snum;
			}		
		//	uart0_puts(" 1");
		}
		//uart0_puts("i");
	 //	sprintf((char *)cbuf,"%d\n\r",lineCount);
	//	uart0_puts((char *)cbuf);
		 uart0_putc('.');
		//if(snum

	   }
}*/
/*************************************************************************/
signed char F32_writeFAT(unsigned long int clusterNum,unsigned long int data)
{
	signed char resp;
	unsigned int index;
 	resp=sd_readSector(FATTableSec+(clusterNum/128),SD_SECSIZE);
	if(resp==-1)
		return resp;
	index=(clusterNum%128)*4;
	sd_buf[index]=data;
	sd_buf[index+1]=(data>>8)&0x000000FF;
	sd_buf[index+2]=(data>>16)&0x000000FF;
	sd_buf[index+3]=(data>>24)&0x000000FF;
	resp=sd_writeSector(FATTableSec+(clusterNum/128),sd_buf);
	/*uart0_puts("\n\r");
	resp=sd_readSector(FATTableSec+(clusterNum/128),SD_SECSIZE);
	uart0_dump(sd_buf,512,32);
	*/
	if(resp==-1)
		return resp;
	return 0;	
}

/*************************************************************************/
#define APPEND 0
#define OVERWRITE 1
signed char F32_writeFile(char *filename,unsigned long curCluster,unsigned long int fileSize,unsigned char *data,unsigned char flags)
{
	  unsigned long int rootDir,clusterNumber,uliTemp,sectorNumber,orgFileSize;
	  unsigned char ucTemp,ucFlag1=0,ucFlag2=0,name[8],ext[3],fIndex=0,*pTemp,tempbuf[16],ucFb;
	  unsigned int ind;
	  unsigned short t;
	  signed char resp;
	  struct dirTab rootTab,*pDirTab;
		
	  clusterNumber=F32_getClusterNumber(curCluster,filename,FILE);   //Check if file exists
	  //uart0_puts(filename);
	  if(!clusterNumber || flags==OVERWRITE)		//File does not exist or overwrite set
	  {
	  		uart0_puts("O");		
	  		rootDir=F32_getFirstSector(curCluster);
	  		resp=sd_readSector(rootDir,SD_SECSIZE);

			if(resp==-1)
	  			return resp;

			pTemp=(unsigned char *)&rootTab.ent[0][0];
			for(uliTemp=0;uliTemp<512;uliTemp++)
				*(pTemp+uliTemp)=sd_buf[uliTemp];

	  		
		/*	uart0_puts("Dir entry");
			uart0_dump(pTemp,512,32);
			uart0_puts("\n\r");
		  */
	  		if(!clusterNumber)
			{
		  		for(ind=1;ind<16;ind++) //scans only 16 entries currently. Need to expand
		  		{
	  				if(rootTab.ent[ind][0]==0x05 || rootTab.ent[ind][0]==0 || rootTab.ent[ind][0]==0xE5)
					{
						ucFlag1=1;
						break;
					}
	  			}
	  			if(!ucFlag1)
	  				return -1;
				for(ucTemp=0;ucTemp<8;ucTemp++)
				{
					if(filename[fIndex]=='.')
					{
						ucFlag2=1;
						fIndex++;
					}
					if(!ucFlag2)
					{
						rootTab.ent[ind][ucTemp]=filename[fIndex];
						fIndex++;
					}
					else
					{
						rootTab.ent[ind][ucTemp]=0x20;
					}
				}

				for(ucTemp=8;ucTemp<8+3;ucTemp++,fIndex++)
				{
					rootTab.ent[ind][ucTemp]=filename[fIndex];
				}
				rootTab.ent[ind][0x0b]=0x20;
				rootTab.ent[ind][0x0c]=0x18; // 5d f3 a2 31 40 31 40 00 00 f7 a2 31 40 03 00 0d 00 00 00 
				rootTab.ent[ind][0x0d]=0;
				t=(HOUR<<11)|(MIN<<5)|(SEC);
				rootTab.ent[ind][0x0f]=(t&0xFF00)>>8;
				rootTab.ent[ind][0x0e]=t&0x00FF;

				rootTab.ent[ind][0x17]=(t&0xFF00)>>8;
				rootTab.ent[ind][0x16]=t&0x00FF;
				

				t=((YEAR-1980)<<9)|(MONTH<<5)|(DOM);
				rootTab.ent[ind][0x11]=(t&0xFF00)>>8;
				rootTab.ent[ind][0x10]=t&0x00FF;

				rootTab.ent[ind][0x13]=(t&0xFF00)>>8;
				rootTab.ent[ind][0x12]=t&0x00FF;

				rootTab.ent[ind][0x19]=(t&0xFF00)>>8;
				rootTab.ent[ind][0x18]=t&0x00FF;
	  /*
			  	rootTab.ent[ind][0x0b]=0x20;
				rootTab.ent[ind][0x0c]=0x18; // 5d f3 a2 31 40 31 40 00 00 f7 a2 31 40 03 00 0d 00 00 00 
				rootTab.ent[ind][0x0d]=0x5d;
				rootTab.ent[ind][0x0e]=0xF3;
				rootTab.ent[ind][0x0f]=0xA2;
				rootTab.ent[ind][0x10]=0x21;
				rootTab.ent[ind][0x11]=0x40;
				rootTab.ent[ind][0x12]=0x21;
				rootTab.ent[ind][0x13]=0x40;
				rootTab.ent[ind][0x16]=0xf3;
				rootTab.ent[ind][0x17]=0xa2;
				rootTab.ent[ind][0x18]=0x21;
				rootTab.ent[ind][0x19]=0x40;						
		*/
				clusterNumber=F32_getFreeCluster();

				rootTab.ent[ind][0x1a]=clusterNumber&0x000000FF;
				rootTab.ent[ind][0x1a+1]=(clusterNumber>>8)&0x000000FF;
				rootTab.ent[ind][0x14]=(clusterNumber>>16)&0x000000FF;
				rootTab.ent[ind][0x14+1]=(clusterNumber>>24)&0x000000FF;
			}
			else
			{
				for(ind=1;ind<16;ind++)
				{
				 	ucFlag1=rootTab.ent[ind][0];
					if(ucFlag1!=0xE5 && ucFlag1!=0x0F && ucFlag1!=0)
					{
				 		uliTemp=(rootTab.ent[ind][0x1a])|(rootTab.ent[ind][0x1a+1]<<8)|(rootTab.ent[ind][0x14]<<16)|(rootTab.ent[ind][0x14+1]<<24);
					//firstByte=pDirTab->ent[ucTemp][0];
						if(uliTemp==clusterNumber)
							break;
					}
				}
				ucFlag1=0;
			
			}
			t=(HOUR<<11)|(MIN<<5)|(SEC);

			rootTab.ent[ind][0x17]=(t&0xFF00)>>8;
			rootTab.ent[ind][0x16]=t&0x00FF;
				
			t=((YEAR-1980)<<9)|(MONTH<<5)|(DOM);
	
			rootTab.ent[ind][0x13]=(t&0xFF00)>>8;
			rootTab.ent[ind][0x12]=t&0x00FF;

			rootTab.ent[ind][0x1c]=fileSize&0x000000FF;
			rootTab.ent[ind][0x1c+1]=(fileSize>>8)&0x000000FF;
			rootTab.ent[ind][0x1c+2]=(fileSize>>16)&0x000000FF;
			rootTab.ent[ind][0x1c+3]=(fileSize>>24)&0x000000FF;
//			sd_buf=(unsigned char *)rootTab;
			resp=sd_writeSector(rootDir,pTemp);

			F32_writeFAT(clusterNumber,0x0fffffff);
			sectorNumber=F32_getFirstSector(clusterNumber);
		/*	uart0_puts("\n\rmod Dir entry\n");
			resp=sd_readSector(rootDir,512);
			uart0_dump(sd_buf,512,32);
			
			sprintf((char *)tempbuf,"S=%d\n",sectorNumber);
			uart0_puts(tempbuf);
		  */
			uliTemp=0;
			ucFlag1=0;
			ucTemp=0;
			while(ucTemp<64)
			{
				for(ind=0;ind<512;ind++)
				{
					if(uliTemp<fileSize)
					{
						sd_buf[ind]=data[uliTemp];
						uliTemp++;
					}
					else
					{
						sd_buf[ind]=0;
					}
				}
				sd_writeSector(sectorNumber+ucTemp,sd_buf);
				ucTemp++;
				if(uliTemp>=fileSize)
					break;					
			}
					
		}
		else	//if flags==Append
		{
			uart0_puts("A");
			rootDir=F32_getFirstSector(curCluster);
			ucTemp=1;
			for(ind=0;ind<sectorsPerCluster;ind++)
			{
				resp=sd_readSector(rootDir+ind,SD_SECSIZE);
				if(resp==-1)
					return resp;
				pDirTab=(struct dirTab *)sd_buf;

				uart0_puts("\nDir Entry");
				uart0_dump(sd_buf,512,32);
			  
				for(;ucTemp<16;ucTemp++)
				{
					ucFlag1=sd_buf[ucTemp*32];
					if(ucFlag1!=0xE5 && ucFlag1!=0x0F && ucFlag1!=0)
					{
				 		uliTemp=(pDirTab->ent[ucTemp][0x1a])|(pDirTab->ent[ucTemp][0x1a+1]<<8)|(pDirTab->ent[ucTemp][0x14]<<16)|(pDirTab->ent[ucTemp][0x14+1]<<24);
					//firstByte=pDirTab->ent[ucTemp][0];
						if(uliTemp==clusterNumber)
							goto foundMatch;
					}
				}
				ucTemp=0;
			}
			foundMatch:
			orgFileSize=(pDirTab->ent[ucTemp][0x1c])|(pDirTab->ent[ucTemp][0x1c+1]<<8)|(pDirTab->ent[ucTemp][0x1c+2]<<16)|(pDirTab->ent[ucTemp][0x1c+3]<<24);
			fileSize=orgFileSize+fileSize;

			pDirTab->ent[ucTemp][0x1c]=fileSize&0x000000FF;
			pDirTab->ent[ucTemp][0x1c+1]=(fileSize>>8)&0x000000FF;
			pDirTab->ent[ucTemp][0x1c+2]=(fileSize>>16)&0x000000FF;
			pDirTab->ent[ucTemp][0x1c+3]=(fileSize>>24)&0x000000FF;

			t=(HOUR<<11)|(MIN<<5)|(SEC);

			pDirTab->ent[ucTemp][0x17]=(t&0xFF00)>>8;
			pDirTab->ent[ucTemp][0x16]=t&0x00FF;
				
			t=((YEAR-1980)<<9)|(MONTH<<5)|(DOM);
	
			pDirTab->ent[ucTemp][0x13]=(t&0xFF00)>>8;
			pDirTab->ent[ucTemp][0x12]=t&0x00FF;


			resp=sd_writeSector(rootDir+ind,sd_buf);
			if(resp==-1)
				return resp;
			
			/*uart0_puts("\n\rmod Dir entry\n");
			resp=sd_readSector(rootDir+ind,512);
			uart0_dump(sd_buf,512,32);
			  */
			while(1)
			{
				uliTemp=F32_getNextCluster(clusterNumber);
				if(uliTemp>0x0ffffff8)
					break;
				if(uliTemp==0)
					return -1;

				clusterNumber=uliTemp;
			}

			if((orgFileSize/32768)<(fileSize/32768))   //if true,a new ccluster is needed
			{															//make changes in FAT Table
				 uliTemp=F32_getFreeCluster();
				 F32_writeFAT(clusterNumber,uliTemp);	//Enter pointer to next cluster in current cluster FAT entry
				 F32_writeFAT(uliTemp,0x0fffffff);	//Mark clusternum as lst cluster				 
			}
	  		
			fileSize=fileSize-orgFileSize;
			uliTemp=(orgFileSize&0x00007FFF);//(orgFileSize%32768)/512;   //Remove effect of multiple clusters,dn get offset in current cluster
			ucTemp=(uliTemp)>>9;

			ind=orgFileSize%512;
			uliTemp=0;

			sectorNumber=F32_getFirstSector(clusterNumber);			
			/*sprintf((char *)tempbuf,"\nS=%d\n",sectorNumber);
			uart0_puts(tempbuf);
			sprintf((char *)tempbuf,"O=%d\n",ucTemp);
			uart0_puts(tempbuf);					
			*/
			resp=sd_readSector(sectorNumber+(unsigned int)ucTemp,SD_SECSIZE);

			
			/*uart0_puts("\n\rorg data\n\r");
			uart0_dump(sd_buf,512,32);
			*/
		/*	sprintf((char *)tempbuf,"C=%d\n",clusterNumber);
			uart0_puts(tempbuf);					
		*/	
		do
		{													
			for(;ucTemp<sectorsPerCluster;ucTemp++)
			{
				for(;ind<512;ind++)
				{
					if(uliTemp<fileSize)
					{
						sd_buf[ind]=data[uliTemp];				
						uliTemp++;
					}
					else
					{
					 	sd_buf[ind]=0;
					}
				}
			
			/*	uart0_puts("\n\r mod data\n\r");
				uart0_dump(sd_buf,512,32);
			  */
				resp=sd_writeSector(sectorNumber+(unsigned int)ucTemp,sd_buf);			
				if(uliTemp>=fileSize)
					break;

				ind=0;
				ucTemp=0;
			}
			clusterNumber=F32_getNextCluster(clusterNumber);
			sectorNumber=F32_getFirstSector(clusterNumber);		
		}while(uliTemp<fileSize && clusterNumber && clusterNumber<0x0ffffff8);
	}
		//uart0_puts("\n\rFile Written\n\r");
		return 0;	  
}

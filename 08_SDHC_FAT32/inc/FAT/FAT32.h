#ifndef __FAT32_H
#define __FAT32_H

/*struct F32_BS_struct{
U8 jumpBoot[3];
U8 OEMName[8];
U8 bytesPerSec[2];
U8 sectorsPerCluster;
U16 reservedSec;
U8 numberOfFAT;
U8 na1[4];
U8 mediaDesc;
U16 na2;
U16 sectorPerTracks;
U16 numberOfHead;
U32 numberOfHidSec;
U32 numberOfSec;
U32 numberOfSecPerFAT;
U16 flags;
U16 FAT32ver;
U32 clusterNo;
U16 secNumberOfFileSys;
U16 secNumberOfBackup;
U8 rev[12];
U8 logicalDriveNumber;
U8 na3;
U8 extSig;
U8 serialNo[4];
U8 volumeLabel[11];
U8 FATName[8];
U8 code[420];
U8 sig[2];
};
*/



signed char F32_getDirStruct(unsigned long ,unsigned char* );
unsigned long F32_getFirstSector(unsigned long );
unsigned long F32_getClusterNumber(unsigned long, unsigned char * ,unsigned char);
unsigned long F32_getNextCluster(unsigned long );
signed char F32_readFile(char * ,unsigned long,unsigned int ,unsigned char* );
signed char F32_writeFile(char *filename,unsigned long curCluster,unsigned long int fileSize,unsigned char *data,unsigned char flags);
unsigned long int F32_getFreeCluster();
signed char F32_fillBuf(char *,unsigned long, unsigned char );
unsigned char ascii2hex(unsigned char, unsigned char );
void F32_getParameters(unsigned char *,unsigned long );

#endif
#ifndef __FAT16_H
#define __FAT16_H

#include <stdint.h>

struct F16_BS_struct{
uint8_t jumpBoot[3];
uint8_t OEMName[8];
uint16_t bytesPerSec;
uint8_t sectorsPerCluster;
uint16_t reservedSec;
uint8_t numberOfFAT;
uint16_t maxRootDir;
uint16_t na1;
uint8_t mediaDesc;
uint16_t sectorPerFAT;
uint16_t sectorPerTracks;
uint16_t numberOfHead;
uint32_t numberOfHidSec;
uint32_t numberOfSec;
uint16_t logicalDriveNumber;
uint8_t extendedSign;
uint16_t serialnumberofPartition;
uint8_t volumeLabel[11];
uint8_t FATName[8];
uint8_t code[448];
uint8_t marker[2];
};



signed char F16_getDirStruct(unsigned long );
unsigned long F16_getFirstSector(unsigned long );
unsigned long F16_getClusterNumber(unsigned long, unsigned char * ,unsigned char);
unsigned long F16_getNextCluster(unsigned long );
signed char F16_readFile(char * ,unsigned long);
signed char F16_loadFile(char *,unsigned long );
signed char F16_fillBuf(char *,unsigned long, unsigned char );
unsigned char F16_ascii2hex(unsigned char, unsigned char );
void F16_getParameters(unsigned char *);


#endif
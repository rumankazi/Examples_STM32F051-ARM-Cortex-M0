#ifndef __FAT
#define __FAT

#include <stdint.h>

void FAT_size_to_str(unsigned long int sz, char* buffer, uint32_t buffer_size);
signed char FAT_get_BS_data(void );
signed char FAT_get_dir_struct(unsigned long ,unsigned char *);
unsigned long FAT_get_first_sector(unsigned long );
unsigned long FAT_get_cluster_number(unsigned long ,unsigned char* ,unsigned char );
unsigned long FAT_get_next_cluster(unsigned long );
signed char FAT_read_file(char *,unsigned long ,unsigned int ,unsigned char *);
signed char FAT_load_file(char *,unsigned long );
signed char FAT_write_file(char *filename,unsigned long curCluster,unsigned long int fileSize,unsigned char *data,unsigned char flags);

struct partiton_struct{
unsigned char state;
unsigned char beg_H;
unsigned short beg_CS;
unsigned char partitionType;
unsigned char end_H;
unsigned char end_CS;
unsigned int firstSector;
unsigned int numberofSec;
};


struct MBR_struct{
unsigned char	nothing[446];		//ignore, placed here to fill the gap in the structure
unsigned char	partitionData[64];	//partition records (16x4)
//struct partiotn_struct part[4];
unsigned int	signature;		//0xaa55
};


#define SD_SECSIZE 512

#define DIR 1
#define FILE 0

#endif
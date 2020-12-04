
#ifndef __compressor_h__
#define __compressor_h__

#define MAX_OFFSET  400 //2176  /* range 1..2176 */
#define MAX_LEN    400 //65536  /* range 2..65536 */


int zx7_Compress(char *dest,char *src,int *len,int src_len);

unsigned int get_optimal(char *data,unsigned int data_len,unsigned int input_index);

void write_bit(int value);

#endif

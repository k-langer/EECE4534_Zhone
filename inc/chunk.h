#ifndef _CHUNK_H_
#define _CHUNK_H_

#define SAMPLE_SIZE                 (1024*5)

typedef struct {
  /* define a union to have different acess to same data in chunk */
  union {
    unsigned char       u08_buff[SAMPLE_SIZE];  /** Unsigned Data Chunk */
    unsigned short      u16_buff[SAMPLE_SIZE/2];
    unsigned int        u32_buff[SAMPLE_SIZE/4];
    signed char         s08_buff[SAMPLE_SIZE];  /** Signed Data Chunk */
    signed short        s16_buff[SAMPLE_SIZE/2];
    signed int          s32_buff[SAMPLE_SIZE/4];
  };
  int                 bytesMax;         /** total number bytes in chunk */ 
  int                 bytesUsed;          /** used bytes in chunk (fill level) */
  e_buff_status_t     e_status;     /** status */ 
  
} chunk_t;

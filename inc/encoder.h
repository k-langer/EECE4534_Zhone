#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "commonTypes.h"

#define QUALITY 8

typedef struct encoder {
    int nbBytes; 
    SpeexBits bits;     
    void* status; 
    float * input;   
    int outputSize;
} encoder_t; 

int encoder_init(encoder_t* pThis);
int encoder_encode(encoder_t* pThis, chunk_t* pAudioChunk, chunk_t* pDataChunk );

#endif

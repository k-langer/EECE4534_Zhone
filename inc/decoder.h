#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "commonTypes.h"

typedef struct decoder {
    int nbBytes; 
    SpeexBits bits;     
    void* status; 
    //char * input;   
    float * output;
    int outputSize;
} decoder_t; 

int decoder_init(decoder_t* pThis, int nbBytes);
int decoder_decode(decoder_t* pThis, chunk_t* pDataChunk, chunk_t* pAudioChunk);


#endif

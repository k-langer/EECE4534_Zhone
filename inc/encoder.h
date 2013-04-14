#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "commonTypes.h"
#include <speex/speex.h>

/*
	Chunk size must be a multiple of
	frame size and greater than frame size
	Controls the audio latency and the amount of
	interupts audio tx will signal to fill the chunk
*/ 
#define CHUNK_SIZE 1120
/*
	These two defines control
	the bitrate of the encoder
*/
#define FRAME_SIZE 160
#define QUALITY 8

typedef struct encoder {
    int nbBytes; 
    SpeexBits bits;     
    void* status; 
    short * input;   
    int outputSize;
} encoder_t; 
/* 
	Must be run before encoder_encode.
	If the decoder is also needed, the
	encoder_init is run before decoder_init
*/
int encoder_init(encoder_t* pThis);
/*
	Recives a pointer to a chunk filled with audio data
	with a size CHUNK_SIZE. encodes data and fills the empty 
	data chunk pointer sets the bytesUsed field in pDataChunk 
	to be the size of the  encoded chunk.
*/
int encoder_encode(encoder_t* pThis, chunk_t* pAudioChunk, chunk_t* pDataChunk );

#endif

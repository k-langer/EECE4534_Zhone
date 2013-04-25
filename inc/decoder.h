#ifndef _DECODER_H_
#define _DECODER_H_

#include "commonTypes.h"

#define QUIET_FLOOR 0
typedef char decoder_t;

/*
decoder_init must be run before decoder_decode.
decoder_init needs nbBytes size from the encoder_init
function.
*/
int decoder_init(decoder_t* pThis, int nbBytes);
/*
decoder_decode takes a data chunk of size pDataChunk->bytesUsed
the size should be a multiple of nbBytes. It fills the empty pointer
to pAudioChunk with decompressed audio data.
*/
int decoder_decode(decoder_t* pThis, chunk_t* pDataChunk, chunk_t* pAudioChunk);


#endif

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "commonTypes.h"

typedef char encoder_t;

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
to be the size of the encoded chunk.
*/
int encoder_encode(encoder_t* pThis, chunk_t* pAudioChunk, chunk_t* pDataChunk );

#endif

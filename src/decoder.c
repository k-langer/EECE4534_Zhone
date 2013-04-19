#include "chunk.h"
#include "commonTypes.h"
#include "decoder.h"
#include <stdlib.h>
#include <stdio.h>

/*The decoder accepts the location of a chunk,
decodes it, and returns the location of the
decoded chunk
*/

int decoder_init(decoder_t* state, int nbBytes)
{
    state = NULL;
    return PASS;
}

int decoder_decode(decoder_t* pThis, chunk_t* pDataChunk, chunk_t* pAudioChunk) {
    int i;
    int place = 0;
    int offset = 0;
    for (i=0; i < pDataChunk->bytesUsed; i++) {
        for ( place = 0; place < SAMPLE_DIV; place++) {
            pAudioChunk->u16_buff[place+offset] = ((unsigned short) pDataChunk->u08_buff[i]) << SHIFT;
        }
        offset+=SAMPLE_DIV;
    }
    return PASS;
}

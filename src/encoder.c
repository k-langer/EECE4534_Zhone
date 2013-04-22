#include "chunk.h"
#include "commonTypes.h"
#include "encoder.h"
#include <stdlib.h>
#include <stdio.h>

/*Encoder init configures the internal
state of the encoder. Takes a blank
state structure and configures it.
returns pass/fail.
*/
int encoder_init(encoder_t* state)
{
    printf("[INIT] encoder BIT: %d RATE: %d\n",SHIFT,SAMPLE_DIV);
    state = NULL;
    return PASS;
}

/*The encoder accepts the location of a chunk,
encodes it, and fills the location of the
encoded chunk. Returns pass or fail.
*/
int encoder_encode( encoder_t* pThis, chunk_t* pAudioChunk, chunk_t* pDataChunk) {
    int i;
    int count = 0;
    for (i=0; i < CHUNK_SIZE; i+=SAMPLE_DIV) {
        pDataChunk->s08_buff[count] = (signed char) (pAudioChunk->s16_buff[i] >> SHIFT);
        count++;
    }
    pDataChunk->bytesUsed = count;
    return PASS;
}

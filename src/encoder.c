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
   unsigned char quietSamples = 0; 
   unsigned char smallSample; 
   unsigned char excCh = 1<<(7); 
   unsigned char savedSample = QUIET_FLOOR; 
   for (i=0; i < CHUNK_SIZE; i+=SAMPLE_DIV) {
      smallSample = (char) (pAudioChunk->s16_buff[i] >> SHIFT);   
      if ( (smallSample != savedSample) || (quietSamples == excCh-1) ) {
         if (quietSamples > 1 ) {
            count-=quietSamples;
            pDataChunk->s08_buff[count++] = quietSamples | excCh; 
            pDataChunk->s08_buff[count++] = savedSample; 
         }
         quietSamples = 0;
      }
      if ( smallSample == savedSample ) {
         quietSamples++;
      }
      savedSample = smallSample; 
      pDataChunk->s08_buff[count++] = smallSample>>1;
    }
    pDataChunk->bytesUsed = count;  
    return PASS;
}

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
    unsigned char excCh = 1<<(8*sizeof(char)-1); 
    for (i=0; i < CHUNK_SIZE; i+=SAMPLE_DIV) {
      smallSample = (char) (pAudioChunk->s16_buff[i] >> SHIFT);   
      /*If the contigious silence ends or is larger than the sample 
        storage for samples, then encode the silence as a MSB of 1 and a length of silence
      */
      if ( (quietSamples!=0 && smallSample != QUIET_FLOOR) || quietSamples == excCh-1 ) {
         pDataChunk->s08_buff[count++] = quietSamples | excCh; 
         quietSamples = 0;  
      } 
      if ( smallSample == QUIET_FLOOR ) {
         quietSamples++; 
      } else {
         /*If it is not silence, encode it into the data chunk and shift over 1 more, as 
           the MSB is only used to indiciate a special sample and not a data sample */
         pDataChunk->s08_buff[count++] = smallSample>>1;
      } 
    }
    pDataChunk->bytesUsed = count;  
    return PASS;
}

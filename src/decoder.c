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
    printf("[INIT] decoder BIT: %d RATE: %d\n",SHIFT,SAMPLE_DIV);
    state = NULL;
    return PASS;
}

int decoder_decode(decoder_t* pThis, chunk_t* pDataChunk, chunk_t* pAudioChunk) {
   int i;
   int j; 
   int offset = 0;
   unsigned char excCh = 1<<(7); 
   unsigned char smpl;
   short vbrSmpl; 
   for (i = 0; i < pDataChunk->bytesUsed; i++) { 
      smpl = pDataChunk->s08_buff[i]; 
      if ( smpl & excCh ) {
         vbrSmpl = ((short) pDataChunk->s08_buff[++i]) << (SHIFT); 
         for (j = 0; j < SAMPLE_DIV*(smpl&(~(excCh))); j++) {
            pAudioChunk->s16_buff[offset++] = vbrSmpl;
         }
      } else {
         for ( j = 0; j < SAMPLE_DIV; j++ ) {
            pAudioChunk->s16_buff[offset++] = ((short) smpl) << (SHIFT+1);
         } 
      }
   }
   pAudioChunk->bytesUsed = offset*sizeof(short); 
   return PASS;
}

#include "chunk.h"
#include "commonTypes.h"
/*The encoder accepts the location of a chunk, 
  encodes it, and returns the location of the 
  encoded chunk 
*/

dataChunk_t* encoder(audioChunk_t* pThis) {
    //Encode data at a chunk granularity   
    dataChunk_t* chunk = (dataChunk_t*) pThis;
    return chunk;  
}

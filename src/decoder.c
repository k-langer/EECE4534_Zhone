#import "chunk.h"
/*The decoder accepts the location of a chunk,
decodes it, and returns the location of the
decoded chunk
*/

chunk_t* decoder(encodedChunk_t* pThis) {
    //Decode data at a chunk granularity
    chunk_t* chunk = (chunk_t*) pThis;
    return chunk;
}

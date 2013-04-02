#import "chunk.h"
/*The decoder accepts the location of a chunk,
decodes it, and returns the location of the
decoded chunk
*/

audioChunk_t* decoder(dataChunk_t* pThis) {
    //Decode data at a chunk granularity
    audioChunk_t* chunk = (audioChunk_t*) pThis;
    return chunk;
}

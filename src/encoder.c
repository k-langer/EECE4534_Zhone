/*The encoder accepts the location of a chunk, 
  encodes it, and returns the location of the 
  encoded chunk 
*/

encodedChunk_t* encoder(chunk_t* pThis) {
    //Encode data at a chunk granularity   
    encodedChunk_t* encodedChunk = (encodedChunk_t*) pThis;
    return encodedChunk;  
}

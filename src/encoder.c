#include "chunk.h"
#include "commonTypes.h"
#include "encoder.h"

/*Encoder init configures the internal
  state of the encoder. Takes a blank
  state structure and configures it. 
  returns pass/fail.
*/
int encoder_init(encoder_t* state)
{
    state->status = speex_encoder_init(&speex_nb_mode);
    int quality = QUALITY;
    speex_encoder_ctl(state->status, SPEEX_SET_QUALITY , &quality); 

    state->input = malloc(FRAME_SIZE*sizeof(float) );   
    
    speex_bits_init(&(state->bits));
    
    state->status = speex_encoder_init(&speex_nb_mode);   
    speex_encoder_ctl((void*)state->status, SPEEX_SET_QUALITY , &quality);
    //TODO replace dummyOut with a look-up table   
    char * dummyOut = malloc(FRAME_SIZE);
    speex_encode((void*)state->status, state->input, &(state->bits));
    state->nbBytes = speex_bits_write(&(state->bits), dummyOut, FRAME_SIZE);
    free(dummyOut);
    state->outputSize = CHUNK_SIZE/FRAME_SIZE * state->nbBytes;
    return PASS;  
} 

/*The encoder accepts the location of a chunk, 
  encodes it, and fills the location of the 
  encoded chunk. Returns pass or fail. 
*/
int encoder_encode( encoder_t* pThis, chunk_t* pAudioChunk, chunk_t* pDataChunk)
{
    int i;
    int j;
    char* out = (char*)pDataChunk->u16_buff;
    for (i = 0; i < CHUNK_SIZE/FRAME_SIZE; i++) {
        for (j = 0; j < FRAME_SIZE; j++) {
            pThis->input[j] = (float)pAudioChunk->s16_buff[j+i*pThis->nbBytes];
        }   
        speex_bits_reset(&(pThis->bits));
        speex_encode(pThis->status, pThis->input, &(pThis->bits));
        speex_bits_write(&(pThis->bits), out + i*pThis->nbBytes , pThis->nbBytes);
    }
    pDataChunk->bytesUsed = pThis->nbBytes*CHUNK_SIZE/FRAME_SIZE;
    return PASS;
}

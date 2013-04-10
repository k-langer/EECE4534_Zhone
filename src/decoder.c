#include "chunk.h"
#include "commonTypes.h"
#include "decoder.h"

/*The decoder accepts the location of a chunk,
decodes it, and returns the location of the
decoded chunk
*/

int decoder_init(decoder_t* state, int nbBytes)
{
    state->status = speex_decoder_init(&speex_nb_mode);
    int enh=1;
    speex_decoder_ctl((void*)state->status, SPEEX_SET_ENH, &enh);
    speex_bits_init(&(state->bits));
    state->nbBytes = nbBytes;
    state->output = malloc(FRAME_SIZE*sizeof(float));
    return PASS;
}

int decoder_decode(decoder_t* pThis, chunk_t* pDataChunk, chunk_t* pAudioChunk)
{
    /*
    int i;
    int nbBytes = pThis->nbBytes;
    for (i = 0; i < CHUNK_SIZE/FRAME_SIZE; i++) {
        speex_bits_read_from(&(pThis->bits),(unsigned char*)pDataChunk->u16_buff[i*nbBytes],nbBy$
        speex_decode(pThis->status,&(pThis->bits),pThis->output);  
    }
    for (i=0;i<FRAME_SIZE;i++) {
        pAudioChunk->u16_buff[i] = pThis->output[i];
    }
    */
    return PASS;           
}

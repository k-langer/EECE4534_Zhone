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
    int i;
    int nbBytes = pThis->nbBytes;
    int bytePos = 0;
    //TODO figure out why this is needed! It is init in decoder_init but segfaults if not init here. 
    speex_bits_init(&pThis->bits);
    while (bytePos < pDataChunk->bytesUsed) {
        speex_bits_read_from(&pThis->bits,(char*)pDataChunk->u08_buff+bytePos,nbBytes); 
        speex_decode(pThis->status,&pThis->bits,pThis->output);  
        for (i=0;i<FRAME_SIZE;i++) {
            pAudioChunk->u16_buff[i] = pThis->output[i];
        }
        bytePos+=nbBytes;
    }
    return PASS;              
}

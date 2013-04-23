/*
 * receive_path.c
 *
 *  Created on: Apr 9, 2013
 *      Author: pal
 */
#include "receive_path.h"

return_value_t receive_path_init( receive_path_t* pThis, bufferPool_t* pBufferPool, isrDisp_t* pIsrDisp, wc_t* pCommunicator ) {
	receive_path_t receive_path = *pThis;
	receive_path.pBufferPool = pBufferPool;
	receive_path.pIsrDisp = pIsrDisp;
	receive_path.pCommunicator = pCommunicator;

	audioTx_init( &receive_path.audio_tx, receive_path.pBufferPool, receive_path.pIsrDisp );
	decoder_init( &receive_path.decoder, 10 );

	return PASS;
}

return_value_t receive_path_start( receive_path_t* pThis ) {
	receive_path_t receive_path = *pThis;

	audioTx_start( &receive_path.audio_tx );

	return PASS;
}

return_value_t receive_path_stop( receive_path_t* pThis ) {

	return PASS;
}

return_value_t receive_path_process_chunk( receive_path_t* pThis ) {
	receive_path_t receive_path = *pThis;

	chunk_t* pDataChunk;
	bufferPool_acquire( receive_path.pBufferPool, &pDataChunk );
	if ( Wc_Receive(receive_path.pCommunicator, pDataChunk ) ) {
		return PASS;
	} else {
		chunk_t* pAudioChunk;
		bufferPool_acquire( receive_path.pBufferPool, &pAudioChunk );
		decoder_decode( &receive_path.decoder, pDataChunk, pAudioChunk );
		bufferPool_release( receive_path.pBufferPool, pDataChunk );
		audioTx_put( &receive_path.audio_tx, pAudioChunk );
		bufferPool_release( pAudioChunk );
	}

	return PASS;
}

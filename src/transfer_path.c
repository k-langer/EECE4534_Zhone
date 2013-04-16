/*
 * transfer_path.c
 *
 *  Created on: Apr 9, 2013
 *      Author: pal
 */
#include "transfer_path.h"

return_value_t transfer_path_init( transfer_path_t* pThis, bufferPool_t* pBufferPool, isrDisp_t* pIsrDisp, wc_t* pCommunicator ) {
	transfer_path_t transfer_path = *pThis;
	transfer_path.pBufferPool = pBufferPool;
	transfer_path.pIsrDisp = pIsrDisp;
	transfer_path.pCommunicator = pCommunicator;

	audioRx_init( &transfer_path.audio_rx, transfer_path.pBufferPool, transfer_path.pIsrDisp );
	encoder_init( &transfer_path.encoder );

	return PASS;
}

return_value_t transfer_path_start( transfer_path_t* pThis ) {
	transfer_path_t transfer_path = *pThis;

	audioRx_start( &transfer_path.audio_rx );

	return PASS;
}

return_value_t transfer_path_stop( transfer_path_t* pThis ) {

	return PASS;
}

return_value_t transfer_path_process_chunk( transfer_path_t* pThis ) {
	transfer_path_t transfer_path = *pThis;

	chunk_t* pAudioChunk;
	bufferPool_acquire( transfer_path.pBufferPool, &pAudioChunk );

	if ( audioRx_getNbNc( &transfer_path.audio_rx, &pAudioChunk ) == FAIL ) {
		return PASS;
	} else {
		chunk_t* pDataChunk;
		bufferPool_acquire( transfer_path.pBufferPool, &pDataChunk );
		encoder_encode( &transfer_path.encoder, pAudioChunk, pDataChunk );
		bufferPool_release( transfer_path.pBufferPool, pAudioChunk );
		Wc_Send( transfer_path.pCommunicator, pDataChunk );
	}

	return PASS;
}

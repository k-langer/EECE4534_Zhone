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

	audioRx_init( transfer_path.audio_rx, transfer_path.pBufferPool, transfer_path.pIsrDisp );
	//initialize encoder

	return PASS;
}

return_value_t transfer_path_start( transfer_path_t* pThis ) {
	transfer_path_t transfer_path = *pThis;

	audioRx_start( transfer_path.audio_rx );

	return PASS;
}

return_value_t transfer_path_stop( transfer_path_t* pThis ) {
	transfer_path_t transfer_path = *pThis;

	audioRx_stop( transfer_path.audio_rx );

	return PASS;
}

return_value_t transfer_path_process_chunk( transfer_path_t* pThis ) {
	transfer_path_t transfer_path = *pThis;

	chunk_t* audioChunk;
	bufferPool_acquire( transfer_path.pBufferPool, &audioChunk );

	if ( audioRx_getNbNc( transfer_path.audio_rx, &audioChunk ) == FAIL ) {
		return NO_DATA_AVAILABLE;
	} else {
		chunk_t* dataChunk;
		bufferPool_acquire( transfer_path.pBufferPool, &dataChunk );
		//encode chunk first, then push communicator
		bufferPool_release( transfer_path.pBufferPool, &audioChunk );
		Wc_Send( transfer_path.pCommunicator, &dataChunk );
	}

	return PASS;
}

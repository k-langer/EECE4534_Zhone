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

	audioTx_init( receive_path.audio_tx, receive_path.pBufferPool, receive_path.pIsrDisp );
	//initialize decoder

	return PASS;
}

return_value_t receive_path_start( receive_path_t* pThis ) {
	receive_path_t receive_path = *pThis;

	audioTx_start( &receive_path.audio_tx );

	return PASS;
}

return_value_t receive_path_stop( receive_path_t* pThis ) {
	receive_path_t receive_path = *pThis;

	audioTx_stop( &receive_path.audio_tx ); 	//TODO: needs to be implemented

	return PASS;
}

return_value_t receive_path_process_chunk( receive_path_t* pThis ) {
	receive_path_t receive_path = *pThis;

	chunk_t* dataChunk;
	bufferPool_acquire( receive_path.pBufferPool, &dataChunk );
	if ( Wc_Receive(receive_path.pCommunicator, dataChunk ) ) {
		return NO_DATA_AVAILABLE;
	} else {
		chunk_t* audioChunk;
		bufferPool_acquire( receive_path.pBufferPool, &audioChunk );
		//decode chunk first then push to audio tx
		bufferPool_release( receive_path.pBufferPool, &dataChunk );
		audioTx_put( &receive_path.audio_tx, audioChunk);
	}

	return PASS;
}

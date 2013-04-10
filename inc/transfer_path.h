#ifndef TRANSFER_PATH_H
#define TRANSFER_PATH_H

#include "commonTypes.h"
#include "queue.h"
#include "bufferPool.h"
#include "audioRx.h"
#include "isrDisp.h"
#include "uartTx.h"

/***
    Transfer Path
        -contains audio_input, encoder, and xbee_tx
        -owns encoder_input queue & xbee_tx_input queue
        -handles moving of data from one queue to another
***/

typedef struct {
    audioRx_t audio_rx;
    encoder_t encoder;
    uartTx_t* pUartTx;

    isrDisp_t* pIsrDisp;
    bufferPool_t* pBufferPool;
} transfer_path_t;

/***
    init
        -intializes all the components of the transfer path

        pThis - pointer to transfer path object to initialize        

        returns PASS on success or FAIL on failure
***/
return_value_t transfer_path_init( transfer_path_t* pThis, bufferPool_t* pBufferPool, isrDisp_t* pIsrDisp, uartTx_t* pUartTx );

/***
 * start
 *
 */
return_value_t transfer_path_start( transfer_path_t* pThis );

/***
    process_chunk
        -if a chunk is available on encoder_input_queue, 
            encodes the chunk and passes it to xbee_tx_input_queue
        -if no chunk is availabe, exits

        pThis - pointer to transfer path object to run

        returns PASS if chunk was processed, NO_DATA_AVAILABLE if no data was available, or FAIL on failure
***/
return_value_t transfer_path_process_chunk( transfer_path_t* pThis );

/***
    stop
        -clears queues and sets all componenets to their initial states

        pThis - pointer to transfer path to stop

        returns PASS on success or FAIL on failure
***/
return_value_t transfer_path_stop( transfer_path_t* pThis );

#endif

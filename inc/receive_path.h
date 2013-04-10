#ifndef RECEIVE_PATH_H
#define RECEIVE_PATH_H

#include "commonTypes.h"
#include "queue.h"
#include "bufferPool.h"
#include "audioTx.h"
#include "isrDisp.h"
#include "uart_Rx.h"

/***
    Receive Path
        -contains xbee_rx, decoder, and audio_output modules
        -owns decoder_input queue and audio_output queue
        -handles moving data between queue for processing by relevant modules
***/

typedef struct {
    uartRx_t* pUartRx;
    decoder_t decoder;
    audioTx_t audio_tx;

    isrDisp_t* pIsrDisp;
    bufferPool_t* pBufferPool;
} receive_path_t;

/***
    init
        -intializes all the components of the receive path

        pThis - pointer to receive path object to initialize        

        returns PASS on success or FAIL on failure
***/
return_value_t receive_path_init( receive_path_t* pThis, bufferPool_t* pBufferPool, isrDisp_t* pIsrDisp, uartRx_t* pUartRx );

/**
 * start
 */
return_value_t receive_path_start( receive_path_t* pThis );

/***
    process_chunk
        -if a chunk is available on decoder_input_queue, 
            decodes the chunk and passes it to audio_output_queue
        -if no chunk is availabe, exits

        pThis - pointer to receive path object to run

        returns PASS if chunk was processed, NO_DATA_AVAILABLE if no data was available, or FAIL on failure
***/
return_value_t receive_path_process_chunk( receive_path_t* pThis );

/***
    stop
        -clears queues and sets all componenets to their initial states

        pThis - pointer to receive path to stop

        returns PASS on success or FAIL on failure
***/
return_value_t receive_path_stop( receive_path_t* pThis ); 

#endif

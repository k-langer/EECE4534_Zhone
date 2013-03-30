#ifndef TRANSFER_PATH_H
#define TRANSFER_PATH_H

/***
    Transfer Path
        -contains audio_input, encoder, and xbee_tx
        -owns encoder_input queue & xbee_tx_input queue
        -handles moving of data from one queue to another
***/

typedef struct {
    audio_input_t audio_input;
    encoder_t encoder;
    xbee_tx_t xbee_tx;

    queue_t encoder_input_queue;
    queue_t xbee_tx_input_queue;
} transfer_path_t;

/***
    init
        -intializes all the components of the transfer path

        pThis - pointer to transfer path object to initialize        

        returns 0 on success or -1 on failure
***/
int transfer_path_init( transfer_path_t* pThis );

/***
    run
        -if a chunk is available on encoder_input_queue, 
            encodes the chunk and passes it to xbee_tx_input_queue
        -if no chunk is availabe, exits

        pThis - pointer to transfer path object to run

        returns 0 if chunk was processed, 1 if no data was available, or -1 on failure
***/
int transfer_path_run( transfer_path_t* pThis );

/***
    stop
        -clears queues and sets all componenets to their initial states

        pThis - pointer to transfer path to stop

        returns 0 on success or -1 on failure
***/
int transfer_path_stop( transfer_path_t* pThis );

#endif

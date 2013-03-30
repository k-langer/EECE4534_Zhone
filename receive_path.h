#ifndef RECEIVE_PATH_H
#define RECEIVE_PATH_H

/***
    Receive Path
        -contains xbee_rx, decoder, and audio_output modules
        -owns decoder_input queue and audio_output queue
        -handles moving data between queue for processing by relevant modules
***/

typedef struct {
    xbee_rx_t xbee_rx;
    decoder_t decoder;
    audio_output_t audio_output;

    queue_t decoder_input_queue;
    queue_t audio_output_queue;
} receive_path_t;

/***
    init
        -intializes all the components of the receive path

        pThis - pointer to receive path object to initialize        

        returns 0 on success or -1 on failure
***/
int receive_path_init( receive_path_t* pThis );

/***
    run
        -if a chunk is available on encoder_input_queue, 
            encodes the chunk and passes it to xbee_tx_input_queue
        -if no chunk is availabe, exits

        pThis - pointer to receive path object to run

        returns 0 if chunk was processed, 1 if no data was available, or -1 on failure
***/
int receive_path_run( receive_path_t* pThis );

/***
    stop
        -clears queues and sets all componenets to their initial states

        pThis - pointer to receive path to stop

        returns 0 on success or -1 on failure
***/
int receive_path_stop( receive_path_t* pThis ); 

#endif

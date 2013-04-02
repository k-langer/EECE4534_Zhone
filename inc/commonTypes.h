#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include "chunk.h"

/***
    Common Types
        -header file containing any typedefs we decide to create for clearer code
***/

/***
    return_value_t
        -enum for return values of functions
        -add new return values here if necessary!
***/

typedef enum {
    PASS,
    FAIL,
    NO_DATA_AVAILABLE
} return_value_t;


/***
    zhone_address_t
        -integer value of a zhone address
***/
typedef unsigned short zhone_address_t;

/***
	a generic status enum that contains all of the possible statuses for both the UI
	and the xbeeTX module

	USE IT!
***/

typedef enum {
    IDLE,
    IN_CALL,
    RECEIVING,
    DIALING,
    CALL1,
    CALL2,
    END_CALL,
    ACCEPT_CALL,
    REJECT_CALL
} phone_status_t;

/***
	Chunk from audio transmit and receive
***/
typedef chunk_t audioChunk_t; 
/***
	Chunk for Xigbee_tx and Xigbee_rx
***/
typedef chunk_t dataChunk_t;

#endif

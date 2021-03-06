#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include "tll_common.h"
#define SHIFT 8
#define SAMPLE_DIV 4
#define CHUNK_SIZE 1280
/***
    Common Types
        -header file containing any typedefs we decide to create for clearer code
***/

/***
    return_value_t
        -enum for return values of functions
        -add new return values here if necessary!
***/

typedef int return_value_t;

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
    DIALING,     //calling someone is impliying that you are dialing them
    CALL1,        
    CALL2,
    END_CALL, 
    ACCEPT_CALL,
    REJECT_CALL
} phone_status_t;

#endif

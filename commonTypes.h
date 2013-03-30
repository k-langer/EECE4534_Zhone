#ifndef COMMONTYPES_H
#define COMMONTYPES_H

/***
    Common Types
        -header file containing any typedefs we decide to create for clearer code
***/

/***
    zhone_return_value_t
        -enum for return values of functions
        -add new return values here if necessary!
***/

typedef enum {
    PASS,
    FAIL,
    NO_DATA_AVAILABLE
} zhone_return_value_t;


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
} zhone_status_t;

#endif

#ifndef COMMONTYPES_H
#define COMMONTYPES_H

/***
    Common Types
        -header file containing any typedefs we decide to create for clearer code
***/

/***
    address_t
        -integer value of a zhone address
***/
typedef unsigned int address_t;

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
} status_t;

#endif

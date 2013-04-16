#ifndef UI_H
#define UI_H

#include "commonTypes.h"
#include "isrDisp.h"

/***
    UI
        -track status
        -allow getting and setting of status
        -update status and LED output through ISRs

	To Do:
	- After a call ends, status needs to be set back to idle, this is not detected by an ISR
	- After a user hangs up, the status needs to be set back to idle (it is currently set to END_CALL)
***/

typedef struct {

	phone_status_t status;
} ui_t;

/***
    init
        -initialize status to IDLE
        -register ISRs

        *pThis - pointer to ui object to initialize

        return PASS for success or FAIL for failure
***/
//static isrDisp_t isrDisp; //this will be created in Zhone.c

void ui_init( ui_t* pThis, isrDisp_t* isrDisp );
//void ui_init(void);

/***
    get_status

        pThis - pointer to ui object to grab status from

        returns status of *pThis
***/
phone_status_t ui_get_status(ui_t* pThis);

/***
    set_status

        pThis - pointer to ui object whose status will be set
        newStatus - status that will be set on *pThis

        returns PASS for success or FAIL for failure
***/
void ui_set_status(ui_t* pThis, phone_status_t newStatus);

/***
    button1_ISR & button2_ISR
        -check status to determine state of system
        -update status
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/
void ui_button1_ISR(void* pThis);
void ui_button2_ISR(void* pThis);



#endif

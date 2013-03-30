#ifndef UI_H
#define UI_H

#include "commonTypes.h"

/***
    UI
        -track status
        -allow getting and setting of status
        -update status and LED output through ISRs
***/

typedef struct {
    status_t status;
} ui_t;

/***
    init
        -initialize status to IDLE
        -register ISRs
        
        *pThis - pointer to ui object to initialize
        
        return PASS for success or FAIL for failure
***/
return_value_t ui_init(ui_t* pThis);

/***
    get_status
    
        pThis - pointer to ui object to grab status from
    
        returns status of *pThis
***/
status_t ui_get_status(ui_t* pThis);

/*** 
    set_status
    
        pThis - pointer to ui object whose status will be set 
        newStatus - status that will be set on *pThis

        returns PASS for success or FAIL for failure
***/
return_value_t ui_set_status(ui_t* pThis, status_t newStatus);

/***
    button1_ISR & button2_ISR
        -check status to determine state of system
        -update status        
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/
void ui_button1_ISR(ui_t* pThis);
void ui_button2_ISR(ui_t* pThis);

#endif

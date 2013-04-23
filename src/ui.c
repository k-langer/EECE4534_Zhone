#include "ui.h"
#include "display.h"
#include "isrDisp.h"
#include "extio.h"



/***
    init
		-Initialize display and button modules

        *pThis - pointer to ui object to initialize

        return PASS for success or FAIL for failure
***/



void ui_init( ui_t *pThis, isrDisp_t *isrDisp ) {


	int status = IDLE;

	status = isrDisp_init(isrDisp); //Initialize ISR
	if (1 != status) {
		printf("FAIL");
	}


	status =extio_init(isrDisp); //Initialize extio module
	if (1 != status) {
		printf("FAIL");
	}

	extio_callbackRegister(EXTIO_PB0_HIGH, ui_button1_ISR, (void *) pThis);
	extio_callbackRegister(EXTIO_PB1_HIGH, ui_button2_ISR, (void *) pThis);


	display_init();
	display_mainMenu(); 

}

/***
    get_status

        pThis - pointer to ui object to grab status from

        returns status of *pThis
***/
phone_status_t ui_get_status(ui_t* pThis) {

	return pThis->status;
}



/***
    set_status

        pThis - pointer to ui object whose status will be set
        newStatus - status that will be set on *pThis

        returns PASS for success or FAIL for failure
***/
void ui_set_status(ui_t* pThis, phone_status_t newStatus) {

	pThis->status = newStatus;


	if(newStatus == IDLE) {

		display_mainMenu();

	} else if (newStatus == IN_CALL) {

		display_inCallMenu();

	} else if (newStatus == RECEIVING) {

		display_receiveMenu1();

	} else if (newStatus == DIALING) {

		display_makingCallMenu();

	} else if (newStatus == CALL1) {

		display_makingCallMenu();

	} else if (newStatus == CALL2) {

		display_makingCallMenu();

	} else if (newStatus == END_CALL) {

		display_mainMenu();

	} else if (newStatus == ACCEPT_CALL) {

		display_inCallMenu();

	} else if (newStatus == REJECT_CALL) {

		display_mainMenu();

	} else {

		//return FAIL;
	}
	//return PASS;

}


/***
    button1_ISR (left push button)
        -check status to determine state of system
        -update status depending on the currentStatus
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/



void ui_button1_ISR ( void* pThis ) {

	ui_t* ui = (ui_t*)pThis;
	phone_status_t currentStatus = ui->status;

	//phone_status_t currentStatus = ui_get_status( pThis );

	if ( currentStatus == IDLE ) {

		ui_set_status( ui,CALL1 );       //call user1 (when ZigBee detects this and starts sending packets, it needs to change status to DIALING)


	} else if ( currentStatus == RECEIVING ) {

		ui_set_status( ui,IN_CALL );    //accept a call from user1 -> go to IN_CALL

	} else if ( currentStatus == IN_CALL ) {

		ui_set_status( ui, END_CALL );        //hang up and change status to END_CALL. Zhone will change status to idle

	}
}


/***
    button2_ISR (right push button)
        -check status to determine state of system
        -update status
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/
void ui_button2_ISR( void* pThis ) {
	ui_t* ui = (ui_t*)pThis;
	phone_status_t currentStatus = ui->status;
	//phone_status_t currentStatus = ui_get_status( ui ); refer to the status directly instead

	if ( currentStatus == IDLE) {

	    ui_set_status( ui,CALL2 );       //call user2 (when ZigBee detects this and starts sending packets, it needs to change status to DIALING)
		display_makingCallMenu();        //get rid of this, have LED update in set status

	} else if ( currentStatus == RECEIVING ) {

		ui_set_status( ui,IDLE );        //decline a call from user1 -> go to idle

	} else if ( currentStatus == IN_CALL ) {

		ui_set_status( ui, END_CALL );        //Hang up -> go to END_CALL to send the last packet. Zhone will then change state to idle

	}
}

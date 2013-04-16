#include "ui.h" 
#include "display.h"
#include "commonTypes.h" 


/***
    init
		-Initialize display and button modules 
        
        *pThis - pointer to ui object to initialize
        
        return PASS for success or FAIL for failure
***/

return_value_t ui_init(ui_t *pThis) { 	
	//Button_init(); Add the Button_init  
	display_init();

	return PASS; 
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
return_value_t ui_set_status(ui_t* pThis, phone_status_t newStatus) {
	
	pThis->status = newStatus;
	return PASS; 
} 


/***
    button1_ISR (left push button) 
        -check status to determine state of system
        -update status depending on the currentStatus        
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/



void ui_button1_ISR ( ui_t* pThis ) { 
    
	phone_status_t currentStatus = ui_get_status( pThis ); 
	
	if ( currentStatus == IDLE ) {	 
    
		ui_set_status( pThis,CALL1 );       //call user1 (when ZigBee detects this and starts sending packets, it needs to change status to DIALING)
		display_makingCallMenu();	        //update the LED display

	} else if ( currentStatus == RECEIVING ) { 

		ui_set_status( pThis,IN_CALL );    //accept a call from user1 -> go to IN_CALL
		display_inCallMenu();              //update LED display 

	} else if ( currentStatus == RECEIVING ) { 

		ui_set_status( pThis,IN_CALL );    //accept a call from user2 -> go to IN_CALL
		display_inCallMenu();              //update LED display 
	
	} else if ( currentStatus == IN_CALL ) { 
	
		ui_set_status( pThis,END_CALL );        //hang up and change status to END_CALL. Zhone will change status to idle 
		display_mainMenu();                 //update LED display 
	}
}


/***
    button2_ISR (right push button) 
        -check status to determine state of system
        -update status        
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/
void ui_button2_ISR( ui_t* pThis ) {

	phone_status_t currentStatus = ui_get_status( pThis ); 
	if ( currentStatus == IDLE) {     

	    ui_set_status( pThis,CALL2 );       //call user2 (when ZigBee detects this and starts sending packets, it needs to change status to DIALING)
		display_makingCallMenu();           //update the LED display

	} else if ( currentStatus == RECEIVING1 ) {

		ui_set_status( pThis,IDLE );        //decline a call from user1 -> go to idle
		display_mainMenu();                 //update the LED display
    
	} else if ( currentStatus == RECEIVING2 ) {

		ui_set_status( pThis,IDLE );        //decline a call from user2 -> go to idle
		display_mainMenu();                 //update the LED display
	
	} else if ( currentStatus == IN_CALL ) {  

		ui_set_status( pThis,END_CALL );        //Hang up -> go to END_CALL to send the last packet. Zhone will then change state to idle   
		display_mainMenu();                 //update LED display 

	}
}








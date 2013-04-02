#include "ui.h" 
#include "display.h"
#include "commontypes.h" 


/***
    init
		-Initialize display and button modules 
        
        *pThis - pointer to ui object to initialize
        
        return PASS for success or FAIL for failure
***/

return_value_t ui_init(ui_t *pThis) { 	
	//Button_init(); Add the Button_init  
	Display_init();

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
	//everything something happens, get status 

	pThis->status = newStatus;
	
} 


/***
    button1_ISR (left push button) 
        -check status to determine state of system
        -update status        
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/


void ui_button1_ISR(ui_t* pThis) {
	


}


/***
    button2_ISR (right push button) 
        -check status to determine state of system
        -update status        
        -update LEDs for user

        pThis - pointer to ui object to be updated
***/
void ui_button2_ISR(ui_t* pThis) {


}




//thoughts:
//interrupt depending on the state!? 
//the whole point of an interrupt is the change the state, this is what the ISR needs to do 
//ISR can check what the current state is and then do appropriate work 
//interrupts come from button module. button module initializes the portF to prepare for interrupts 
















//the ISR handler is going to be different depending on the current state. 


int receiveCall_ISR_handler() { //execute this when user is receiving a call 
	

}

int makingCall_ISR_handler() { //execute this when user makes a call from main menu  

}

int inCall_ISR_handler() { 

} 


int userInterface_start() { 
	
} 


/*
if(idle) { //idle state
//Display LED1 and LED7 to the user  
}

else if (receiveCall) { //receive call 
//blink corresponding when receiving a cal 
}

else if (makingCall) { 
//blink all LEDs
}

else if (inCall) {
//hold all of the LED lights ON when in a call 
}



}

*/
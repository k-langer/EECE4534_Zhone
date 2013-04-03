#include "commonTypes.h"
#include "ui.h"
#include "phonebook.h"
//#include "transfer_path.h"
//#include "receive_path.h"

#include <stdio.h>


int main() {
    
	/***
    Initialization 
        - Declare all of the objects on the next level 
        - Initializataion all the modules on the next level
	***/

	phone_status_t currentStatus;
    phonebook_t phonebook;
	ui_t userInterface; 

	zhone_address_t a1 = 0xF0;
    zhone_address_t a2 = 0x11;
	
	phonebook_init( &phonebook, a1, a2 );
	ui_init ( &userInterface ); 

	/***
    Main Loop
        - Poll the current status
        - Direct other modules depending on the status
	***/

	while(1) { 

	currentStatus =	ui_get_status( &userInterface );            //poll the status 

	/***
    Transmitting 
        - Inform TX module a call was made

	***/

	if ( currentStatus == CALL1 ) {
       
		//start the TX path
		//printf("calling zhone with #%u", phonebook.address1); 
   
	} else if ( currentStatus == CALL2 ) {
		
		//start the TX path 
        //printf("calling zhone with #%u", phonebook.address2); 
	
	}

	/***
    Receiving
        - Inform RX modules a call is incoming (ZigBee needs to tell Zhone there is a incoming call)
		- Still need to figure out what to do when a END_CALL packet is received 
	***/
	
	if ( currentStatus == RECEIVING1 ) {
	
	    //start the RX path

	}   else if (currentStatus == RECEIVING2) { 

		//start the RX path

	}

	/***
    End Call  
        - Send the END_CALL Message 
		- change status to IDLE 
	***/
	if ( currentStatus == END_CALL ) { 
		
		//send the END_CALL packet
		ui_set_status( &userInterface,IDLE);                    //set the status back IDLE after the last packet was sent 

	}



	} return 0;
}
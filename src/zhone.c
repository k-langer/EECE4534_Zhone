#include "zhone.h"

#include <stdio.h>


return_value_t zhone_init( zhone_t* pThis) {
    
	/***
    Initialization 
        - Declare all of the objects on the next level 
        - Initializataion all the modules on the next level
	***/

	zhone_t zhone = *pThis;

	zhone_address_t a1 = 0xF0;
    zhone_address_t a2 = 0x11;
	
    isrDisp_init( &zhone.isr_disp );
	bufferPool_init( &zhone.buffer_pool );
	phonebook_init( &zhone.phonebook, a1, a2 );
	ui_init ( &zhone.ui, &zhone.isr_disp );
	receive_path_init( &zhone.receive_path, &zhone.buffer_pool, &zhone.isr_disp, &zhone.uart_rx );
	transfer_path_init( &zhone.transfer_path, &zhone.buffer_pool, &zhone.isr_disp, &zhone.uart_tx );

	return PASS;
}

return_value_t zhone_run( zhone_t* pThis) {
	/***
    Main Loop
        - Poll the current status
        - Direct other modules depending on the status
	***/

	zhone_t zhone = *pThis;

	phone_status_t ui_status = zhone.ui.status;
	//initialize wireless status

	while(1) { 

		while ( ui_status == IDLE /* && wireless status is idle*/ ) {
			phone_status_t ui_status = zhone.ui.status;
			//update wireless status
		}

		if ( ui_status != IDLE ) {
			zhone_address_t number_to_call;
			switch ( ui_status ) {
				case CALL1:
					number_to_call = zhone.phonebook.address1;
					break;
				case CALL2:
					number_to_call = zhone.phonebook.address2;
					break;
				default:
					return FAIL;
			}
			//call number_to_call

			ui_set_status( &zhone.ui, DIALING );
			//set wireless status to dialing as well

			//while wireless status remains dialing, poll its status

			switch ( 1 /*wireless status*/ ) {
				case ACCEPT_CALL:
					//accept the call
					ui_set_status( &zhone.ui, IN_CALL );
					break;
				case REJECT_CALL:
					//reject the call
					ui_set_status( &zhone.ui, IDLE );
					continue;	//go back to beginning of big while(1)
				default:
					return FAIL;
			}

		} else if ( 1 /*wireless status is receiving call*/) {
			ui_set_status( &zhone.ui, RECEIVING );

			ui_status = zhone.ui.status;

			while ( ui_status == RECEIVING ) {
				ui_status = zhone.ui.status;
			}

			switch ( ui_status ) {
				case ACCEPT_CALL:
					//accept the call
					ui_set_status( &zhone.ui, IN_CALL );
					break;
				case REJECT_CALL:
					//reject the call
					ui_set_status( &zhone.ui, IDLE );
					continue;	//go back to beginning of big while(1)
				default:
					return FAIL;
			}
		} else {
			return FAIL;
		}

		ui_status = zhone.ui.status;
		//update wireless status

		receive_path_start( &zhone.receive_path );
		transfer_path_start( &zhone.transfer_path );

		while ( ui_status == IN_CALL /* && wireles status is in-call */) {
			if ( receive_path_process_chunk( &zhone.receive_path ) == FAIL ) {
				return FAIL;
			}
			if ( transfer_path_process_chunk( &zhone.transfer_path ) == FAIL ) {
				return FAIL;
			}
			ui_status = zhone.ui.status;
			//update wireless status
		}

		if ( ui_status == END_CALL ) {
			//tell wireless to end call
		} else if ( 1 /*wireless status is not end call*/ ) {
			return FAIL;
		}

		ui_set_status( &zhone.ui, IDLE );
		//set wireless status to IDLE

	}
	
	return PASS;
}

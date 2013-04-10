#include "zhone.h"

#include <stdio.h>


return_value_t zhone_init( zhone_t* pThis) {

	zhone_t zhone = *pThis;

	zhone_address_t a1 = 0xF0;
    zhone_address_t a2 = 0x11;
	
    isrDisp_init( &zhone.isr_disp );
	bufferPool_init( &zhone.buffer_pool );
	phonebook_init( &zhone.phonebook, a1, a2 );
	ui_init ( &zhone.ui, &zhone.isr_disp );
	Wc_Init( &zhone.communicator, &zhone.buffer_pool, &zhone.isr_disp );
	receive_path_init( &zhone.receive_path, &zhone.buffer_pool, &zhone.isr_disp, &zhone.communicator );
	transfer_path_init( &zhone.transfer_path, &zhone.buffer_pool, &zhone.isr_disp, &zhone.communicator );

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
	phone_status_t communicator_status = zhone.communicator.status;

	while(1) { 

		//NO CALL IN SESSION OR REQUESTED
		while ( ui_status == IDLE /* && wireless status is idle*/ ) {
			ui_status = zhone.ui.status;
			communicator_status = zhone.communicator.status;
		}

		//UI SIGNALS A CHANGE IN STATUS, PRESUMABLY TO PLACE A CALL
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

			//CALL THE NUMBER REQUESTED
			Wc_SetDestination( &zhone.communicator, number_to_call);
			if ( Wc_InitiateCall( &zhone.communicator ) == FAIL ) {
				return FAIL;
			}

			ui_set_status( &zhone.ui, DIALING );
			Wc_SetStatus( &zhone.communicator, DIALING );

			communicator_status = zhone.communicator.status;

			//WAIT FOR REMOTE ZHONE TO ANSWER THE CALL
			while ( communicator_status == DIALING ) {
				communicator_status = zhone.communicator.status;
			}

			//ACCEPT OR REJECT THE CALL AS INSTRUCTED
			switch ( communicator_status ) {
				case ACCEPT_CALL:
					Wc_SetStatus ( &zhone.communicator, IN_CALL );
					ui_set_status( &zhone.ui, IN_CALL );
					break;
				case REJECT_CALL:
					Wc_SetStatus( &zhone.communicator, IDLE );
					ui_set_status( &zhone.ui, IDLE );
					continue;	//go back to beginning of big while(1)
				default:
					return FAIL;
			}

		}
		//COMMUNICATOR SIGNALS A CHANGE IN STATUS, PRESUMABLY RECEIVING A CALL
		else if ( communicator_status == RECEIVING ) {
			ui_set_status( &zhone.ui, RECEIVING );
			ui_status = zhone.ui.status;

			//WAIT FOR UI TO DECIDE WHETHER TO ACCEPT/REJECT CALL
			while ( ui_status == RECEIVING ) {
				ui_status = zhone.ui.status;
			}

			//ACCEPT OR REJECT CALL AS INSTRUCTED
			switch ( ui_status ) {
				case ACCEPT_CALL:
					if ( Wc_AcceptCall( &zhone.communicator ) == FAIL ) {
						return FAIL;
					}
					Wc_SetStatus( &zhone.communicator, IN_CALL );
					ui_set_status( &zhone.ui, IN_CALL );
					break;
				case REJECT_CALL:
					if ( Wc_RejectCall( &zhone.communicator ) == FAIL ) {
						return FAIL;
					}
					Wc_SetStatus( &zhone.communicator, IDLE );
					ui_set_status( &zhone.ui, IDLE );
					continue;	//go back to beginning of big while(1)
				default:
					return FAIL;
			}
		} else {
			//UNEXPECTED STATUS CHANGE OCCURRED
			return FAIL;
		}

		//IF THIS POINT IS REACHED, ZHONE MUST BE IN-CALL
		ui_status = zhone.ui.status;
		communicator_status = zhone.communicator.status;

		//START FLOW OF DATA
		Wc_Start( &zhone.communicator );
		receive_path_start( &zhone.receive_path );
		transfer_path_start( &zhone.transfer_path );

		//CYCLE BETWEEN PROCESSING OUTGOING AND INCOMING CHUNKS UNTIL
		//CHANGE IN STATUS IS SIGNALLED BY UI OR COMMUNICATOR
		while ( ui_status == IN_CALL && communicator_status == IN_CALL ) {
			if ( receive_path_process_chunk( &zhone.receive_path ) == FAIL ) {
				return FAIL;
			}
			if ( transfer_path_process_chunk( &zhone.transfer_path ) == FAIL ) {
				return FAIL;
			}
			ui_status = zhone.ui.status;
			communicator_status = zhone.communicator.status;
		}

		//END CALL!
		if ( ui_status == END_CALL ) {
			if ( Wc_EndCall( &zhone.communicator ) == FAIL ) {
				return FAIL;
			}
		} else if ( communicator_status != END_CALL ) {
			return FAIL;
		}

		ui_set_status( &zhone.ui, IDLE );
		Wc_SetStatus( &zhone.communicator, IDLE );

	}
	
	return PASS;
}

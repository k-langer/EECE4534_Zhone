#ifndef ZHONE_H
#define ZHONE_H

#include "commonTypes.h"
#include "phonebook.h"
#include "ui.h"
#include "bufferPool.h"
#include "receive_path.h"
#include "transfer_path.h"
#include "wc.h"
#include "isrDisp.h"


/***

* Zhone
 * 	-main struct of the zhone device
 * 	-contains other elements of the zhone
 * 	-controls flow of data between elements, intitiation/termination of calls
 *
***/

typedef struct {
	receive_path_t receive_path;
	transfer_path_t transfer_path;
	phonebook_t phonebook;
	ui_t ui;
	bufferPool_t buffer_pool;
	isrDisp_t isr_disp;
	wc_t communicator;
} zhone_t;

return_value_t zhone_init( zhone_t* pThis );
return_value_t zhone_run( zhone_t* pThis );

#endif

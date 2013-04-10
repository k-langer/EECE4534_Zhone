/******************************************************************************
 *@file: wc.h
 *
 *@brief: 
 *  - Sends and receives messages to other components
 *
 * Target: TLL6527v1-0
 * Compiler: GCC     Output format: ELF
 *
 * @author:  Todd Lunter
 * @created: April 9, 2013
 *
 *******************************************************************************/

#ifndef WC_H_
#define WC_H_

#include "commonTypes.h"


typedef struct {
	phone_status_t status;
	queue_t tx_queue;
	queue_t rx_queue;

	bufferPool_t* pBufferPool;
} wc_t;
/** Initializes the wireless communicator
 *
 * @param pThis     the wireless communicator structure
 * @param pBufPool  the buffer pool pointer
 * @param pIsrDisp  the ISR Dispatcher pointer
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Init( wc_t *pThis, bufferPool_t *pBufPool, isrDisp_t *pIsrDisp );

/** Starts the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Start( wc_t *pThis );

/** Stops the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Stop( wc_t *pThis );

/** Sets who to target with the messages
 *
 * @param to    short address to target
 *
 * @return PASS on sucess, FAIL otherwise
 */
return_value_t Wc_SetDestination( wc_t *pThis, zhone_address_t to );

/** Sends an initialize call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_InitiateCall( wc_t *pThis );

/** Sends an accept call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_AcceptCall( wc_t *pThis );

/** Sends a data message with the given data
 *
 * @param pChunk    chunk of data to send out the Xbee module
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Send( wc_t *pThis, chunk_t *pChunk );

/** Sends an end call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_EndCall( wc_t *pThis );

/** Sends a reject call message
 *
 * @return PASS on sucess, FAIL otherwise
 */
return_value_t Wc_RejectCall( wc_t *pThis );

/** Gets a message from the communicator
 *
 * @param pData     chunk of data to overwrite
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Receive( wc_t *pThis, chunk_t *pChunk );

/** Sets the status of the communicator
 *
 * @param status	new status communicator will be set to
 *
 * @return PASS on succes, FAIL otherwise
 */
return_value_t Wc_SetStatus( wc_t *pThis );

#endif /* WC_H_ */

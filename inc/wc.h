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
#include "bufferPool.h"
#include "queue.h"
#include "chunk.h"
#include "uartRx.h"
#include "uartTx.h"

#if WIRE
#include "wire.h"
#else
#include "xbee.h"
#endif

/** Definitions */
#define WC_INITIATE_CALL_BYTE   0x01
#define WC_ACCEPT_CALL_BYTE     0x02
#define WC_END_CALL_BYTE        0x03
#define WC_REJECT_CALL_BYTE     0x04
#define WC_DATA_BYTE            0x05

/** Structures */
typedef struct {
    uartRx_t rx;
    uartTx_t tx;
    isrDisp_t *pIsr;
    bufferPool_t *pBufPool;
    bufferPool_t *pMiniPool;
#if WIRE
    wire_t wire;
#else
    xbee_t xbee;
#endif
    unsigned short to;
	phone_status_t status;
} wc_t;

/** Initializes the wireless communicator
 *
 * @param pThis     the wireless communicator structure
 * @param pBufPool  the buffer pool pointer
 * @param pIsrDisp  the ISR Dispatcher pointer
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Init( wc_t *pThis, bufferPool_t *pBufPool, isrDisp_t *pIsrDisp );

/** Starts the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Start( wc_t *pThis );

/** Stops the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Stop( wc_t *pThis );

/** Sets who to target with the messages
 *
 * @param to    short address to target
 *
 * @return PASS on sucess, FAIL otherwise
 */
int Wc_SetDestination( wc_t *pThis, unsigned short to );

/** Sends an initialize call message
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_InitiateCall( wc_t *pThis );

/** Sends an accept call message
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_AcceptCall( wc_t *pThis );

/** Sends a data message with the given data
 *
 * @param pChunk    chunk of data to send out the Xbee module
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Send( wc_t *pThis, chunk_t *pChunk );

/** Sends an end call message
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_EndCall( wc_t *pThis );

/** Sends a reject call message
 *
 * @return PASS on sucess, FAIL otherwise
 */
int Wc_RejectCall( wc_t *pThis );

/** Gets a message from the communicator
 *
 * @param pData     chunk of data to overwrite
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Receive( wc_t *pThis, chunk_t *pChunk );

#endif /* WC_H_ */

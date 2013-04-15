/******************************************************************************
 *@file: wc.c
 *
 *@brief: 
 *  - Sends and receives messages to other components
 *
 * Target: TLL6527v1-0
 * Compiler: GCC     Output format: ELF
 *
 * @author:  Todd Lunter
 * @created: April 15, 2013
 *
 *******************************************************************************/

#include "commonTypes.h"
#include "wc.h"

/** Initializes the wireless communicator
 *
 * @param pThis     the wireless communicator structure
 * @param pBufPool  the buffer pool pointer
 * @param pIsrDisp  the ISR Dispatcher pointer
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Init( wc_t *pThis, bufferPool_t *pBufPool, isrDisp_t *pIsrDisp )
{
    uartRx_init(&pThis->rx, pThis->pIsr, pThis->pBufPool);
    uartTx_init(&pThis->tx, pThis->pIsr, pThis->pBufPool);

    Wire_Init(&pThis->wire, &pThis->rx, &pThis->tx, &pThis->pBufPool);
    Xbee_Init(&pThis->xbee, &pThis->rx, &pThis->tx, &pThis->pBufPool);
}

/** Starts the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Start( void )
{
    uartRx_start(&pThis->rx);
}

/** Stops the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Stop( void );

/** Sets who to target with the messages
 *
 * @param to    short address to target
 *
 * @return PASS on sucess, FAIL otherwise
 */
return_value_t Wc_SetDestination( unsigned short to );

/** Sends an initialize call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_InitiateCall( void );

/** Sends an accept call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_AcceptCall( void );

/** Sends a data message with the given data
 *
 * @param pChunk    chunk of data to send out the Xbee module
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Send( chunk_t *pChunk );

/** Sends an end call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_EndCall( void );

/** Sends a reject call message
 *
 * @return PASS on sucess, FAIL otherwise
 */
return_value_t Wc_RejectCall( void );

/** Gets a message from the communicator
 *
 * @param pData     chunk of data to overwrite
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Receive( chunk_t *pChunk );

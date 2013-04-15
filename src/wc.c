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

    return PASS;
}

/** Starts the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Start( wc_t *pThis, void )
{
    uartRx_start(&pThis->rx);

    return PASS;
}

/** Stops the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Stop( wc_t *pThis, void )
{
    return PASS;
}

/** Sets who to target with the messages
 *
 * @param to    short address to target
 *
 * @return PASS on sucess, FAIL otherwise
 */
return_value_t Wc_SetDestination( wc_t *pThis, unsigned short to )
{
    pThis->to = to;
    
    return PASS;
}

/** Sends an initialize call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_InitiateCall( wc_t *pThis, void )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = INITIATE_CALL_BYTE;
        pChunk->bytesUsed = 1;

        return Wire_Send(&pThis->wire, pChunk);
    }
    return FAIL;
}

/** Sends an accept call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_AcceptCall( wc_t *pThis, void )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = ACCEPT_CALL_BYTE;
        pChunk->bytesUsed = 1;

        return Wire_Send(&pThis->wire, pChunk);
    }
    return FAIL;
}

/** Sends a data message with the given data
 *
 * @param pChunk    chunk of data to send out the Xbee module
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Send( wc_t *pThis, chunk_t *pChunk )
{
    int i;
    int len = pChunk->bytesUsed;
    for (i = len - 1; i >= 0; i--)
    {
        pChunk->u08_buff[i+1] = pChunk->u08_buff[i];
    }
    pChunk->u08_buff[0] = DATA_BYTE;
    pChunk->bytesUsed = 1 + len;

    return Wire_Send(&pThis->wire, pChunk);
}

/** Sends an end call message
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_EndCall( wc_t *pThis, void )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = END_CALL_BYTE;
        pChunk->bytesUsed = 1;

        return Wire_Send(&pThis->wire, pChunk);
    }
    return FAIL;
}

/** Sends a reject call message
 *
 * @return PASS on sucess, FAIL otherwise
 */
return_value_t Wc_RejectCall( wc_t *pThis, void )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = REJECT_CALL_BYTE;
        pChunk->bytesUsed = 1;

        return Wire_Send(&pThis->wire, pChunk);
    }
    return FAIL;
}

/** Gets a message from the communicator
 *
 * @param pData     chunk of data to overwrite
 *
 * @return PASS on success, FAIL otherwise
 */
return_value_t Wc_Receive(a wc_t *pThis, chunk_t *pChunk )
{
    /* Check for status update or data */
    return PASS;
}

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

#include "startup.h"
#include "bf52x_uart.h"
#include "tll_common.h"
#include "commonTypes.h"
#include "wc.h"

#if WIRE
#include "wire.h"
#else
#include "xbee.h"
#endif

bufferPool_t miniPool;

/** Initializes the wireless communicator
 *
 * @param pThis     the wireless communicator structure
 * @param pBufPool  the buffer pool pointer
 * @param pIsrDisp  the ISR Dispatcher pointer
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Init( wc_t *pThis, bufferPool_t *pBufPool, isrDisp_t *pIsrDisp )
{
    pThis->pIsr     = pIsrDisp;
    pThis->pBufPool = pBufPool;
    pThis->pMiniPool = &miniPool;
    bufferPool_init( pThis->pMiniPool );

    uartRx_init(&pThis->rx, pThis->pMiniPool, pThis->pIsr);
    uartTx_init(&pThis->tx, pThis->pMiniPool, pThis->pIsr);

#if WIRE
    Wire_Init(&pThis->wire, &pThis->rx, &pThis->tx, pThis->pMiniPool);
#else
    Xbee_Init(&pThis->xbee, &pThis->rx, &pThis->tx, pThis->pBufPool);
#endif

    return PASS;
}

/** Starts the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Start( wc_t *pThis )
{
    bf52x_uart_settings settings = {
        .parenable = 0,
        .parity = 0,
        .rxtx_baud = BF52X_BAUD_RATE_9600
    };

    bf52x_uart_deinit();
    bf52x_uart_init(&settings); 

    *pPORTF_FER |= 0xc000;
    *pPORTF_MUX &= ~0x0400;
    *pPORTF_MUX |= 0x0800;
    *pPORTFIO_DIR |= 0x4000;
    *pPORTFIO_DIR &= ~(0x8000);

    return uartRx_start(&pThis->rx);
}

/** Stops the wireless communicator
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Stop( wc_t *pThis )
{
    uartRx_dmaStop();

    bf52x_uart_settings settings = {
        .parenable = 0,
        .parity = 0,
        .rxtx_baud = BF52X_BAUD_RATE_115200
    };

    bf52x_uart_deinit();
    bf52x_uart_init(&settings); 

    *pPORTF_FER &= ~0xc000;

    return PASS;
}

/** Sets who to target with the messages
 *
 * @param to    short address to target
 *
 * @return PASS on sucess, FAIL otherwise
 */
int Wc_SetDestination( wc_t *pThis, unsigned short to )
{
    pThis->to = to;
    
    return PASS;
}

/** Sends an initialize call message
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_InitiateCall( wc_t *pThis )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = WC_INITIATE_CALL_BYTE;
        pChunk->bytesUsed = 1;

#if WIRE
        return Wire_SendMessage(&pThis->wire, pChunk);
#else
        return Xbee_SendTransmitMessage(&pThis->xbee, pThis->to, pChunk);
#endif
    }
    return FAIL;
}

/** Sends an accept call message
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_AcceptCall( wc_t *pThis )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = WC_ACCEPT_CALL_BYTE;
        pChunk->bytesUsed = 1;

#if WIRE
        return Wire_SendMessage(&pThis->wire, pChunk);
#else
        return Xbee_SendTransmitMessage(&pThis->xbee, pThis->to, pChunk);
#endif
    }
    return FAIL;
}

/** Sends a data message with the given data
 *
 * @param pChunk    chunk of data to send out the Xbee module
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Send( wc_t *pThis, chunk_t *pChunk )
{
    int i;
    int len;
    int status = 0;
    chunk_t *new_chunk;

    while (pChunk->bytesUsed)
    {
        len = (pChunk->bytesUsed <= 100) ? pChunk->bytesUsed : 100;

        status = bufferPool_acquire(pThis->pMiniPool, &new_chunk);
        if (FAIL == status)
        {
            printf("Mini Pool Fail!\n");
            break;
        }

        new_chunk->u08_buff[0] = WC_DATA_BYTE;
        for (i = 0; i < len; i++)
        {
            new_chunk->u08_buff[i+1] = pChunk->u08_buff[i];
        }
        new_chunk->bytesUsed = 1 + len;

        for (i = len; i < pChunk->bytesUsed; i++)
        {
            pChunk->u08_buff[i] = pChunk->u08_buff[i + len];
        }

        pChunk->bytesUsed -= len;

#if WIRE
        status = Wire_SendMessage(&pThis->wire, new_chunk);
#else
        status = Xbee_SendTransmitMessage(&pThis->xbee, pThis->to, new_chunk);
#endif
        if (FAIL == status)
        {
            break;
        }
    }
    bufferPool_release(pThis->pBufPool, pChunk);
    return status;
}

/** Sends an end call message
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_EndCall( wc_t *pThis )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = WC_END_CALL_BYTE;
        pChunk->bytesUsed = 1;

#if WIRE
        return Wire_SendMessage(&pThis->wire, pChunk);
#else
        return Xbee_SendTransmitMessage(&pThis->xbee, pThis->to, pChunk);
#endif
    }
    return FAIL;
}

/** Sends a reject call message
 *
 * @return PASS on sucess, FAIL otherwise
 */
int Wc_RejectCall( wc_t *pThis )
{
    chunk_t *pChunk = NULL;
    if (PASS == bufferPool_acquire(pThis->pBufPool, &pChunk))
    {
        pChunk->u08_buff[0] = WC_REJECT_CALL_BYTE;
        pChunk->bytesUsed = 1;

#if WIRE
        return Wire_SendMessage(&pThis->wire, pChunk);
#else
        return Xbee_SendTransmitMessage(&pThis->xbee, pThis->to, pChunk);
#endif
    }
    return FAIL;
}

/** Gets a message from the communicator
 *
 * @param pData     chunk of data to overwrite
 *
 * @return PASS on success, FAIL otherwise
 */
int Wc_Receive( wc_t *pThis, chunk_t *pChunk )
{
    unsigned char received_msg = 0;
    while (!received_msg)
    {
#if WIRE
        if ( FAIL == Wire_GetMessageNb(&pThis->wire, pChunk) )
#else
        if ( FAIL == Xbee_GetMessageNb(&pThis->xbee, pChunk) )
#endif
        {
            return FAIL;
        }

        int i;
        switch (pChunk->u08_buff[0])
        {
            case WC_INITIATE_CALL_BYTE:
                pThis->status = DIALING;
                break;
            case WC_ACCEPT_CALL_BYTE:
                pThis->status = ACCEPT_CALL;
                break;
            case WC_END_CALL_BYTE:
                pThis->status = END_CALL;
                break;
            case WC_REJECT_CALL_BYTE:
                pThis->status = IDLE;
                break;
            case WC_DATA_BYTE:
                for (i = 1; i < pChunk->bytesUsed; i++)
                {
                    pChunk->u08_buff[i-1] = pChunk->u08_buff[i];
                }
                pChunk->bytesUsed--;
                received_msg = 1;
                break;
            default:
                break;
        }
    }
    return PASS;
}

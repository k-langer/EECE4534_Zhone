#include "tll_common.h"
#include "uartRx.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <tll_config.h>
#include <tll_sport.h>
#include <queue.h>
#include <power_mode.h>


char sramPending[120] __attribute__((section(".sram_app.data")));

/** 
 * Configures the DMA rx with the buffer and the buffer length to 
 * receive
 * Parameters:
 * @param pchunk  pointer to receive chunk
 *
 * @return void
 */
void uartRx_dmaConfig(char* cPtr, unsigned short len)
{
	DISABLE_DMA(*pDMA10_CONFIG);
	*pDMA10_START_ADDR   = cPtr;
	*pDMA10_X_COUNT      = len;
	*pDMA10_X_MODIFY     = 1;
	*pUART1_IER         |= 0x1;
	ENABLE_DMA(*pDMA10_CONFIG);
}

/** Initialize audio rx
 *    - get pointer to buffer pool
 *    - register interrupt handler
 *    - initialize RX queue

 * Parameters:
 * @param pThis  pointer to own object
 * @param pBuffP  pointer to buffer pool to take and return chunks from
 * @param pIsrDisp   pointer to interrupt dispatcher to get ISR registered
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int uartRx_init(uartRx_t *pThis, bufferPool_t *pBuffP,
                 isrDisp_t *pIsrDisp)
{
    if ( NULL == pThis || NULL == pBuffP || NULL == pIsrDisp) {
        //printf("[UART_RX]: Failed init\r\n");
        return FAIL;
    }
    
    pThis->pPending = NULL;
    pThis->pBuffP = pBuffP;
    pThis->state = UARTRX_WAITING;
    
    // init queue with 
    queue_init(&pThis->queue, UARTRX_QUEUE_DEPTH);
 
     /* Configure the DMA3 for RX (data receive/memory write) */
     /* Read, 1-D, interrupt enabled, Memory write operation, 16 bit transfer,
      * Auto buffer
      */
    *pDMA10_CONFIG = 0x10A2;//0xA2; //SYNC | WNR | WDSIZE_8 | DI_EN;

    /**
     * Register the interrupt handler
     */
    isrDisp_registerCallback(pIsrDisp, ISR_DMA10_UART1_RX, uartRx_isr, pThis);

    //printf("[UART_RX]: RX init complete\r\n");
    
    return PASS;
}




/** start audio rx
 *    - start receiving first chunk from DMA
 *      - acqurie chunk from pool 
 *      - config DMA 
 *      - start DMA + SPORT
 * Parameters:
 * @param pThis  pointer to own object
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int uartRx_start(uartRx_t *pThis)
{

	//printf("[UART RX]: uartRx_start: implemented\r\n");

    /* prime the system by getting the first buffer filled */
    if ( FAIL == bufferPool_acquire(pThis->pBuffP, &pThis->pPending ) ) {
        //printf("[UART_RX]: Failed to acquire buffer\r\n");
        return FAIL;
    }
    
    //*pSIC_IMASK = 0x00000000;
    //uartRx_dmaConfig(pThis->pPending);
    uartRx_dmaConfig(sramPending, 3);
    //*pSIC_IMASK = 0xffffffff;
    
    // enable the audio transfer
    //printf("Enabled XBEE RX\r\n");
    //ENABLE_DMA(*pDMA10_CONFIG);
    
    return PASS;                                         
}

void uartRx_dmaStop(void) {
    //*pSIC_IMASK = 0x00000000;
	// stop DMA it might otherwise bombard us with interrupts
    DISABLE_DMA(*pDMA10_CONFIG);

    // turn off the TX ISR as we do not use the DMA anymore
    *pUART1_IER &= ~ERBFI;

    //*pSIC_IMASK = 0xffffffff;
    return;
}

/** uartRx_isr

 * Parameters:
 * @param pThisArg  pointer to own object
 *
 * @return None 
 */
void uartRx_isr(void *pThisArg)
{
	//printf("RX finished\r\n");
    // local pThis to avoid constant casting 
    uartRx_t *pThis  = (uartRx_t*) pThisArg;
    
    if ( *pDMA10_IRQ_STATUS & 0x1 ) {

        static int isr = 0;
        isr++;
        if (isr > 100)
        {
            asm("nop");
            isr = 0;
        }

        if (pThis->state == UARTRX_WAITING)
        {
            unsigned short packet_length = ((sramPending[1] << 8) | sramPending[2]) + 1;

            if (0x7e == sramPending[0] && 0 < packet_length)
            {
                int i;
                for (i = 0; i < 3; i++) {
                    pThis->pPending->s08_buff[i] = sramPending[i];
                }

                pThis->state = UARTRX_COMPLETING;
                uartRx_dmaConfig(sramPending, packet_length);
            }
            else if (0x7e == sramPending[1])
            {
                sramPending[0] = 0x7e;    
                sramPending[1] = sramPending[2];
                uartRx_dmaConfig(sramPending+2, 1);
            }
            else if (0x7e == sramPending[2])
            {
                sramPending[0] = 0x7e;    
                uartRx_dmaConfig(sramPending+1, 2);
            }
            else
            {
                uartRx_dmaConfig(sramPending, 3);
            }
        }
        else if (pThis->state == UARTRX_COMPLETING)
        {
            int i;
            for (i = 0; i < pThis->pPending->s08_buff[2] + 1; i++) {
                pThis->pPending->s08_buff[i+3] = sramPending[i];
            }
            
            // chunk is now filled update the length
            pThis->pPending->bytesUsed = pThis->pPending->s08_buff[2] + 4; //pThis->pPending->size;

            /* Insert the chunk previously read by the DMA RX on the
                RX QUEUE and a data is inserted to queue
             */
            if ( FAIL == queue_put(&pThis->queue, pThis->pPending) ) {
                // reuse the same buffer and overwrite last samples
                //uartRx_dmaConfig(pThis->pPending);
                uartRx_dmaConfig(sramPending, 3);

                //printf("[INT]: RX packet dropped\r\n");
            } else {
                if ( PASS == bufferPool_acquire(pThis->pBuffP, &pThis->pPending ) ) {
                    uartRx_dmaConfig(sramPending, 3);
                } else {
                    //printf("Buffer pool empty!\r\n");
                }
            }

            pThis->state = UARTRX_WAITING;
        }
        *pDMA10_IRQ_STATUS  |= 0x0001;  // clear the interrupt
    }
}

int uartRx_getChunk(uartRx_t *pThis, chunk_t *pChunk)
{
    chunk_t *chunk_rx;
    
    queue_get(&pThis->queue, (void**)&chunk_rx);

	unsigned int count;
	unsigned int len = chunk_rx->bytesUsed;

	// copy manuall since memcpy does not work currently
	for (count = 0; len > count; count++) {
		pChunk->u08_buff[count] = chunk_rx->u08_buff[count];
	}

	// update length of actual copied data
	pChunk->bytesUsed = chunk_rx->bytesUsed;
    
    if ( FAIL == bufferPool_release(pThis->pBuffP, chunk_rx) ) {
        return FAIL;
    }
    

    return PASS;
}

/** audio rx get 
 *   copyies a filled chunk into pChunk
 *   blocking call, blocks if queue is empty 
 *     - get from queue 
 *     - copy in to pChunk
 *     - release chunk to buffer pool 
 * Parameters:
 * @param pThis  pointer to own object
 * @param pChunk Pointer to chunk object
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int uartRx_get(uartRx_t *pThis, chunk_t *pChunk)
{
    /* Block till a chunk arrives on the rx queue */
    while( queue_is_empty(&pThis->queue) )
	{
    	//printf("RX Queue is empty\r\n");
//		powerMode_change(PWR_ACTIVE);
        asm("idle;");
    }
    powerMode_change(PWR_FULL_ON);

	return uartRx_getChunk(pThis, pChunk);    
}

int uartRx_getNb(uartRx_t *pThis, chunk_t *pChunk)
{
    /* Block till a chunk arrives on the rx queue */
    if( queue_is_empty(&pThis->queue) )
	{
    	//printf("RX Queue is empty\r\n");
//		powerMode_change(PWR_ACTIVE);
        //asm("idle;");
		return FAIL;
    }
    //powerMode_change(PWR_FULL_ON);
	return uartRx_getChunk(pThis, pChunk);    
}


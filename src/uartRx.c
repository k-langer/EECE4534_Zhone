#include "tll_common.h"
#include "uartRx.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <tll_config.h>
#include <tll_sport.h>
#include <queue.h>
#include <power_mode.h>


//chunk_t sramPending __attribute__((section(".sram_app.data")));
char sramPending[10] __attribute__((section(".sram_app.data")));

/** 
 * Configures the DMA rx with the buffer and the buffer length to 
 * receive
 * Parameters:
 * @param pchunk  pointer to receive chunk
 *
 * @return void
 */
void uartRx_dmaConfig(char* cPtr)
{
	DISABLE_DMA(*pDMA10_CONFIG);
	*pDMA10_START_ADDR   = cPtr;
	*pDMA10_X_COUNT      = 10; //pchunk->size;
	*pDMA10_X_MODIFY     = 1;
	*pUART1_IER |= 0x1;
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
        printf("[UART_RX]: Failed init\r\n");
        return FAIL;
    }
    
    pThis->pPending = NULL;
    pThis->pBuffP = pBuffP;
    
    // init queue with 
    queue_init(&pThis->queue, UARTRX_QUEUE_DEPTH);
 
     /* Configure the DMA3 for RX (data receive/memory write) */
     /* Read, 1-D, interrupt enabled, Memory write operation, 16 bit transfer,
      * Auto buffer
      */
    *pDMA10_CONFIG = 0xA2; //SYNC | WNR | WDSIZE_8 | DI_EN;

    /**
     * Register the interrupt handler
     */
    isrDisp_registerCallback(pIsrDisp, ISR_DMA10_UART1_RX, uartRx_isr, pThis);
    
    //chunk_init(&sramPending);

    printf("[UART_RX]: RX init complete\r\n");
    
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

	printf("[UART RX]: uartRx_start: implemented\r\n");

    /* prime the system by getting the first buffer filled */
     if ( FAIL == bufferPool_acquire(pThis->pBuffP, &pThis->pPending ) ) {
         printf("[UART_RX]: Failed to acquire buffer\r\n");
         return FAIL;
     }
     
     //uartRx_dmaConfig(pThis->pPending);
     uartRx_dmaConfig(sramPending);
     
     // enable the audio transfer
     //printf("Enabled XBEE RX\r\n");
     //ENABLE_DMA(*pDMA10_CONFIG);
     
     return PASS;                                         
}



/** uartRx_isr

 * Parameters:
 * @param pThisArg  pointer to own object
 *
 * @return None 
 */
void uartRx_isr(void *pThisArg)
{
	printf("RX finished\r\n");
    // local pThis to avoid constant casting 
    uartRx_t *pThis  = (uartRx_t*) pThisArg;
    

    if ( *pDMA10_IRQ_STATUS & 0x1 ) {

		int i;
		for (i = 0; i < 10; i++) {
//			printf("%c = 0x%X\r\n", sramPending[i], sramPending[i]);
			pThis->pPending->s08_buff[i] = sramPending[i];
		}
		pThis->pPending->s08_buff[10] = '\r'; pThis->pPending->s08_buff[11] = '\n';

    	//chunk_copy(&sramPending, pThis->pPending);
    	//chunk_init(&sramPending);

        // chunk is now filled update the length
        pThis->pPending->bytesUsed = 12; //pThis->pPending->size;
        
        /* Insert the chunk previously read by the DMA RX on the
            RX QUEUE and a data is inserted to queue
         */
        if ( FAIL == queue_put(&pThis->queue, pThis->pPending) ) {

            // reuse the same buffer and overwrite last samples
            //uartRx_dmaConfig(pThis->pPending);
        	uartRx_dmaConfig(sramPending);

            printf("[INT]: RX packet dropped\r\n");
        } else {

            if ( PASS == bufferPool_acquire(pThis->pBuffP, &pThis->pPending ) ) {
                //uartRx_dmaConfig(pThis->pPending);
            	uartRx_dmaConfig(sramPending);
            } else {
                printf("Buffer pool empty!\r\n");
            }
        }
        
        *pDMA10_IRQ_STATUS  |= 0x0001;  // clear the interrupt
    }
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
    chunk_t *chunk_rx;
    
    /* Block till a chunk arrives on the rx queue */
    while( queue_is_empty(&pThis->queue) ) {
    	printf("RX Queue is empty\r\n");
//		powerMode_change(PWR_ACTIVE);
        asm("idle;");
    }
    powerMode_change(PWR_FULL_ON);
    
    queue_get(&pThis->queue, (void**)&chunk_rx);

    chunk_copy(chunk_rx, pChunk);
    
    if ( FAIL == bufferPool_release(pThis->pBuffP, chunk_rx) ) {
        return FAIL;
    }
    

    return PASS;
}

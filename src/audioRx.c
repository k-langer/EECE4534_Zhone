/**
 *@file audioRx.c
 *
 *@brief
 *  - receive audio samples from DMA
 *
 * Target:   TLL6527v1-0      
 * Compiler: VDSP++     Output format: VDSP++ "*.dxe"
 *
 * @author  Gunar Schirner
 *          Rohan Kangralkar
 * @date 	03/15/2009
 *
 * LastChange:
 * $Id: audioRx.c 7 2013-03-29 03:06:06Z ovaskevi $
 *
 *******************************************************************************/
#include "tll_common.h"
#include "audioRx.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <tll_config.h>
#include <tll_sport.h>
#include <queue.h>
#include <power_mode.h>



/** 
 * Configures the DMA rx with the buffer and the buffer length to 
 * receive
 * Parameters:
 * @param pchunk  pointer to receive chunk
 *
 * @return void
 */
void audioRx_dmaConfig(chunk_t *pchunk)
{
    DISABLE_DMA(*pDMA3_CONFIG);
    *pDMA3_START_ADDR   = &pchunk->u16_buff[0];
    *pDMA3_X_COUNT      = pchunk->bytesMax/2;  // 16 bit data so we change the stride and count
    *pDMA3_X_MODIFY     = 2;
    ENABLE_DMA(*pDMA3_CONFIG);
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
int audioRx_init(audioRx_t *pThis, bufferPool_t *pBuffP, 
                 isrDisp_t *pIsrDisp)
{
    if ( NULL == pThis || NULL == pBuffP || NULL == pIsrDisp) {
        printf("[ARX]: Failed init\n");
        return FAIL;
    }
    
    pThis->pPending     = NULL;
    pThis->pBuffP       = pBuffP;
    
    // init queue with 
    queue_init(&pThis->queue, AUDIORX_QUEUE_DEPTH);   
 
     /* Configure the DMA3 for RX (data receive/memory write) */
     /* Read, 1-D, interrupt enabled, Memory write operation, 16 bit transfer,
      * Auto buffer
      */
    *pDMA3_CONFIG = WNR | WDSIZE_16 | DI_EN | FLOW_AUTO;

    /**
     * Register the interrupt handler
     */

    isrDisp_registerCallback(pIsrDisp, ISR_DMA3_SPORT0_RX, audioRx_isr, pThis);
    
    printf("[ARX]: RX init complete\n");
    
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
int audioRx_start(audioRx_t *pThis)
{

    /* prime the system by getting the first buffer filled */
     if ( FAIL == bufferPool_acquire(pThis->pBuffP, &pThis->pPending ) ) {
         printf("[ARX]: Failed to acquire buffer\n");
         return FAIL;
     }
     
     audioRx_dmaConfig(pThis->pPending);    
     
     // enable the audio transfer 
     ENABLE_SPORT0_RX();
     
     return PASS;                                         
}



/** audioRx_isr

 * Parameters:
 * @param pThisArg  pointer to own object
 *
 * @return None 
 */
void audioRx_isr(void *pThisArg)
{

    // local pThis to avoid constant casting 
    audioRx_t *pThis  = (audioRx_t*) pThisArg; 
    
    if ( *pDMA3_IRQ_STATUS & 0x1 ) {

        // chunk is now filled update the length
        pThis->pPending->bytesUsed = pThis->pPending->bytesMax;
        
        /* Insert the chunk previously read by the DMA RX on the
            RX QUEUE and a data is inserted to queue
         */
        if ( FAIL == queue_put(&pThis->queue, pThis->pPending) ) {
            
            // reuse the same buffer and overwrite last samples 
            audioRx_dmaConfig(pThis->pPending);
            
            //printf("[INT]: RX packet dropped\n");
        } else {
            
            if ( PASS == bufferPool_acquire(pThis->pBuffP, &pThis->pPending ) ) {
                audioRx_dmaConfig(pThis->pPending);
            } else {
                printf("Buffer pool empty!\n");
            }
        }
        
        *pDMA3_IRQ_STATUS  |= 0x0001;  // clear the interrupt
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
int audioRx_get(audioRx_t *pThis, chunk_t *pChunk)
{
    chunk_t                  *chunk_rx;
    
    /* Block till a chunk arrives on the rx queue */
    while( queue_is_empty(&pThis->queue) ) {
    	asm("nop;");
    }
    
    queue_get(&pThis->queue, (void**)&chunk_rx);

    chunk_copy(chunk_rx, pChunk);
    
    if ( FAIL == bufferPool_release(pThis->pBuffP, chunk_rx) ) {
        return FAIL;
    }
    
    return PASS;
}

/** audio rx get
 *    non-blocking
 *    no copy
 *    caller is responsible for releasing the buffer
 *
 * Parameters:
 * @param pThis  pointer to own object
 * @param ppChunk Pointer Pointer to chunk object
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int audioRx_getNbNc(audioRx_t *pThis, chunk_t **ppChunk)
{

    // check if something in queue
    if(queue_is_empty(&pThis->queue)){
    	return FAIL;
    } else {
        queue_get(&pThis->queue, (void**)ppChunk);
        return PASS;
    }
}



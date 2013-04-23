#include <tll_common.h>
#include <bf52xI2cMaster.h>
#include "queue.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <extio.h>
#include <tll6527_core_timer.h>
#include "startup.h"
#include "wc.h"

bufferPool_t bp;
isrDisp_t isrDisp;
chunk_t *chunk;
wc_t wc;

int main ( void )
{
    int                         status                  = 0;

    /* Blackfin setup function to configure processor */	
    status = blackfin_setup(); //returns 0 if successful and -1 if failed
    if (status) {
        printf("\r\n Blackfin Setup Failed"); 
        return -1;
    }
    
    /* FPGA setup function to configure FPGA, make sure the FPGA configuration
    binary data is loaded in to SDRAM at "FPGA_DATA_START_ADDR" */
    status = fpga_loader(0x3E70BE61);
    //status = fpga_loader(0x3F6CF620); //returns 0 if successful and -1 if failed
    if (status) {
        printf("\r\n FPGA Setup Failed"); 
        return -1;
    }

    /* Initialize the interrupt Dispatcher */
    status = isrDisp_init(&isrDisp);
    if ( PASS != status ) {
        return FAIL;
    }

    /* Initialize the buffer pool */
    status = bufferPool_init(&bp);
    if ( PASS != status ) {
        return FAIL;
    }

    /* Initialize the extio */
    status = extio_init(&isrDisp);
    if ( PASS != status ) {
        return FAIL;
    }

    int i = 0;
    status = Wc_Init(&wc, &bp, &isrDisp);
    if ( PASS != status ) {
        return FAIL;
    }

    status = Wc_Start(&wc);
    if ( PASS != status ) {
        return FAIL;
    }

    chunk_t *new_chunk = NULL;
    bufferPool_acquire(&bp, &new_chunk);

    while (1)
    {
        Wc_Receive(&wc, new_chunk);
        if (new_chunk->bytesUsed > 0)
        {
            asm("nop;");
            bufferPool_release(&bp, new_chunk);
            new_chunk = NULL;
            bufferPool_acquire(&bp, &new_chunk);
        }
    }

    return PASS;
}

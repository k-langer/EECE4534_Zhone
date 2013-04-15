#include <tll_common.h>
#include <bf52xI2cMaster.h>
#include "queue.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <extio.h>
#include <tll6527_core_timer.h>
#include "startup.h"
#include "uartRx.h"
#include "uartTx.h"
#include "xbee.h"
#include "wire.h"

//xbee_t xbee;
uartRx_t uartRx;
uartTx_t uartTx;
bufferPool_t bp;
isrDisp_t isrDisp;
chunk_t *chunk;
wire_t wire;

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
    status = fpga_loader(0xF445850D);
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

    bf52x_uart_settings settings = {
        .parenable = 0,
        .parity = 0,
        .rxtx_baud = BF52X_BAUD_RATE_9600
    };

    bf52x_uart_deinit();
    bf52x_uart_init(&settings); 

    status = uartRx_init(&uartRx, &bp, &isrDisp);
    if ( PASS != status ) {
        return FAIL;
    }

    status = uartTx_init(&uartTx, &bp, &isrDisp);
    if ( PASS != status ) {
        return FAIL;
    }

    status = uartRx_start(&uartRx);
    if ( PASS != status ) {
        return FAIL;
    }

    //status = Xbee_Init(&xbee, &uartRx, &uartTx);
    status = Wire_Init(&wire, &uartRx, &uartTx, &bp);
    if ( PASS != status ) {
        return FAIL;
    }

    *pPORTF_FER |= 0xc000;
    *pPORTF_MUX &= ~0x0400;
    *pPORTF_MUX |= 0x0800;
    *pPORTFIO_DIR |= 0x4000;
    *pPORTFIO_DIR &= ~(0x8000);

    chunk_t *new_chunk = NULL;

    char data[9];

    //while (1)
    //{
    //    bufferPool_acquire(&bp, &new_chunk);

    //    unsigned char msg[] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f };
    //    for (i = 0; i < 5; i++)
    //    {
    //        new_chunk->u08_buff[i] = msg[i];
    //    }

    //    new_chunk->bytesUsed = 5;
    //    Wire_SendMessage(&wire, new_chunk);

    //    bufferPool_acquire(&bp, &new_chunk);

    //    while (FAIL == Wire_GetMessageNb(&wire, new_chunk))
    //    {
    //        i = 0;
    //        while (i++ < 150000);
    //    }

    //    bufferPool_release(&bp, new_chunk);
    //    new_chunk = NULL;
    //}
    //bufferPool_acquire(&bp, &new_chunk);
    
    //while (1)
    //{
    //    //bf52x_uart_receive(data, 9);
    //    //asm("nop;");
    //    uartRx_get(&uartRx, new_chunk);
    //    if (new_chunk->bytesUsed > 0)
    //    {
    //        asm("nop;");
    //        bufferPool_release(&bp, new_chunk);
    //        new_chunk = NULL;
    //        bufferPool_acquire(&bp, &new_chunk);
    //    }
    //}

    return 0;
}

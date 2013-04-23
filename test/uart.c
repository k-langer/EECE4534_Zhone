#include <tll_common.h>
#include <bf52xI2cMaster.h>
#include "queue.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <extio.h>
#include <tll6527_core_timer.h>
#include "startup.h"
//#include "fpga_gpio_uart.bin.h"
#include "uartRx.h"
#include "uartTx.h"
#include "wire.h"

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
    status = fpga_loader(0x3e70be61); //returns 0 if successful and -1 if failed
    //status = fpga_programmer((unsigned char*)fpga_gpio_uart_bin, sizeof(fpga_gpio_uart_bin));
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

    status = Wire_Init(&wire, &uartRx, &uartTx, &bp);
    if ( PASS != status ) {
        return FAIL;
    }

    status = uartRx_start(&uartRx);
    if ( PASS != status ) {
        return FAIL;
    }

    *pPORTF_FER |= 0xc000;
    *pPORTF_MUX &= ~0x0400;
    *pPORTF_MUX |= 0x0800;
    *pPORTFIO_DIR |= 0x4000;
    *pPORTFIO_DIR &= ~0x8000;

    chunk_t *new_chunk = NULL;
    char data[5] = {0};
    
    while (1)
    {
        //bf52x_uart_receive(data, 5);
        //int i = 0;
        //while (i++ < 300000);

        //uartRx_get(&uartRx, new_chunk);
        Wire_GetMessage(&wire, new_chunk);
        if (new_chunk->bytesUsed > 0)
        {
            asm("nop;");
            bufferPool_release(&bp, new_chunk);
            new_chunk = NULL;
            bufferPool_acquire(&bp, &new_chunk);
        }
    }

    return 0;
}

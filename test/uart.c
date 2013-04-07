#include <tll_common.h>
#include <bf52xI2cMaster.h>
#include "queue.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include <extio.h>
#include <tll6527_core_timer.h>
#include "startup.h"
//#include "uartRx.h"

//uartRx_t uartRx;
bufferPool_t bp;
isrDisp_t isrDisp;
chunk_t *chunk;

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
    status = fpga_loader(0x80373f23); //returns 0 if successful and -1 if failed
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

    printf("UART1_LCR: %04x\n", *pUART1_LCR);

    int i = 0;
    while (i++ < 750000);

    //bf52x_uart_settings settings = {
    //    .parenable = 0,
    //    .parity = 0,
    //    .rxtx_baud = BF52X_BAUD_RATE_9600
    //};

    //bf52x_uart_deinit();
    //bf52x_uart_init(&settings); 
    *pPORTF_FER |= 0xc000;
	*pPORTF_MUX &= ~0x0400;
	*pPORTF_MUX |= 0x0800;
    *pPORTFIO_DIR &= ~(0x4000);

    i = 0;
    while (i++ < 750000);

    //status = uartRx_init(&uartRx, &bp, &isrDisp);
    //if ( PASS != status ) {
    //    return FAIL;
    //}

    //status = uartRx_start(&uartRx);
    //if ( PASS != status ) {
    //    return FAIL;
    //}

    char data[100] = {0};
    printf("D\n");
    while (1) {
        status = bf52x_uart_receive(data, 1);
        printf("Data: %s\n", data);
    }

    i = 0;

    return 0;
}

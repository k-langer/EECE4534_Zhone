#include <stdio.h>
#include "ssm2602.h"
#include <tll_config.h>
#include <sys/exception.h>
#include <bf52x_uart.h>
#include <bf52xI2cMaster.h>
#include <tll6527_core_timer.h>
#include "fpga_gpio_uart.bin.h"
#include "startup.h"
#include "tll_sport.h"
#include "audioTx.h"
#include "audioRx.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include "chunk.h"
#include "encoder.h"
#include "decoder.h"
#include "wc.h"

bufferPool_t bufferPool;
isrDisp_t isrDisp;
chunk_t* testChunk;
chunk_t* dataChunk;
chunk_t* readyChunk;
audioRx_t testInput;
audioTx_t testOutput;
encoder_t encoder;
decoder_t decoder;
wc_t wc;


#define I2C_CLOCK   (400*_1KHZ)

//audio tx/rx testing
int main(void)
{
    int                         status                  = -1;

    /* Blackfin setup function to configure processor */
    status = blackfin_setup(); //returns 0 if successful and -1 if failed
    if (status) {
        printf("\r\n Blackfin Setup Failed");
        return -1;
    }

    /* FPGA setup function to configure FPGA, make sure the FPGA configuration
    binary data is loaded in to SDRAM at "FPGA_DATA_START_ADDR" */
    //status = fpga_setup(); //returns 0 if successful and -1 if failed
    status = fpga_programmer((unsigned char *)fpga_gpio_uart_bin, sizeof(fpga_gpio_uart_bin));
    if (status) {
        printf("\r\n FPGA Setup Failed");
        return -1;
    }

    printf("hello world\n");

    bufferPool_init( &bufferPool );
    isrDisp_init( &isrDisp );

    bf52xI2cMaster_init(0, I2C_CLOCK);
    status = ssm2602_init( &isrDisp, 0x2F, SSM2602_SR_8000/2, SSM2602_TX );
    coreTimer_init();
	if (PASS != status) {
		printf("SSM2602 init failed\r\n");
		return status;
	}
    encoder_t encoder; 
    decoder_t decoder;
    audioTx_init( &testOutput, &bufferPool, &isrDisp );
    audioRx_init( &testInput, &bufferPool, &isrDisp );
    encoder_init( &encoder );
    decoder_init( &decoder, encoder.nbBytes );
    Wc_Init( &wc, &bufferPool, &isrDisp );

    audioRx_start( &testInput );
    audioTx_start( &testOutput );
    //Wc_Start( &wc );

    //bf52x_uart_settings settings = {
    //    .parenable = 0,
    //    .parity = 0,
    //    .rxtx_baud = BF52X_BAUD_RATE_9600
    //};    
    //      
    //bf52x_uart_deinit();
    //bf52x_uart_init(&settings); 

    *pPORTF_FER |= 0xc000;
    *pPORTF_MUX &= ~0x0400;
    *pPORTF_MUX |= 0x0800;
    *pPORTFIO_DIR |= 0x4000;
    *pPORTFIO_DIR &= ~(0x8000);

    chunk_t* dataChunk = malloc(sizeof(chunk_t));
    //chunk_t* readyChunk = malloc(sizeof(chunk_t));

    while( 1 ) {
    	if ( audioRx_getNbNc( &testInput, &testChunk) == PASS ) {
            bufferPool_acquire( &bufferPool, &dataChunk );
    		encoder_encode( &encoder, testChunk, dataChunk );
            bufferPool_release( &bufferPool, testChunk );
            //if (FAIL == Wc_Send( &wc, dataChunk ))
            //    break;

            //Wc_Start( &wc );
            //bufferPool_acquire( &bufferPool, &readyChunk );
            //while (FAIL == Wc_Receive( &wc, readyChunk ));
            //Wc_Stop( &wc );
            //bufferPool_acquire( &bufferPool, &dataChunk );
    		decoder_decode( &decoder, dataChunk, readyChunk );
            bufferPool_release( &bufferPool, dataChunk );
            audioTx_put( &testOutput, readyChunk );
            bufferPool_release( &bufferPool, readyChunk );
		}
    }

    return 0;
}

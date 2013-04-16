#include <stdio.h>
#include "ssm2602.h"
#include <tll_config.h>
#include <sys/exception.h>
#include <bf52xI2cMaster.h>
#include <tll6527_core_timer.h>
#include "startup.h"
#include "tll_sport.h"
#include "audioTx.h"
#include "audioRx.h"
#include "bufferPool.h"
#include "isrDisp.h"
#include "chunk.h"
#include "encoder.h"
#include "decoder.h"

bufferPool_t bufferPool;
isrDisp_t isrDisp;
chunk_t* testChunk;
chunk_t* dataChunk;
chunk_t* readyChunk;
audioRx_t testInput;
audioTx_t testOutput;
encoder_t encoder;
decoder_t decoder;

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
    status = fpga_setup(); //returns 0 if successful and -1 if failed
    if (status) {
        printf("\r\n FPGA Setup Failed");
        return -1;
    }

    printf("hello world\n");

    bufferPool_init( &bufferPool );
    isrDisp_init( &isrDisp );
    chunk_init( testChunk );
    chunk_init( dataChunk );
    chunk_init( readyChunk );

    bf52xI2cMaster_init(0, I2C_CLOCK);
    status = ssm2602_init( &isrDisp, 0x2F, SSM2602_SR_8000/2, SSM2602_TX );
    coreTimer_init();
	if (PASS != status) {
		printf("SSM2602 init failed\r\n");
		return status;
	}

    audioTx_init( &testOutput, &bufferPool, &isrDisp );
    audioRx_init( &testInput, &bufferPool, &isrDisp );
    encoder_init( &encoder );
    decoder_init( &decoder, encoder.nbBytes );

    audioRx_start( &testInput );
    audioTx_start( &testOutput );

    while( 1 ) {
    	if ( audioRx_getNbNc( &testInput, &testChunk) == PASS ) {
    		encoder_encode( &encoder, testChunk, dataChunk );
    		decoder_decode( &decoder, dataChunk, readyChunk );
            audioTx_put( &testOutput, readyChunk );
		}
    }

    return 0;
}

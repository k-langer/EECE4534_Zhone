#include <stdio.h>
#include <tll_config.h>
#include <sys/exception.h>
#include <bf52x_uart.h>
#include <bf52xI2cMaster.h>
#include <tll6527_core_timer.h>
#include "fpga_gpio_uart.bin.h"
#include "startup.h"
#include "tll_sport.h"
#include "zhone.h"

zhone_t zhone;


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

    bf52xI2cMaster_init(0, I2C_CLOCK);
    coreTimer_init();
	if (PASS != status) {
		printf("SSM2602 init failed\r\n");
		return status;
	}

    zhone_init( &zhone );
    zhone_run( &zhone );

    return 0;
}

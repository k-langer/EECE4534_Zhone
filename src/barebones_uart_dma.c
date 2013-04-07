/*
 * barebones_uart_dma.c
 *
 *  Created on: Apr 22, 2012
 *      Author: schirner
 */

#include <cdefBF527.h>
#include <stdio.h>
#include <sys/exception.h>
#include "startup.h"

#define MSG_SIZE 10

char tx_buff[MSG_SIZE] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', '\r', '\n'};
char rx_buff[MSG_SIZE] __attribute__((section(".sram_app.data")));

typedef struct {
	unsigned int txDone, txErr;
	unsigned int rxDone, rxErr;
	unsigned int misc;
	unsigned int rxRead;
} tCount;

tCount gCount = {0,0,0,0,0,0};

void uart_ISR()__attribute__((interrupt_handler));

void uartRx_DMAConfig(void) {
	// set address, count, step, config = interrupt enable | sync | 8 bits | write | dma enable
	*pDMA10_CONFIG = 0x0;
	asm("ssync;");
	*pDMA10_START_ADDR = rx_buff;
	*pDMA10_X_COUNT = MSG_SIZE;
	*pDMA10_X_MODIFY = 1;
	*pDMA10_PERIPHERAL_MAP = 0xA000; // UART 1 RX DMA
	*pDMA10_IRQ_STATUS |= DMA_DONE | DMA_ERR; // clear interrupts potentially pending from before
	asm("ssync;");
	*pDMA10_CONFIG = 0x00A3;
	asm("ssync;");
}

int main() {

    int                         status                  = -1;

    /* Blackfin setup function to configure processor */
    status = blackfin_setup(); //returns 0 if successful and -1 if failed
    if (status) {
        printf("\r\n Blackfin Setup Failed");
        return -1;
    }


	// good values taken from isr handler -- they work
#if 0
	*pSIC_IMASK0 = 0x3000000;
	*pSIC_IMASK1 = 0x0;
	*pSIC_IMASK = 0x3000000;
	*pSIC_ISR0 = 0x0;
	*pSIC_ISR1 = 0x0;
	*pSIC_ISR = 0x0;
	*pSIC_IWR0 = 0x1;
	*pSIC_IWR1 = 0xFFFFFFFF;
	*pSIC_IWR = 0x1;
	*pSIC_IAR0 = 0x0;
	*pSIC_IAR1 = 0x11000000;
	*pSIC_IAR2 = 0x33332222;
	*pSIC_IAR3 = 0x44444433;
	*pSIC_IAR4 = 0x55555555;
	*pSIC_IAR5 = 0x6666655;
	*pSIC_IAR6 = 0x33333000;
	*pSIC_IAR7 = 0x66655555;
#else
	// enable interrupts for UART RX DMA , UART TX DMA
	//*pSIC_IMASK0 |= 0x03000000; // DMA11, DMA10 DMA
#endif
	asm("ssync;");


	printf("UART setup\r\n");
	//configure UART

#if 0 // uart already setup automatically (I believe otherwise prints would not work)
	// set bit to configure dlh, dll
	*pUART1_LCR = 0x0080;

	*pUART1_DLH = 0x0000;
	*pUART1_DLL = 0x0036;

	// turn off bit to configure dlh, dll, allowing access to other uart register
	*pUART1_LCR = 0x0000;

	// 8 bits, 1 stop bit, no parity
	*pUART1_LCR = 0x0003;

	// allow interrupts for both rx and tx
	*pUART1_IER = 0x0003;

	// turn on the uart
	*pUART1_GCTL = 0x0001;
#endif

	asm("ssync;");
	printf("DMA setup\r\n");

	uartRx_DMAConfig();
	// test -- hook up rx directly to tx
	//*pUART1_MCR = 0x10;



#if 0 // disable tx DMA we use printfs for it
	// set address, count, step, config = interrupt enable | sync | 8 bits | read | dma enable
	*pDMA11_CONFIG = 0x0;
	*pDMA11_START_ADDR = tx_buff;
	*pDMA11_X_COUNT = MSG_SIZE;
	*pDMA11_X_MODIFY = 1;
	*pDMA11_IRQ_STATUS |= DMA_DONE | DMA_ERR; // clear interrupts potentially pending from before
	printf("About to launch\r\n");
	*pDMA11_CONFIG = 0x00A1;

	asm("ssync;");
#endif

	// allow interrupts for both rx and tx
	// needed as they double as DMA handshake lines
	*pUART1_IER |= (ERBFI | ETBEI) ;

	// hook up isr
	register_handler(ik_ivg10, uart_ISR);
	// enable interrupts for UART RX DMA , UART TX DMA
	*pSIC_IMASK0 |= 0x03000000; // DMA11, DMA10 DMA

	int rxDonePrev = 0;
	while(1){

		// wait until rx DMA int came
		while(gCount.rxDone == rxDonePrev) {
			asm("idle;");
		}
		rxDonePrev = gCount.rxDone;

		// print received chars
		printf("Received %d chars\r\n", MSG_SIZE);

		int i;
		for (i = 0; i < MSG_SIZE; i++) {
			printf("%d: %c 0x%X\r\n", i, rx_buff[i], rx_buff[i]); // debug... all 0x0, bah
		}
		printf("\r\n");
		// print coutners
		printf("txDone %d, txErr %d, rxDone %d, rxErr %d, misc %d, rxRead %d\r\n",
				gCount.txDone, gCount.txErr, gCount.rxDone, gCount.rxErr, gCount.misc, gCount.rxRead);

		// setup for next receive run
		uartRx_DMAConfig();
	}
	while(1) {
		asm("idle;");
	}

	return 0;
}

void uart_ISR() {

	if(*pDMA10_IRQ_STATUS & DMA_DONE) {
		gCount.rxDone++;
		// clear int
		*pDMA10_IRQ_STATUS |= DMA_DONE;
		// stop dma
		*pDMA10_CONFIG = 0x0000;

	} else if (*pDMA10_IRQ_STATUS & DMA_ERR) {
		gCount.rxErr++;
		// clear int
		*pDMA10_IRQ_STATUS |= DMA_ERR;
		// stop dma
		*pDMA10_CONFIG = 0x0000;
	} else if(*pDMA11_IRQ_STATUS & DMA_DONE) {
		gCount.txDone++;
		// clear interrupt
		*pDMA11_IRQ_STATUS |= DMA_DONE;
		// disable DMA
		*pDMA11_CONFIG = 0x0000;
	} else if (*pDMA11_IRQ_STATUS & DMA_ERR) {
		gCount.txErr++;
		// clear interrupt
		*pDMA11_IRQ_STATUS |= DMA_ERR;
		// disable DMA
		*pDMA11_CONFIG = 0x0000;
	} else if (*pUART1_IIR & 0x1) {
		gCount.rxRead++;
		//printf("UART read?\r\n"); // gets rid of interrupt on tx side
	}
	else
	{
		// not sure who triggers interrupt here
		// maybe due to printfs
		gCount.misc++;
		//printf("NA?\r\n");
		//*pDMA11_CONFIG = 0x0000;
		//*pDMA10_CONFIG = 0x0000;
	}

	//printf("\r\n Interrupt \r\n");
	//*pUART1_IER = 0x0003;
}


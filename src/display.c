#include "display.h"
#include "commonTypes.h"

//These libraries were taken from Lab1, probably needed
//#include "ADP5588_Driver.h"
//#include <gpio.h>


//#include "startup.h"
/***
	Display
	  - Offers the a menu to the user via the LEDs
	  - Sets the LEDs in the appropriate location when called
	  - See display.h for more detailed documentation
***/

/***
	Declaration of registers

***/
volatile unsigned short *pPORTF_FER = (unsigned short*)0xFFC03200;
volatile unsigned short *pPORTFIO_DIR = (unsigned short*)0xFFC00730;
volatile unsigned short *pPORTFIO_INEN = (unsigned short*)0xFFc00740;
volatile unsigned short *pPORTFIO = (unsigned short*)0xFFC00700;

//volatile unsigned short *pTIMER_ENABLE = (unsigned short*) 0xF;
//volatile unsigned short *pTIMER_DISABLE = (unsigned short*) 0xF;
//volatile unsigned short *pTIMER_CONFIG = (unsigned short*) 0xF;
//volatile unsigned short *pTIMER_PERIOD = (unsigned short*) 0xF;



void display_init() {
	*pPORTF_FER |= 0x00;		  //LED is lower 8 bits of portF
	*pPORTFIO_DIR |= 0XFF;

}


void display_mainMenu() {
	//Enable LED0/LED7
	*pPORTFIO &= 0x00;
	busy_wait(10000000);
	*pPORTFIO |= 0x81;
	busy_wait( 10000000 );

	//return PASS;
}

void display_receiveMenu1() {


	*pPORTFIO &= 0x00;
	busy_wait( 10000000 );
	*pPORTFIO |= 0x80;
	busy_wait( 10000000 );

	//return PASS;
}

void display_receiveMenu2() {


	*pPORTFIO &= 0x00;
	busy_wait( 10000000 );
	*pPORTFIO |= 0x01;
	busy_wait( 10000000 );

	//return PASS;
}


void display_makingCallMenu() {

    *pPORTFIO &= 0x00;
	busy_wait( 10000000 );
	*pPORTFIO |= 0xFF;
	busy_wait( 10000000 );

    //return PASS;
}


void display_inCallMenu() {

    *pPORTFIO |= 0xFF;
	//return PASS;
}



//Instead of busy wait use the GPIO tiemrs
//Timer1 blink every











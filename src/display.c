#include "display.h"
#include "ADP5588_Driver.h" 
#include "startup.h"
#include "extio.h"

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



void display_init() {
	extio_ledConfig( EXTIO_LED0 );
	extio_ledConfig( EXTIO_LED1 );
	extio_ledConfig( EXTIO_LED2 );
	extio_ledConfig( EXTIO_LED3 );
	extio_ledConfig( EXTIO_LED4 );
	extio_ledConfig( EXTIO_LED5 );
	extio_ledConfig( EXTIO_LED6 );
	extio_ledConfig( EXTIO_LED7 );
}


void display_mainMenu() {
	extio_ledGroupOff( 0xFF );
	extio_ledGroupOn( 0x81 );
}

void display_receiveMenu1() {
	extio_ledGroupOff( 0xFF );
	extio_ledGroupOn( 0xEF );
}

void display_receiveMenu2() {
	extio_ledGroupOff( 0xFF );
	extio_ledGroupOn( 0xDF );
}


void display_makingCallMenu() {
    extio_ledGroupOff( 0xFF );
    extio_ledGroupOn( 0xC3 );
}


void display_inCallMenu() {
	extio_ledGroupOff( 0xFF );
	extio_ledGroupOn( 0xE7 );
}

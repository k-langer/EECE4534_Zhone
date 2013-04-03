#include "display.h"
#include "commontypes.h" 
#include "phonebook.h"  

//These libraries were taken from Lab1, probably needed  
//#include "ADP5588_Driver.h"
//#include "startup.h"
//#include <gpio.h>

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

return_value_t display_init() { 
 
	*pPORTF_FER &= 0x00;		  //LED is lower 8 bits of portF
	*pPORTFIO_DIR &= 0XFF; 
	*pPORTFIO_INEN &= 0x00;  
	
	display_mainMenu();           //Display the main menu when initialized 

	return PASS; 
}


return_value_t display_mainMenu() { 
	//Enable LED0/LED7 
	*pPORTFIO &= 0x00; 
	busy_wait(10000000);
	*pPORTFIO |= 0x81;
	busy_wait( 10000000 );

	return PASS;
}

return_value_t display_receiveMenu1() { 
	 
	
	*pPORTFIO &= 0x00;
	busy_wait( 10000000 );
	*pPORTFIO |= 0x10; 
	busy_wait( 10000000 );

	return PASS;
} 

return_value_t display_receiveMenu2() {

	
	*pPORTFIO &= 0x00; 	
	busy_wait( 10000000 );
	*pPORTFIO |= 0x01; 
	busy_wait( 10000000 );

	return PASS; 
}


return_value_t display_makingCallMenu() {	
	
    *pPORTFIO &= 0x00; 
	busy_wait( 10000000 );
	*pPORTFIO |= 0xFF; 
	busy_wait( 10000000 );

    return PASS;
} 


return_value_t display_inCallMenu() { 

    *pPORTFIO |= 0xFF; 
	return PASS;
}


void busy_wait(unsigned int cycles) 
{
	while ( cycles-- )
	{
			//asm("nop;");
			//asm("ssync;"); 
	}
}

/*
int main() { //for testing purposes 
	display_init(); 
	display_mainMenu();
	display_receiveMenu1();
	display_receiveMenu2();
	display_makingCallMenu(); 
	display_inCallMenu(); 
} 
*/











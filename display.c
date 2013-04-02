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

return_value_t Display_init() { 
	//LED is lower 8 bits of portF 
	*pPORTF_FER &= 0x00; 
	*pPORTFIO_DIR &= 0XFF; 
	*pPORTFIO_INEN &= 0x00;  
	
	Display_mainMenu(); //Display the main menu when initialized 

	return PASS; 
}


return_value_t Display_mainMenu() { 
	//Enable LED0/LED7 
	*pPORTFIO &= 0x00; 
	busy_wait(10000000);
	*pPORTFIO |= 0x81;

	return PASS;
}

return_value_t Display_receiveMenu(phonebook_t * pThis, zhone_address_t address1, zhone_address_t address2) { 
	 
	if( address1 ) { 
	*pPORTFIO &= 0x00;
	busy_wait( 10000000 );
	*pPORTFIO |= 0x10; 
	

	} else if( address2 ) {
	*pPORTFIO &= 0x00; 	
	busy_wait( 10000000 );
	*pPORTFIO |= 0x01; 
	}
	busy_wait( 10000000 );
	
	
	return PASS;
} 


return_value_t Display_makingCallMenu() {	
	
    *pPORTFIO &= 0x00; 
	busy_wait( 10000000 );
	*pPORTFIO |= 0xFF; 

    return PASS;
} 


return_value_t Display_inCallMenu() { 

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


int main() { //test each function individually 
	Display_init(); 
	Display_mainMenu;
	//Display_receiveMenu(*pThis, address1, address2); ???????
	Display_makingCallMenu(); 
	Display_inCallMenu(); 
} 












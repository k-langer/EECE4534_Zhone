#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "commonTypes.h"

/***
    Display
        - Offers the a menu to the user via the LEDs 
		- Sets the LEDs in the appropriate location when called 
		- Address 1 corresponds to LED0 
		- Address 2 corresponds to LED7 
***/



/***
	init 

	Initializes the LEDs 
	
	returns PASS on success or FAIL on failure 
***/

return_value_t display_init();

/***

	mainMenu

	Displays the main  Menu
	Turns on LEDs 0 and 7 
***/

return_value_t display_mainMenu(); 

/***
	receiveMenu

	Blinks the LED that is calling the user on and off 
	User 1 corresponds to the leftmost LED, User 2 corresponds to the rightmost LED
***/

return_value_t display_receiveMenu1();

return_value_t display_receiveMenu2();


/***
	makingCallMenu

	Blinks all the LEDs on and off to indicate that you are making a call 	
***/

return_value_t display_makingCallMenu();

/***
	inCallMenu

	Hold all of the LEDs on to indicate that you are in a call 
***/

return_value_t display_inCallMenu(); 


/***
	busy_wait

	Function taken directly from Lab1 to add a small delay to the LEDs 
***/
void busy_wait(unsigned int); 



#endif 

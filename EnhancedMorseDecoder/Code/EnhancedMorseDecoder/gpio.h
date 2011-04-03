/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	gpio.h
Created on:	3/26/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/26/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Cointains a functions to work with the GIPO pins on the Morse Decoder PCB:
		PORTB5 - Digital Morse Code ouptut
-------------------------------------------------------------------------------------------------
REFRENCES:
	Section 10. I/O Ports - page 4 for latch vs. port register usage

-----------------------------------------------------------------------------------------------*/
#ifndef GPIO_H_
#define GPIO_H_

#include "p33fxxxx.h" // includes the correct header for processor being used

//example usage:
//to turn the morse code output on:	CODE_OUTPUT = CODE_ON;

//----- DEFINES: -----
#define DETECTED		0 //reversed for active low output
#define NOT_DETECTED 	1
#define GPIO_OUTPUT		0 //used to set the TRIS to an ouput

#define CODE_TRIS		_TRISB5 // define the tris (data direction reg) for CODE_OUTPUT

#define CODE_OUTPUT		_LATB5 // define LED1 to the latch register for port b5

//----- Function Prototypes -----
void init_gpio(void);

#endif /* GPIO_H_ */


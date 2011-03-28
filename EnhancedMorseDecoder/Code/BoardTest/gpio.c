/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	gpio.c
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

#include "gpio.h"

/******************************************************************************
 * Name: init_gpio
 * Inputs: n/a
 * Outputs: n/a 
 *
 * Description: sets portb5 (digital morse code output) as an output.
 *              initalizes the output value to code not detected
 *****************************************************************************/
void init_gpio(void){

	// set the LED ports to outputs
	CODE_TRIS = GPIO_OUTPUT;
	
	// initialize it to off
	CODE_OUTPUT = NOT_DETECTED;
}

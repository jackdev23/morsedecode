/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	oscillator.c
Created on:	3/13/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/13/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Cointains a functions to initialize the microcontrollers clock to come from the primary
	oscillator in XT mode. the decoder board has an 8 Mhz crystal and this will configure it to
	run the system clock at 80Mhz (instructions at 40 MIPS).
	
	The main file should have set up the Internal FRC at POR, Clock Switching and 
	Posc configured in XT mode
-------------------------------------------------------------------------------------------------
REFRENCES:
	Section 7. Oscillator - page 23 for example code and preceding section for equations
-----------------------------------------------------------------------------------------------*/

#include "oscillator.h" // header for oscillator functions

void osc_init(void){
	
	// Configure PLL prescaler, PLL postscaler, PLL divisor
	PLLFBD=30; 				// M = 32
	CLKDIVbits.PLLPOST = 0;	// N2 = 2
	CLKDIVbits.PLLPRE = 0; 	// N1 = 2
	
	// Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
	__builtin_write_OSCCONH(0x03);
	__builtin_write_OSCCONL(0x01);
	
	// Wait for Clock switch to occur
	while (OSCCONbits.COSC! = 0b011);
	
	// Wait for PLL to lock
	while(OSCCONbits.LOCK! = 1) {};
}	

/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	oscillator.h
Created on:	3/13/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/13/2011 - Dylan Thorner - Original release
	2: 03/25/2011 - Dylan Thorner - Added preprocessor directives to prevent file from being 
									included twice
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Cointains a functions to initialize the microcontrollers clock to come from the primary
	oscillator in XT mode. the decoder board has an 8 Mhz crystal and this will configure it to
	run the system clock at 80Mhz (instructions at 40 MIPS).
	
	Designed to work with the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.
	
	The main file should have set up the Internal FRC at POR, Clock Switching and 
	Posc configured in XT mode
-------------------------------------------------------------------------------------------------
REFRENCES:
	Section 7. Oscillator - page 23 for example code and preceding section for equations
-----------------------------------------------------------------------------------------------*/
#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_


#include "p33fxxxx.h" // includes correct header for processor


// Function protypes
void init_osc(void);


#endif  //OSCILLATOR_H_



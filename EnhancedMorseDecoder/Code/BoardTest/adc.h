/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	adc.h
Created on:	3/25/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/25/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Header file for functions to work with the ADC.
	
	Designed to work with the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.
	
-------------------------------------------------------------------------------------------------
REFRENCES:
	Section 16. Analog-to-Digital Converter (ADC)
	
	Example project for ADC configuration: CE100_ADC_IIR_Filter_14aug09 
		from http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en532298
-----------------------------------------------------------------------------------------------*/

#ifndef ADC_H_
#define ADC_H_

#include "p33fxxxx.h" // includes the correct header for the processor being used

// Defines to set up sampling frequency
#define Fosc		80000000  	// 80MHz clock
#define Fcy			(Fosc/2)	// Internal speed
#define Fs   		2100		// sampling frequency
#define SAMPPRD    (Fcy/Fs)-1	// Timer Compare register value for sampling period

// Function prototypes
void init_adc(void);

#endif /* ADC_H_ */

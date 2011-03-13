/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	led.h
Created on:	3/12/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/12/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Cointains a functions to deal with the LEDs for the morse decoder PCB.
	PORTB10 -> LED1
	PORTB11 -> LED2
	
-------------------------------------------------------------------------------------------------
REFRENCES:
	Section 10. I/O Ports - page 4 for latch vs. port register usage
	
	SASK WM8510 Record Play Demo with Intro - how to define LEDs and TRIS registers for easy use
-----------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" // includes correct header for processor


//example usage:
//to turn LED1 on:	LED1 = LED_ON;

//----- DEFINES: -----

#define LED_ON 		1
#define LED_OFF 	0
#define OUTPUT_PORT	0 //used to set the TRIS to an ouput

#define LED1_TRIS	_TRISB10 // define the tris (data direction reg) for LED1
#define LED2_TRIS	_TRISB11 // define the tris (data direction reg) for LED2

#define LED1 		_LATB10 // define LED1 to the latch register for port b10
#define LED2 		_LATB11 // define LED2 to the latch register for port b11



//----- Function Prototypes -----

void init_leds(void);






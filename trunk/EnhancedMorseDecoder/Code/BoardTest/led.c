/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	led.c
Created on:	3/12/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/12/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Cointains a functions to deal with the LEDs for the morse decoder PCB.
	PORTB10 - LED1
	PORTB11 - LED2
------------------------------------------------------------------------------------------------*/

#include "led.h" //include the files to work with the LEDs

/******************************************************************************
 * Name: init_leds
 * Inputs: n/a
 * Outputs: n/a 
 *
 * Description: sets portb10 (led1) and portb11 (led2) as outputs.
 *              initalizes the output value to 0 (LED off)
 *****************************************************************************/
void init_leds(void){

	// set the LED ports to outputs
	LED1_TRIS = OUTPUT_PORT;
	LED2_TRIS = OUTPUT_PORT;
	
	// initialize them both to off
	LED1 = LED_OFF;
	LED2 = LED_OFF;
}

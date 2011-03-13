/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	main.c
Created on:	3/12/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/12/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	main file for project.
	intended to test the  dsPIC33FJ128GP802 microcontroller on the enhanced morse decoder PCB.
	
	This program provides the following test outputs:
		LED 2 is always on (check LED and that it was programmed)
		LED 1 is flashing at ???? Hz (check LED and oscillator)
		2nd to leftmost pin on X5 is a ??? kHz square wave (check oscillator)
		ADC is converting from pin 2 and storeing samples in ??? (check ADC operation)
------------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" // includes correct header for processor
//#include "dsp.h" // include microchip DSP routines

#include "led.h" //include the files to work with the LEDs



int main (void){
	
	int i = 0;
	
	// initialize the LEDs 
	init_leds();
	// initialize the oscilator
	
	
	
	// turn on the second LED to verify functionality
	LED2 = LED_ON;
	
	

	// Loop Forever
	while(1){
		
		// no nothing - delay
		for(i=0; i<32767; i++){}
		// no nothing - delay
		for(i=0; i<32767; i++){}
		// no nothing - delay
		for(i=0; i<32767; i++){}
		
		LED1 = LED_OFF;
		
		// no nothing - delay
		for(i=0; i<32767; i++){}
		// no nothing - delay
		for(i=0; i<32767; i++){}
		// no nothing - delay
		for(i=0; i<32767; i++){}
		
		LED1 = LED_ON;
	}
	
	
	
	
	
	return 0;
}

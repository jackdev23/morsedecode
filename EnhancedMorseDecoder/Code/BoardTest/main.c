/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	main.c
Created on:	3/12/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/12/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Main file for the project to test the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.
	
	This program provides the following test outputs:
		LED 2 is always on (check LED and that it was programmed)
		LED 1 is flashing at ???? Hz (check LED and oscillator)
		2nd to leftmost pin on X5 is a ??? kHz square wave (check oscillator)
		ADC is converting from pin 2 and storeing samples in ??? (check ADC operation)
------------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" // includes correct header for processor
//#include "dsp.h" // include microchip DSP routines

#include "led.h" //include the files to work with the LEDs
#include "oscillator.h" // file to initialize the oscillator


// Device Configuration

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching(FSCM is disabled), OSC2 Pin set to output clk (will be ignored b/c used for oscillator), 
//and Configure Posc in XT mode
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);

// Watchdog Timer Enabled/disabled by user software (will stay disabled)
_FWDT(FWDTEN_OFF);


int main (void){
	
	// used to delay for LED blinking
	int i = 0;
	int j = 0;
	
	//////////////////////////////
	//      Initialization      //
	//////////////////////////////
	
	// initialize the LEDs 
	init_leds();
	// initialize the oscilator and swich from FRC to oscillator
	init_osc();
	
	
	
	// turn on the second LED to verify functionality
	LED2 = LED_ON;
	
	

	// Loop Forever
	while(1){
		
		// no nothing - delay
		for(i=0; i<100; i++){
			for(j=0; j<32767; j++){}
		}		
		LED1 = LED_OFF;
		
		// no nothing - delay
		for(i=0; i<100; i++){
			for(j=0; j<32767; j++){}
		}
		
		LED1 = LED_ON;
	}
	
	
	
	
	
	return 0;
}



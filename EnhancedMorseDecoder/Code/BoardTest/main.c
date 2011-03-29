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
		LED 1 is flashing (to check LED and oscillator)
		2nd to leftmost pin on X5 is a 1050 Hz square wave, toggled when a sample is taken 
																			(to check oscillator)
		ADC is converting from pin 2 and storeing samples in ??? (check ADC operation)
------------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" // includes correct header for processor
//#include "dsp.h" // include microchip DSP routines

#include "led.h" //include the files to work with the LEDs
#include "oscillator.h" // file to initialize the oscillator
#include "gpio.h" // file to work with the GPIO pins (digital morse code output)



#define FLASH_LED // Define this to have LED 1 flash and LED 2 on, else comment out



// Device Configuration

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching(FSCM is disabled), OSC2 Pin set to output clk (will be ignored b/c used for oscillator), 
//and Configure Posc in XT mode
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);

// Watchdog Timer Enabled/disabled by user software (will stay disabled)
_FWDT(FWDTEN_OFF);


int main (void){
	
	//////////////////////////////
	//        Variables         //
	//////////////////////////////
	
	// used to delay for LED blinking
	int i = 0;
	int j = 0;
	//fractional[] adc_vals; // past ??? samples taken by the ADC
	
	
	
	//////////////////////////////
	//      Initialization      //
	//////////////////////////////
	init_osc();		// initialize the oscilator and swich from FRC to oscillator
	init_leds();	// initialize the LEDs 
	init_gpio();	// initialize the GPIO pin
	//init_adc();		// initialize the ADC
	
	
	
	//Disable the code that flashes the LED if not needed
	#ifdef FLASH_LED
		LED2 = LED_ON;// turn on the second LED to verify functionality
	#endif /* FLASH_LED  */
	

	// Loop Forever
	while(1){
		
		//Disable the code that flashes the LED if not needed
		#ifdef FLASH_LED
		
		// no nothing - delay
		for(i=0; i<60; i++){
			for(j=0; j<32767; j++){}
		}		
		LED1 = LED_OFF;
		
		// no nothing - delay
		for(i=0; i<100; i++){
			for(j=0; j<32767; j++){}
		}
		
		LED1 = LED_ON;
		
		#endif /* FLASH_LED */
	}
	
	
	
	
	
	return 0;
}



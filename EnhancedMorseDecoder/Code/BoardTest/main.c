/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	main.c
Created on:	3/12/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/12/2011 - Dylan Thorner - LEDs, oscillator etc.
	2: 04/01/2011 - Dylan Thorner - ADC testing 
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Main file for the project to test the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.
	
	This program provides the following test outputs:
		LED 2 is lit when voltage on ADC exceeds 1.65 V (check LED and ADC configuration)
		LED 1 is toogleing at 1050Hz (to check LED, oscillator, and Fs)
		ADC is converting from pin 2 and storeing samples in adc_vals (check ADC operation)
------------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" // includes correct header for processor
#include "dsp.h" // include microchip DSP routines, Must also include libdsp-coff.a in project

#include "led.h" //include the files to work with the LEDs
#include "oscillator.h" // file to initialize the oscillator
#include "gpio.h" // file to work with the GPIO pins (digital morse code output)
#include "adc.h" // file to define ADC parameters


//#define FLASH_LED // Define this to have LED 1 flash and LED 2 on, else comment out
#define NUM_ADC_VALS 100 // number of past ADC values to be stored



// Global Variables //
fractional adc_vals [NUM_ADC_VALS]; // past samples taken by the ADC



//////////////////////////
// Device Configuration //
//////////////////////////
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
	//int i = 0;
	//int j = 0;
	
	
	
	//////////////////////////////
	//      Initialization      //
	//////////////////////////////
	init_osc();		// initialize the oscilator and swich from FRC to oscillator
	init_leds();	// initialize the LEDs 
	init_gpio();	// initialize the GPIO pin
	init_adc();		// initialize the ADC
	

	// Loop Forever
	while(1){
		/* Initial code for testing LEDs:
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
		
		#endif // FLASH_LED 
		*/
	}
	
	
	
	
	
	return 0;
}


/*=============================================================================
_ADC1Interrupt():ADC Conversion is complete  
				ISR name is chosen from the device linker script.
				List of names in MPLAB C30 Compiler User's Guide (DS51284)
=============================================================================*/
/******************************************************************************
 * Name: _ADC1Interrupt
 * Inputs: n/a
 * Outputs: n/a 
 *
 * Description: This version of the ISR is for testing only. it will toogle 
 *	LED1 each sample, and light LED2 if the sample is over 1.65 V (0.5)
 *****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	fractional sample; // sample
	fractional thres = Float2Fract(0.5);
	static int index = 0; // index into the adc_vals array

	// get value from ADC register - ADC1BUF0
    // this assumes that the ADC is configured to output fractionl type
	sample = ADC1BUF0;
	
	// For sampling rate testing, toggle LED1 each sample
	LED1 = LED1 ^ 1; // test this.
	
	// For ADC testing, compare to a threshold and light LED2 if samp > thres
	if(sample > thres){		
		LED2 = LED_ON;
	} else{
		LED2 = LED_OFF;
	}
	
	// Add sample to ADC buffer
	adc_vals[index] = sample;
	index++;
	//check if end of buffer was reached
	if(index == NUM_ADC_VALS){
		index = 0;
	}	

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
}

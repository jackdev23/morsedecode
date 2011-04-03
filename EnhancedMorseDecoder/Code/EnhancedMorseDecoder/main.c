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
	Main file for the project for the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.

------------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" 	// includes correct header for processor
#include "dsp.h" 		// include microchip DSP routines, Must also include libdsp-coff.a in project

#include "led.h" 		// include the files to work with the LEDs
#include "oscillator.h" // file to initialize the oscillator
#include "gpio.h" 		// file to work with the GPIO pins (digital morse code output)
#include "adc.h" 		// file to define ADC parameters
#include "decoder_dsp.h"// DSP routine


#define FLASH_LED 		// Define this to have LED 1 flash and LED 2 on, else comment out
//#define DEBUG_MODE 1	// Define this to create a sinusiod rather that get sames from ADC
						//select Debugger --> Select Tool --> MPLAB SIM

//////////////////////
// Global Variables //
//////////////////////
FIRStruct filterI; 	// struct used by the FIR Filter function for real data
FIRStruct filterR; 	// struct used by the FIR Filter function for img data
volatile float proc_samp;	// the sample after it has been processed by the DSP routine
volatile float prev_proc_samp [500]; // past processed samples for debugging
static const float detect_thres = 0.0012; // threshold for detectiong Morse code


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
	//      Initialization      //
	//////////////////////////////
	init_osc();				// initialize the oscilator and swich from FRC to oscillator
	init_leds();			// initialize the LEDs 
	init_gpio();			// initialize the GPIO pin
	init_adc();				// initialize the ADC
	filter_init(&filterR, &filterI);	// initialize low pass filters
	
	
	// Turn on LED2 if FLASH_LED is defined
	#ifdef FLASH_LED
		LED2 = LED_ON;
	#endif //FLASH_LED
	

	// Loop Forever
	while(1){
		// Do Nothing
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
 * Description: 
 *****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void){
	
	fractional sample; // sample from ADC
	static int index = 0; // index into proc samp

	// get value from ADC register - ADC1BUF0
    // this assumes that the ADC is configured to output fractionl type
	sample = ADC1BUF0;

	// call the DSP routine with the sample
    proc_samp = decoder_dsp(sample, &filterR, &filterI);
    
    
    // for debug: add processed sample to past values array
    prev_proc_samp[index] = proc_samp;
    
    index++;
    if (index == 500){
	    index = 0;
	}
	
	// If morse code is detected
	if(proc_samp > detect_thres){
		CODE_OUTPUT = DETECTED;
		
		// #ifdef
		LED1 = LED_ON;
		
	}
	// Morse code not detected
	else{
		CODE_OUTPUT = NOT_DETECTED;
		
		LED1 = LED_OFF;		
	}	
	
		
	// For sampling rate testing, toggle LED1 each sample
	//LED1 = LED1 ^ 1; 
    

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
}
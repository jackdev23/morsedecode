/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	main.c
Created on:	3/12/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/12/2011 - Dylan Thorner - LEDs, oscillator etc.
	2: 04/01/2011 - Dylan Thorner - ADC testing
	3: 04/03/2011 - Added DSP routines and filters, First release for testing
	4: 04/10/2011 - Dylan Thorner - made it so a signal would not be detected until NUM_DET 
									samples above/below the threshold were seen in a row
	5: 04/10/2011 - Dylan Thorner - Added code for for dynamic thresholding
	6: 04/11/2011 - Sean Winfree  - threshold max/min change
	
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Main file for the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.
	
	Takes an input on port A4 (act. high), when this is high the decoder will measure the noise
	from the DSP routine and save the maximum sample. This max sample will be set as the new 
	threshold if it is with in a set of pre defined limits.
	
------------------------------------------------------------------------------------------------*/

#include "p33fxxxx.h" 	// includes correct header for processor
#include "dsp.h" 		// include microchip DSP routines, Must also include libdsp-coff.a in project

#include "led.h" 		// include the files to work with the LEDs
#include "oscillator.h" // file to initialize the oscillator
#include "gpio.h" 		// file to work with the GPIO pins (digital morse code output)
#include "adc.h" 		// file to define ADC parameters
#include "decoder_dsp.h"// DSP routine


#define FLASH_LED 		// Define this to have LED 1 flash and LED 2 on, else comment out

#define NUM_DET 	18 	// number of samples past threshold in a row to allow a detection

#define THRESH_WINDOW_FILTER_SIZE 32

//////////////////////
// Global Variables //
//////////////////////
FIRStruct filterI; 	// struct used by the FIR Filter function for real data
FIRStruct filterR; 	// struct used by the FIR Filter function for img data
volatile float proc_samp;	// the sample after it has been processed by the DSP routine
volatile int detect = 0; 	// 0 -> signal not detected		1-> signal detected
volatile int num_detected = 0; 	// number of samples detected (or not detected) in a row
static const float min_thres = 0.0028;	// minimum acceptable threshold
static const float max_thres = 0.03; 	// maximum acceptable threshold
volatile float detect_thres = 0.004; 	// default threshold for detecting code

//volatile float prev_proc_samp [500]; // past processed samples for debugging
//static const float detect_thres = 0.0012; // threshold for detectiong Morse code, 14mv
//static const float detect_thres = 0.003; // flickers - this was working down to -8dB during the day - recommend min 
//static const float detect_thres = 0.004; // flickers - this was working down to -8dB during the day - recommend min 
///static const float detect_thres = 0.05; // works at high SNR solid - USE WHEN IN TOWER with WINDOW = 32
//static const float detect_thres = 0.01; // works at high SNR solid
//static const float detect_thres = 0.025; // flickers - at window =64 worked some
//static const float detect_thres = 0.018; // flickers - at window =64 worked some,  (did not detect 300mV)

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
	init_adc();				// initialize the ADC
	init_gpio();			// initialize the GPIO pins
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
 * Description: Reads the ADC value, calls the DSP Routine and compares the 
 * 				output to a threshold, If greater than this threshold then a
*				Morse code pulse was detected and this is ouput to a GPIO pin.
 *****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void){
	
	fractional sample; // sample from ADC
	static float max_samp; // used to store the maximum sample when choosing a new threshold
	
	static float window_thresh[THRESH_WINDOW_FILTER_SIZE]; // array to store the average values
  	static int   window_idx;
  	static float average = 0;
	
	//static float max_samp_valus[300];
	//static int index = 0; // index into proc samp, used for debug only
  	//float prev_proc_samp[2000]; static int index=0;
	//float debug_sample;
	//float debug[2000];


	// get value from ADC register - ADC1BUF0
    // this assumes that the ADC is configured to output fractionl type
	sample = ADC1BUF0;

	//debug_sample = Fract2Float(sample);


	// call the DSP routine with the sample
    proc_samp = decoder_dsp(sample, &filterR, &filterI);

    // for debug: add processed sample to past values array
/*
  prev_proc_samp[index] = proc_samp;
	debug[index] = debug_sample;    
    index++;
    if (index == 2000){
	    index = 0;
	}
*/	

	//Check if the dynamic thesholding input is active, if it is keep track of the maximum sample
	// the input is generated by the HF Provider to signify a quiet period after a Periodic Status
	if (DTHRES_INPUT == 1){
		
		LED2 = LED_ON;// REMOVE AFTER TESTING
		
		if(proc_samp > max_samp){
			//keep if its legal
			if(proc_samp < max_thres){ 
				max_samp = proc_samp;
			}	
		}	
		
		//remeber that average has its value from last time
		average = average - window_thresh[window_idx]; // remove old sample
		average = average + mag;  				//add in new sample	
		window_thresh[window_idx] = mag; 		// store the mag (current sample) for next time
	
		window_idx++;
		if(window_idx >= WINDOW_FILTER_SIZE) {
			window_idx = 0; // reset to beginning of window
	 	}	
	 	

		
		detect_thres = max_samp-0.0012; //set new threshold
		
		// Check to ensure the new threshold is within the predefined limits
		if(detect_thres > max_thres){
			detect_thres = max_thres;
		}
		if(detect_thres < min_thres){
			detect_thres = min_thres;
		}
	}
	else{
		LED2 = LED_OFF; // REMOVE AFTER TESTING
		
		max_samp =  min_thres; // start at a smaller threshold next time if necessary
	}
	
	//4-10-2011: now checks for NUM_DET samples to be above/below the threshold before changing 
	//state (detected/not detected)
	// If morse code is detected
	if(detect == 1){
		CODE_OUTPUT = DETECTED; //output to GPIO pin
		#ifdef FLASH_LED 
			LED1 = LED_ON; // Turn on LED1 if FLASH_LED is defined
		#endif //FLASH_LED

		//check if processed sample is below threshold
		if(proc_samp < detect_thres){
			num_detected = num_detected + 1;
		}
		// if not then reset the number detected
		else{
			num_detected = 0;
		}
		
		// check to see if enough samples are detected in a row
		if(num_detected > NUM_DET){
			detect = 0; // state = not detected
			num_detected = 0;
		}
	}
	// Morse code not detected
	else{
		CODE_OUTPUT = NOT_DETECTED; //output to GPIO pin
		#ifdef FLASH_LED 
			LED1 = LED_OFF; // Turn on LED1 if FLASH_LED is defined
		#endif //FLASH_LED	
		
		// check to see if the processed sample is above the sample
		if(proc_samp > detect_thres){
			num_detected = num_detected + 1;
		}
		// if not then reset the number detected
		else{
			num_detected = 0;
		}	
		
		// check to see if enough samples are detected in a row
		if(num_detected > NUM_DET){
			detect = 1; // state = detected
			num_detected = 0;
		}
	}	

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
}


// PREVIOUS CODE FOR DETECTION FROM ISR:
/*
	// If morse code is detected
	if(proc_samp > detect_thres){
		CODE_OUTPUT = DETECTED;
		
		#ifdef FLASH_LED // Turn on LED1 if FLASH_LED is defined
			LED1 = LED_ON;
		#endif //FLASH_LED

		
	}
	// Morse code not detected
	else{
		CODE_OUTPUT = NOT_DETECTED;
		
		#ifdef FLASH_LED // Turn off LED1 if FLASH_LED is defined
			LED1 = LED_OFF;
		#endif //FLASH_LED	
	}	
*/


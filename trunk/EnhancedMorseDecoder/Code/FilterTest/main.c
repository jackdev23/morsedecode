/*******************************************************************
  Main.c

  DESCRIPTION:  


This code just simulates values to test the filter

  ------------------------------------------------------------------
  RELEASE    DATE   		AUTHOR			NOTES
  ------------------------------------------------------------------
  Initial    04/01/2011		Sean Winfree	

REFERENCES:
dsPIC DSP Library

********************************************************************/

#include "dsp.h"
#include "adc.h"
#include "decoder_dsp.h"
#include "math.h"

// the following is specific to the demo board and will need to be changed for the actual
// processor:
#include <p33FJ256GP506.h> // Device for DSC-Starter Kit, Change for EMD device.

// Chip Initialization of Configuration Registers
//   The Configuration registers are located in the program memory space. They are not Special
//   Function Registers. They are mapped into program memory space and are programmed at the
//   time of device programming.
_FOSCSEL(FNOSC_FRC);							// Select source at Power-on Reset --> Internal Fast RC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);// FCKSM = Only clock switching enabled, OSC2 Pin function = Digital I/O 
												// and Primary Oscillator Mode = Disabled
_FGS(GWRP_OFF & GCP_OFF);  						// Code Protect off, Write Protect OFF
_FWDT(FWDTEN_OFF);  							// disable watchdog for debug


// Global Variables 
volatile int debug_cnt = 0;
volatile fractional proc_samp; // the sample after it has been processed by the DSP routine
FIRStruct pFilter; // struct used by the FIR Filter function

int main (void)
{
	int i;
	//debug variable
	fractional sample_frac;
	fractional proc_samp;	
	static int sample_cnt = 0;
	float answerf = 0;

/* The Internal Fast RC (FRC) selected on Power-up provides a nominal 7.3MHz 
 * The FRC uses on-chip PLL to obtain higher speeds. 
 * To configure Oscillator to operate the device at 40MHz. 
 * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
 * Fosc= 7.37M*43/(2*2)=79.2275MHz (approx 80Mhz) for 7.37M input clock */	 
	PLLFBD=41;				/* M=43	*/
	CLKDIVbits.PLLPOST=0;	/* N1=2	*/
	CLKDIVbits.PLLPRE=0;	/* N2=2	*/
	OSCTUN=0;	
	__builtin_write_OSCCONH(0x01);		/*	Initiate Clock Switch to FRC with PLL*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b01);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);


   	//Setup ADC for 2100Hz sampling. Handeled by interrupt routine
   //	adc_init(); //not needed for sim

	// initalize the filter struct
	filter_init(&pFilter);

	// Loop Forever
	while(1)
	{
		i++;

  // DEBUG ONLY - the 700Hz signal should be filtered out
  //			- the 100Hz signal will not be.
  //            - the 50Hz not either, etc

//2*pi*freq_signal/freq_sampling = number_we_are_using
// 700/2100 == 1/3 == 0.33333
// 2*3.14159265*(700/2100)* ==> 2.0943951

//  10Hz =  0.029919930034189
//  50Hz =  0.149599650170943
// 100Hz =  0.299199300341885
// 600Hz =  1.795195802051310
// 700Hz =  2.094395102393195

	// GENERATE SIGNAL

//	sample_frac= Float2Fract(sinf((float)0.029919930034189*(float)sample_cnt)); //10
//	sample_frac= Float2Fract(sinf((float)0.149599650170943*(float)sample_cnt)); //50
//	sample_frac= Float2Fract(0.5* sinf((float)0.299199300341885*(float)sample_cnt)); //100
	//sample_frac= Float2Fract(sinf((float)0.329119230376074*(float)sample_cnt)); //110
	sample_frac= Float2Fract(sinf((float)0.448798950512828*(float)sample_cnt)); //150
//	sample_frac= Float2Fract(sinf((float)1.795195802051310*(float)sample_cnt)); //600
	//sample_frac= Float2Fract(0.5 * sinf((float)2.094395102393195*(float)sample_cnt)); //700
	sample_cnt = sample_cnt + 1;


	proc_samp = decoder_dsp(sample_frac, &pFilter);  //simulation only - building up code by simulating 

	answerf = Fract2Float(proc_samp);

	debug_cnt=0;





	}//end while

}



/*=============================================================================
_ADC1Interrupt():ADC Conversion is complete  
				ISR name is chosen from the device linker script.
				List of names in MPLAB C30 Compiler User's Guide (DS51284)
=============================================================================*/
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	fractional sample; // sample
	fractional sample_frac;
	fractional fsig_frac;
	fractional proc_samp;	
	static int sample_cnt = 0;
	float answerf = 0;

		sample = ADC1BUF0; //doesnt do anything in sim

	debug_cnt=0;

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
}

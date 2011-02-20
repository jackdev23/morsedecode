/*******************************************************************
  Main.c

  DESCRIPTION:  
  The following is the main loop of the Enhanced Morse Decoder. 

  An analog signal is sampled at 2100Hz. The analog signals is 
  transfered to RAM from ADC reg (ADC1BUF0) via ADC1 Interrupt

  The analog signal is then multiplied by a 700Hz sin & cos signal. 



  ------------------------------------------------------------------
  RELEASE    DATE   		AUTHOR			NOTES
  ------------------------------------------------------------------
  Initial    01/01/2011		Sean Winfree	


REFERENCES:
 dsPIC33F Family Reference Manual Section 07 - Oscillators (DS70186C)
 dsPIC33F Family Reference Manual Section 16 - ADC 	(DS70183C)

********************************************************************/

#include "dsp.h"
#include "adc.h"
#include "decoder_dsp.h"

// the following is specific to the demo board and will need to be changed for the actual
// processor:
#include <p33FJ256GP506.h> // Device for DSC-Starter Kit, Change for EMD device.
#include "sask.h"// Device for DSC-Starter Kit, Change for EMD device.


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
	//For DSC-Starter Kit only, Change for EMD device.
	SASKInit(); // initalize LEDS
	// Examples of how to turns the 3 LEDS on:
	GREEN_LED = SASK_LED_ON;
	YELLOW_LED = SASK_LED_ON;
	RED_LED = SASK_LED_ON;

	int i;

// TODO: Setup Clock - create function!
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
   	adc_init();

	// initalize the filter struct
	filter_init(&pFilter);

	// Forever
	while(1)
	{
		i++;
		debug_cnt++;
	}

}



/*=============================================================================
_ADC1Interrupt():ADC Conversion is complete  
				ISR name is chosen from the device linker script.
				List of names in MPLAB C30 Compiler User's Guide (DS51284)
=============================================================================*/
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	fractional sample; // sample

	// get value from ADC register - ADC1BUF0 and 
	sample = ADC1BUF0;

	//TODO: ENSURE THAT ADC RETURNS IN FRACTION FORMAT
	//TODO: CHECK # BITS ON ADC

	// cast to fractional
	sample_frac = (fractional)sample;
	

	// call the DSP routine with the sample
	proc_samp = decoder_dsp(sample, &pFilter);

	debug_cnt=0;

	//TODO: 
	//SquareMagnitudeCplx

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
}

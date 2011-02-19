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


//#include "c_def_types.h"
#include "dsp.h"
//#include "decoder.h"
#include "adc.h"
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
#define NUM_SAMPLES 100
volatile int result_sin[NUM_SAMPLES]; 
volatile int result_cos[NUM_SAMPLES]; 
volatile int debug_cnt = 0;

// Filters from dsp.h
extern FIRStruct pFilterI;
extern FIRStruct fir150FilterI;
extern FIRStruct fir150FilterQ;

extern fractional* FIRDecimate (        /* Decimation by R:1 rate */
   int numSamps,                        /* number of _output_ samples (N) */
                                        /* N = R*p (p integer) */
   fractional* dstSamps,                /* ptr to output samples */
                                        /* (y[n], 0 <= n < N) */
   fractional* srcSamps,                /* ptr to input samples */
                                        /* (x[n], 0 <= n < N*R) */
   FIRStruct* filter,                   /* filter structure: */
                                        /* number of coefficients in filter */
                                        /* same as number of delay elements */
                                        /* M = R*q (q integer) */
                                        /* (h[m], 0 <= m < M) */
                                        /* (d[m], 0 <= m < M) */
   int rate                             /* rate of decimation R (to 1) */

                                        /* returns dstSamps */
);

extern fractional* FIR (                /* FIR filtering */
   int numSamps,                        /* number of input samples (N) */
   fractional* dstSamps,                /* ptr to output samples */
                                        /* (y[n], 0 <= n < N) */
   fractional* srcSamps,                /* ptr to input samples */
                                        /* (x[n], 0 <= n < N) */
   FIRStruct* filter                    /* filter structure: */
                                        /* number of coefficients in filter */
                                        /* same as number of delay elements */
                                        /* (h[m], 0 <= m < M, an increasing */
                                        /*  circular buffer) */
                                        /* (d[m], 0 <= m < M, an increasing */
                                        /*  circular buffer) */

                                        /* returns dstSamps */
);


// TODO: Get the real values
const fractional sin_v[3] = {1,0,1};  
const fractional cos_v[3] = {1,0,1}; 

int main (void)
{
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

	//initialize result vector	
   	for(i=0; i<NUM_SAMPLES; i+=1)
   	{ 	//zero-pad vectors
		result_sin[i] = 0; 
		result_cos[i] = 0; 
	} 

   	//Setup ADC for 2100Hz sampling. Handeled by interrupt routine
   	adc_init();
   	
    /*FIRStructInit( 	&pFilterI,
					100,  // num of coeffs
					);
   	*/
	//uart_init();

   //TODO: Setup Decoder - this only partially complete
  // decoder_init(12,12); //
	
	while(1)
	{
		i++;
		debug_cnt++;


		//TODO: Call the decoder once we have enough samples to decode a dit/dah 
	}

}



/*=============================================================================
_ADC1Interrupt():ADC Conversion is complete  
				ISR name is chosen from the device linker script.
				List of names in MPLAB C30 Compiler User's Guide (DS51284)
=============================================================================*/
unsigned int Buffer = 0;
unsigned int indx = 0;

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{

	// get value from ADC register - ADC1BUF0 and 
	result_sin[indx] = ADC1BUF0;
	result_cos[indx] = ADC1BUF0;

	if(indx<NUM_SAMPLES)
	{
		indx++;
	}else
	{
		indx=0;
	}

	if(Buffer == 0)
	{

	} else
	{
		
	}
	debug_cnt=0;

	Buffer ^= 1;

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
}

/*
 * decoder_dsp.c
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#include "decoder_dsp.h"
#include "filter86.h" // header file for an 86 coefficient filter
#include "dsp.h"
#include "math.h"

#define WINDOW_FILTER_SIZE 16

static fractional sin_vals_frac[NUM_PTS];
static fractional cos_vals_frac[NUM_PTS];


void filter_init(FIRStruct * pFilter){

	// Initialize FIR Filter   	
    FIRStructInit( 	pFilter,
					NCOEFFS,  // num of coeffs
					filter86_coeffs ,// coefficients
					COEFF_PAGE_NUM  ,// 	
					z   // delay
					);
   	
	FIRDelayInit(pFilter);  // initialize the delay line 

	sin_vals_frac[0] = Float2Fract(0.0);
	sin_vals_frac[1] = Float2Fract(0.866025403784439);
	sin_vals_frac[2] = Float2Fract(-0.866025403784438);
	cos_vals_frac[0] = Float2Fract(1.000000000000000);
	cos_vals_frac[1] = Float2Fract(-0.499999999999999);
	cos_vals_frac[0] = Float2Fract(-0.500000000000001);

}


/******************************************************************************
 * Name: decoder_dsp
 * Inputs: fractional sample - the sample that was taken by the ADC
 * Outputs: fractional - the processed sample 
 *
 * Description: Implements the DSP routine defined in 
 *              Morse Decoder - Whitepaper.docx
 *****************************************************************************/
fractional decoder_dsp(fractional sample, FIRStruct * pFilter){

	// Variables for Sin/Cos multiply
	static int index = 0; // index into the sin_vals and cos_vals arrays
		
	fractional sigR; // the read part (multiplied by cosine)
	fractional sigI; // the imaginary part (multiplied by sine)

	// Variables for filtering
	fractional fsigR; // outputs of the filter
	fractional fsigI; // outputs of the filter	

  	float  mag=0;  // magnitude of the filtered values
  	static float window[WINDOW_FILTER_SIZE]; // array to store the average values
  	static int   window_idx;
  	static float average = 0;
  	static float debug_sigR, debug_sigI, debug_fsigR, debug_fsigI, debug_mag;
	static float debug_sigR_ary[1000];

	///////////////////////////////////////////////////////
	// Multiply by sine and cosine                       //
	///////////////////////////////////////////////////////

	sigR = sample * cos_vals_frac[index]; // real output
	sigI = sample * sin_vals_frac[index]; // imaginary output

	index = index + 1;
	// if the pointer has reached the end of the array
	if(index == NUM_PTS){
		index = 0; // reassign to the begining
	}
	
	debug_sigR = Fract2Float(sigR);
	debug_sigI = Fract2Float(sigI);

	///////////////////////////////////////////////////////
	// Lowpass filter                                    //
	///////////////////////////////////////////////////////
	// number samples, output, input, filter struct
	FIR(1,&fsigR,&sigR,pFilter);  // Real
	FIR(1,&fsigI,&sigI,pFilter);  // Imag	
	
	//convert results to float so we can use math.h functions :-)
	debug_fsigR = Fract2Float(fsigR);
	debug_fsigI = Fract2Float(fsigI);
	mag = 	sqrtf( powf(debug_fsigR,2) + powf(debug_fsigI,2));
	
	///////////////////////////////////////////////////////
	// Window Avg Filter                                 //
	///////////////////////////////////////////////////////	
	// lots of ways to do this.  an easy way is to add in new value and subtract out 
	// old value.  taking the average at the end.
	// or we can just make a filter of all 1's
	
	//remeber that average has its value from last time
	average = average - window[window_idx]; // remove old sample
	average = average + mag;  				//add in new sample	
	window[window_idx] = mag; // store the mag (current sample) for next time

	window_idx++;
	if(window_idx >= WINDOW_FILTER_SIZE) {
		window_idx = 0; // reset to beginning of window
 	}	
	//TODO: think about this.
	// average needs to be divide by the window size, if window is too large we can get overflow.
	//  this would corrupt the window for at least all of window size. 
	// we could reduce the size of the window by throwing out every other sample after the filter.
	// I would like at least 1000 samples per second to decide if we maintain the pin high or low 
    //  for downstream computaion. 
	
	return average/WINDOW_FILTER_SIZE; //return result

}

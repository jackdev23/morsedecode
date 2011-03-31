/*
 * decoder_dsp.c
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#include "decoder_dsp.h"
#include "filter86.h" // header file for an 86 coefficient filter

#define WINDOW_FILTER_SIZE 16

void filter_init(FIRStruct * pFilter){

	// Initialize FIR Filter   	
    FIRStructInit( 	pFilter,
					NCOEFFS,  // num of coeffs
					filter86_coeffs ,// coefficients
					COEFF_PAGE_NUM  ,// 	
					z   // delay
					);
   	
	FIRDelayInit(pFilter);  // initialize the delay line 
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

  	fractional mag=0;  // magnitude of the filtered values
  	static fractional window[WINDOW_FILTER_SIZE];
  	static int        window_idx;
  	static fractional average = 0;
  
	
	///////////////////////////////////////////////////////
	// Multiply by sine and cosine                       //
	///////////////////////////////////////////////////////

	sigR = sample * cos_vals[index]; // real output
	sigI = sample * sin_vals[index]; // imaginary output

	index = index + 1;
	// if the pointer has reached the end of the array
	if(index == NUM_PTS){
		index = 0; // reassign to the begining
	}



	///////////////////////////////////////////////////////
	// Lowpass filter                                    //
	///////////////////////////////////////////////////////
	// number samples, output, input, filter struct
	FIR(1,&fsigR,&sigR,pFilter);  // Real
	FIR(1,&fsigI,&sigI,pFilter);  // Imag	
	
	mag = (fsigR ^ 2) + (fsigI ^ 2);  //TODO: should we take sqrt?  not sure it is needed.
	
	
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

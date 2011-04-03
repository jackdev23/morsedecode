/*
 * decoder_dsp.c
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner & Sean Winfree
 */

#include "decoder_dsp.h"
#include "filter86.h" // header file for an 86 coefficient filter
#include "dsp.h"
#include "math.h"

#define WINDOW_FILTER_SIZE 32

static fractional sin_vals_frac[NUM_PTS];
static fractional cos_vals_frac[NUM_PTS];

/******************************************************************************
 * Name: filter_init
 * Inputs: FIRStruct * filterR - pointer to real filter struct
 *		   FIRStruct * filterI - pointer to imaginary filter struct
 * Outputs: n/a 
 *
 * Description: Initializes both input filter structs and their delay lines
 *****************************************************************************/
void filter_init(FIRStruct * filterR, FIRStruct * filterI){

	// Initialize real FIR Filter   	
    FIRStructInit( 	filterR,			//
					NCOEFFS,  			// num of coeffs
					filter86_coeffs ,	// coefficients
					COEFF_PAGE_NUM  ,	// 	
					zR   				// delay for reals
					);
	
	// Initialize imaginary FIR Filter   	
    FIRStructInit( 	filterI,			//
					NCOEFFS,  			// num of coeffs
					filter86_coeffs ,	// coefficients
					COEFF_PAGE_NUM  ,	// 	
					zI   				// delay for reals
					);
   	
	FIRDelayInit(filterR);  // initialize the delay line 
	FIRDelayInit(filterI);  // initialize the delay line 

	// These values are defined for Fsig = 700Hz and Fsample = 2100Hz ONLY.
	//  if you change either of these frequencies then these values must be recomputed.
	sin_vals_frac[0] = Float2Fract(0.0);
	sin_vals_frac[1] = Float2Fract(0.866025403784439);
	sin_vals_frac[2] = Float2Fract(-0.866025403784438);
	cos_vals_frac[0] = Float2Fract(1.000000000000000);
	cos_vals_frac[1] = Float2Fract(-0.499999999999999);
	cos_vals_frac[2] = Float2Fract(-0.500000000000001);

}


/******************************************************************************
 * Name: decoder_dsp
 * Inputs: fractional sample - the sample that was taken by the ADC
 * Outputs: fractional - the processed sample 
 *
 * Description: Implements the DSP routine defined in 
 *              Morse Decoder - Whitepaper.docx
 *****************************************************************************/
float decoder_dsp(fractional sample, FIRStruct * filterR, FIRStruct * filterI){

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
  	static float debug_fsigR, debug_fsigI;
  	
  	
  	
	///////////////////////////////////////////////////////
	// Multiply by sine and cosine                       //
	///////////////////////////////////////////////////////

	// the "*" sign cannot be used because fracts are seen as integers by the compiler
	VectorMultiply(1, &sigR, &sample, &cos_vals_frac[index]); // real output
	VectorMultiply(1, &sigI, &sample, &sin_vals_frac[index]); // imaginary output
	
	index = index + 1;
	// if the pointer has reached the end of the array
	if(index == NUM_PTS){
		index = 0; // reassign to the begining
	}
	
	
	///////////////////////////////////////////////////////
	// Lowpass filter                                    //
	///////////////////////////////////////////////////////
	// number samples, output, input, filter struct
	FIR(1,&fsigR,&sigR,filterR);  // Real
	FIR(1,&fsigI,&sigI,filterI);  // Imag	
	
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
	
	return average/WINDOW_FILTER_SIZE; //return result

}

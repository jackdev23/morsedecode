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

	// Variables for filtering
	fractional fsig; // outputs of the filter

  	static float debug_fsig[500];
	static float debug_samp[500];

	// number samples, output, input, filter struct
	FIR(1,&fsig,&sample,pFilter);  // just a filter	

	//save away for analysis
	debug_samp[index] = Fract2Float(sample);
	debug_fsig[index] = Fract2Float(fsig);
	index++;

	if(index>=500){
		index=0;
	}
	return fsig; //return result

}

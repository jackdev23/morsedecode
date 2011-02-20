/*
 * decoder_dsp.c
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#include "decoder_dsp.h"
#include "filter86.h" // header file for an 86 coefficient filter


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
	//fractional fsigI;



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
	
	FIR(1,&fsigR,&sigR,pFilter);


	return 0; //dummy return

}

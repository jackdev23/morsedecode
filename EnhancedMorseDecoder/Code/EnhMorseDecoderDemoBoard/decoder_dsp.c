/*
 * decoder_dsp.c
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#include "decoder_dsp.h"


/******************************************************************************
 * Name: decoder_dsp
 * Inputs: fractional sample - the sample that was taken by the ADC
 * Outputs: fractional - the processed sample 
 *
 * Description: Implements the DSP routine defined in 
 *              Morse Decoder - Whitepaper.docx
 *****************************************************************************/
fractional decoder_dsp(fractional sample){

	static int index = 0; // index into the sin_vals and cos_vals arrays

	fractional sigR; // the read part (multiplied by cosine)
	fractional sigI; // the imaginary part (multiplied by sine)



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



	return 0; //dummy return

}

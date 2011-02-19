/*
 * decoder_dsp.c
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

/******************************************************************************
 * Name: decoder_dsp
 * Inputs: fractional sample - the sample that was taken by the ADC
 * Outputs: fractional - the processed sample 
 *
 * Description: Implements the DSP routine defined in 
 *              Morse Decoder - Whitepaper.docx
 *****************************************************************************/
fractional decoder_dsp(fractional sample){

	const float * cos_ptr = cos_vals; // these are pointers use to access the array of sin and cos values
	const float * sin_ptr = sin_vals; // pointer (that can be changed) to a float (that cannot be changed)

	///////////////////////////////////////////////////////
	// Multiply by sine and cosine                       //
	///////////////////////////////////////////////////////

	sigR = sig * (*cos_ptr); // real output of the hilbert transform
	cos_ptr++;
	// if the pointer has reached the end of the array
	if(cos_ptr == cos_vals + (F_SAMP/F_SIG)){
		cos_ptr = cos_vals; // reassign to the begining
	}

	sigI = sig * (*sin_ptr); // imaginary output of the hilbert transform
	sin_ptr++;
	// if the pointer has reached the end of the array
	if(sin_ptr == sin_vals + (F_SAMP/F_SIG)){
		sin_ptr = sin_vals; // reassign to the begining
	}

	///////////////////////////////////////////////////////
	// Lowpass filter                                    //
	///////////////////////////////////////////////////////

}
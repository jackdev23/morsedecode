/*
 * decoder_dsp.h
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#ifndef DECODER_DSP_H_
#define DECODER_DSP_H_

#include "dsp.h" // microchip filter header file


#define F_SAMP 	2100 		 // sampling rate
#define F_SIG  	700 		 // the frequency of the sinusoid being sampled
#define NUM_PTS F_SAMP/F_SIG // the number of points the sin and cos arrays contain


// Function pototypes
void filter_init(FIRStruct * pFilter);
fractional decoder_dsp(fractional sample,FIRStruct * pFilter);

#endif /* DECODER_DSP_H_ */

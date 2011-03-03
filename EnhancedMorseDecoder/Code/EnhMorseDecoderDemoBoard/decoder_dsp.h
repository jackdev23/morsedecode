/*
 * decoder_dsp.h
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#ifndef DECODER_DSP_H_
#define DECODER_DSP_H_

#include "dsp.h" // microchip filter header file


#define F_SAMP 2100 // sampling rate
#define F_SIG  700 // the frequency of the sinusoid being sampled
#define NUM_PTS F_SAMP/F_SIG // the number of points the sin and cos arrays contain

// These values are defined for Fsig = 700Hz and Fsample = 2100Hz ONLY.
//  if you change either of these frequencies then these values must be recomputed.
//static const fractional sin_vals[NUM_PTS] = {0.0,0.866025403784439,-0.866025403784438};  
static const fractional sin_vals[NUM_PTS] = {0x000A,0x000B,0x000C};  
// example of how to initalize a fraction type as found online. Verify this.
// puts it in a soecial secion of prog. mem. that cannot be accessed by regualar C statements?
//static const fractional sin_vals[NUM_PTS] __attribute__ ((space(psv))) = {0.0,0.866025403784439,-0.866025403784438};
static const fractional cos_vals[NUM_PTS] = {1.000000000000000,-0.499999999999999,-0.500000000000001}; 

//TODO: because fractional is really an int the init of the decimal values does not work.

// Function pototypes
void filter_init(FIRStruct * pFilter);
fractional decoder_dsp(fractional sample,FIRStruct * pFilter);

#endif /* DECODER_DSP_H_ */

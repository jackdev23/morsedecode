/*
 * decoder_dsp.h
 *
 *  Created on: Feb 19, 2011
 *      Author: Dylan Thorner
 */

#ifndef DECODER_DSP_H_
#define DECODER_DSP_H_

// These values are defined for Fsig = 700Hz and Fsample = 2100Hz ONLY.
//  if you change either of these frequencies then these values must be recomputed.
const fractional sin_v[3] = {0.0,0.866025403784439,-0.866025403784438};  
const fractional cos_v[3] = {1.000000000000000,-0.499999999999999,-0.500000000000001}; 


#endif /* DECODER_DSP_H_ */
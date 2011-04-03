
/*
 * filter86.h
 *
 *  Created on: Feb 19, 2011
 *      Author: SWINFREE
 */

#ifndef FILTER86_H_
#define FILTER86_H_

#include "dsp.h"

#define NCOEFFS 86				// number of coeff
#define COEFF_PAGE_NUM 0xFF00   // coeff page number; data space = 0xFF00 

////////////////////////////////////////
// FIR Coeffecient Buffer
fractional filter86_coeffs[NCOEFFS] __attribute__ ((space(xmemory),far)) = {
0x0004,0x0007,0x000b,0x000e,0x0010,0x000f,0x0009,0xfffb,0xffe4,0xffc5,0xff9f,
0xff74,0xff4b,0xff2a,0xff18,0xff1c,0xff3c,0xff78,0xffcf,0x0038,0x00a6,0x010b,
0x0155,0x0171,0x0154,0x00f8,0x005e,0xff96,0xfeb7,0xfde1,0xfd3b,0xfcea,0xfd11,
0xfdc7,0xff17,0x00f8,0x0351,0x05f8,0x08b6,0x0b4e,0x0d82,0x0f1c,0x0ff4,0x0ff4,
0x0f1c,0x0d82,0x0b4e,0x08b6,0x05f8,0x0351,0x00f8,0xff17,0xfdc7,0xfd11,0xfcea,
0xfd3b,
0xfde1,
0xfeb7,
0xff96,
0x005e,
0x00f8,
0x0154,
0x0171,
0x0155,
0x010b,
0x00a6,
0x0038,
0xffcf,
0xff78,
0xff3c,
0xff1c,
0xff18,
0xff2a,
0xff4b,
0xff74,
0xff9f,
0xffc5,
0xffe4,
0xfffb,
0x0009,
0x000f,
0x0010,
0x000e,
0x000b,
0x0007,
0x0004 
};



// FIR Delay Buffer
fractional zR[NCOEFFS] __attribute__ ((space(ymemory),far)); 
fractional zI[NCOEFFS] __attribute__ ((space(ymemory),far)); 
////////////////////////////////////////

#endif /* FILTER86_H_ */
//EOF
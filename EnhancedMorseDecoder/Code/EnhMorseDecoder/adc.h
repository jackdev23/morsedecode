/*
 * adc.h
 *
 *  Created on: DEC 13, 2010
 *      Author: SWINFREE
 */

#ifndef ADC_H_
#define ADC_H_


// Sampling Control
#define Fosc		80000000  	// 80MHz clock
#define Fcy			(Fosc/2)	// Internal speed
#define Fs   		2100		// sampling frequency
#define SAMPPRD    (Fcy/Fs)-1	// Timer Compare register value for sampling period

#include<p33Fxxxx.h>

//-----------------------------------------------------------------------------//

void adc_init(void);

#endif /* ADC_H_ */

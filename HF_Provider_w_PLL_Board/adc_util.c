/*
 * adc_suppress.c
 *
 *  Created on: Apr 17, 2010
 *      Author: maelliot
 */
#include <avr/io.h>

#include "adc_util.h"

/**
 * The following two functions support the trailing macro that allows
 * ADC interrupt atomic operations to occur safely (no other locks
 * are provided).  As such, don't change those return values!
 */
// Unset interrupts
int adc_util_disable_isr(ADC_t *_adc){
	uint8_t isrlvl = _adc->INTFLAGS & ADC_CH_INTLVL_gm;
	_adc->INTFLAGS &= ~ADC_CH_INTLVL_gm;
	return (isrlvl > 0) ? isrlvl : ADC_CH_INTLVL_LO_gc;
}

// Set interrupts
int adc_util_enable_isr(ADC_t *_adc, uint8_t isrlvl){
	_adc->INTFLAGS |= isrlvl;
	return 0;
}

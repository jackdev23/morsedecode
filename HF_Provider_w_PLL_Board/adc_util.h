/*
 * adc_util.h
 *
 *  Created on: Apr 17, 2010
 *      Author: maelliot
 */

#ifndef ADC_UTIL_H_
#define ADC_UTIL_H_

int adc_util_disable_isr(ADC_t *_adc);
int adc_util_enable_isr(ADC_t *_adc, uint8_t isrlvl);

/**
 * Usage:
 *
 *    DECODER_SUPPRES_ADC(&ADCA){
 *    	code executed with ADC disabled (ADC interrupt will not fire here!)
 *    }
 *
 * Note: if the ADC is off the code will execute, and the ADC will have LO interrupts on.
 */
#define SUPPRESS_ADC(_adc) for ( uint8_t __ToDo = adc_util_disable_isr((_adc)); __ToDo ; __ToDo = adc_util_enable_isr((_adc), __ToDo) )

#endif /* ADC_UTIL_H_ */

/*
 * adc.c
 *
 *  Created on: DEC 13, 2010
 *      Author: SWINFREE
 */

#include "adc.h"

void init_TMR3(void);  // local function used to setup ADC functionality

/*========================================================================================  
ADC Setup 
========================================================================================*/
void adc_init(void)
{
	AD1CON1bits.FORM   = 3;		// Data Output Format: Signed Fraction (Q15 format)
	AD1CON1bits.SSRC   = 2;		// Sample Clock Source: GP Timer starts conversion - TMR3
	AD1CON1bits.ASAM   = 1;		// ADC Sample Control: Sampling begins immediately after conversion
	AD1CON1bits.AD12B  = 1;		// 12-bit ADC operation

	AD1CON2bits.CHPS  = 0;		// Converts CH0
     
	AD1CON3bits.ADRC=0;			// ADC Clock is derived from Systems Clock (ADC_CHANNEL_FCY)
	AD1CON3bits.ADCS = 63;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
								// ADC Conversion Time for 12-bit Tc=14*Tad = 22.4us 
								// slowest converstion speeed possible. 0x3F(63) means divide by 64
					
	AD1CON1bits.ADDMABM = 1; 	// DMA buffers are built in conversion order mode ?
	AD1CON2bits.SMPI    = 0;	// SMPI must be 0. Generates Interrupt after completion of 
								// every sample/conversion operation

    //AD1CHS0: A/D Input Select Register
    AD1CHS0bits.CH0SA=0;		// MUXA +ve input selection (AN0) for CH0
	AD1CHS0bits.CH0NA=0;		// MUXA -ve input selection (Vref-) for CH0

    //AD1PCFGH/AD1PCFGL: Port Configuration Register
	AD1PCFGL=0xFFFF;			// disable all analog inputs
	AD1PCFGH=0xFFFF;			// disable all analog inputs
    AD1PCFGLbits.PCFG0 = 0;		// set AN0 as Analog Input
       
    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 0;			// Do Not Enable A/D interrupt  !! This should be enabled eventually
    AD1CON1bits.ADON = 1;		// Turn on the A/D converter	
	
	//Setup Timer 3 to set sampling rate of ADC
    init_TMR3();

	IEC0bits.AD1IE = 1;			// Enable A/D interrupt, 

} //adc_init

/*========================================================================================  
Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). As a result, the module 
will stop sampling and trigger a conversion on every Timer3 time-out, i.e., Ts=125us. 
At that time, the conversion process starts and completes Tc=14*Tad periods later.

When the conversion completes, the module starts sampling again. However, since Timer3 
is already on and counting, about (Ts-Tc)us later, Timer3 will expire again and trigger 
next conversion. 
========================================================================================*/
void init_TMR3(void) 
{
        TMR3 			= 0x0000;  // initialize timer to 0
		PR3			  	= SAMPPRD; //sampling rate
        IFS0bits.T3IF 	= 0;
        IEC0bits.T3IE 	= 0;

        //Start Timer 3
        T3CONbits.TON = 1;

}//init_TMR3
//TIMER INTERRUPT TRIGGER - define sampling rate
// This ADC module trigger mode is configured by setting SSRC<2:0> = 010. TMR3 (for ADC1)
// and TMR5 (for ADC2) can be used to trigger the start of the A/D conversion when a match occurs
// between the 16-bit Timer Count register (TMRx) and the 16-bit Timer Period register (PRx). The
// 32-bit timer can also be used to trigger the start of the A/D conversion.

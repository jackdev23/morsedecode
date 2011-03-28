/*-----------------------------------------------------------------------------------------------
Author: 	Dylan Thorner
Filename: 	adc.c
Created on:	3/26/2011
-------------------------------------------------------------------------------------------------
CHANGE HISTORY:
	1: 03/26/2011 - Dylan Thorner - Original release
-------------------------------------------------------------------------------------------------
DESCRIPTION:
	Functions to set up and work with the ADC.
	
	Designed to work with the dsPIC33FJ128GP802 microcontroller on the Enhanced 
	Morse Decoder Board Rev. 1.0.
	
-------------------------------------------------------------------------------------------------
REFRENCES:
	Section 16. Analog-to-Digital Converter (ADC)
	
	Example project for ADC configuration: CE100_ADC_IIR_Filter_14aug09 
		from http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en532298
-----------------------------------------------------------------------------------------------*/


/******************************************************************************
 * Name: init_adc
 * Inputs: n/a
 * Outputs: n/a 
 *
 * Description: Initializes the ADC module to the specifications listed in the
 *				comments below.
 *****************************************************************************/
void init_adc(void){
	
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
}	



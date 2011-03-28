/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33fxxxx/PIC24Hxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* RK               03/22/06  First release of source file
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* Code Tested on:
* Explorer 16 Demo board with dsPIC33FJ256GP710 controller &
* Explorer 16 Demo board with  PIC24HJ256GP610 controller
* The Processor starts with the External Crystal without PLL enabled and then the Clock is switched to PLL Mode.
**********************************************************************/
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif

#include "ctglPin.h"

//Macros for Configuration Fuse Registers:
//Invoke macros to set up  device configuration fuse registers.
//The fuses will select the oscillator source, power-up timers, watch-dog
//timers etc. The macros are defined within the device
//header files. The configuration fuse registers reside in Flash memory.

// DJT - added same as EMD code
// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching(FSCM is disabled), OSC2 Pin set to output clk (will be ignored b/c used for oscillator), 
//and Configure Posc in XT mode
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);


// External Oscillator
//_FOSCSEL(FNOSC_PRI);			// Primary (XT, HS, EC) Oscillator
//_FOSC(FCKSM_CSECME & OSCIOFNC_OFF  & POSCMD_XT);  
								// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
								// OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: XT Crystanl

// Internal FRC Oscillator
//_FOSCSEL(FNOSC_FRCPLL );		// FRC Oscillator with PLL
//_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF  & POSCMD_NONE); 
								// Clock Switching and Fail Safe Clock Monitor is disabled
								// OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: Disabled


_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software

int main (void)
{
	//DJT - Added the same oscilator init section as used in EMD code
	// Configure PLL prescaler, PLL postscaler, PLL divisor
	PLLFBD=38; 				// M = 40
	CLKDIVbits.PLLPOST = 0;	// N2 = 2
	CLKDIVbits.PLLPRE = 0; 	// N1 = 2
	
	// Disable Watch Dog Timer
	RCONbits.SWDTEN=0;
	
	// Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
	__builtin_write_OSCCONH(0x03); // Set the NOSC control bits (OSCCON<10:8>) for XTPLL
	__builtin_write_OSCCONL(0x01); //Set the OSWEN bit (OSCCON<0>) to initiate the oscillator switch.
	
	// Wait for Clock switch to occur
	while (OSCCONbits.COSC != 0b011) {};
	
	// Wait for PLL to lock
	while(OSCCONbits.LOCK != 1) {};
	


// Initialise Toggle Pin
    ctglPinInit();

	while (1)
	{
// Continuously toggle RB5 pin, it will be toggling at 1/4 of system clock frequency
		ctglPin();
	}					 		/* Loop endlessly */
       	       	            	/* If an Oscillator failure occurs, the device will */
        	                    /* vector to the Oscillator Failure Trap */
                                /* Upon eturn from the trap, the device will operate */
                                /* using FRC oscillator (nominally 7.37MHz) */
                                /* Observe the toggling rate of pin RA6 */

/* Note:
Once you have recovered from the Clock failure (i.e. serviced the
Oscillator Fail trap and are now using the internal FRC), you can
switch back to the crystal if required using a clock switch operation
*/

}





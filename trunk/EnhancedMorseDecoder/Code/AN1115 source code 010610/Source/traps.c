
/**********************************************************************
* � 2006 Microchip Technology Inc.
*
* FileName:        traps.c
* Dependencies:    p33FJ256GP710.h
* Processor:       dsPIC33F
* Compiler:        MPLAB� C30 v2.01 or higher
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
* Hrushikesh Vasuki 05/03/05  First release of source file
* Richard Fischer   07/14/05  Add DMAC exception handler
* Priyabrata Sinha  01/27/06  Ported to non-prototype devices
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* 1. This file contains trap service routines (handlers) for hardware
*    exceptions generated by the dsPIC30F device.
* 2. All trap service routines in this file simply ensure that device
*    continuously executes code within the trap service routine. Users
*    may modify the basic framework provided here to suit to the needs
*    of their application.
*
**********************************************************************/

#include <p33Fxxxx.h>

void __attribute__((__interrupt__)) _OscillatorFail(void);
void __attribute__((__interrupt__)) _AddressError(void);
void __attribute__((__interrupt__)) _StackError(void);
void __attribute__((__interrupt__)) _MathError(void);
void __attribute__((__interrupt__)) _DMACError(void);
void __attribute__((__interrupt__)) _AltOscillatorFail(void);
void __attribute__((__interrupt__)) _AltAddressError(void);
void __attribute__((__interrupt__)) _AltStackError(void);
void __attribute__((__interrupt__)) _AltMathError(void);
void __attribute__((__interrupt__)) _AltDMACError(void);

/* Primary Exception Vector handlers:
   These routines are used if INTCON2bits.ALTIVT = 0.
   All trap service routines in this file simply ensure that device
   continuously executes code within the trap service routine. Users
   may modify the basic framework provided here to suit to the needs
   of their application. */

void __attribute__((__interrupt__, no_auto_psv)) _OscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        //Clear the trap flag
        while (1);
}

void __attribute__((__interrupt__, no_auto_psv)) _AddressError(void)
{
        INTCON1bits.ADDRERR = 0;        //Clear the trap flag
        while (1);
}

void __attribute__((__interrupt__, no_auto_psv)) _StackError(void)
{
        INTCON1bits.STKERR = 0;         //Clear the trap flag
        while (1);
}

void __attribute__((__interrupt__, no_auto_psv)) _MathError(void)
{
        INTCON1bits.MATHERR = 0;        //Clear the trap flag
        while (1);
}


void __attribute__((__interrupt__, no_auto_psv)) _DMACError(void)
{
		/* reset status bits, real app should check which ones */
		DMACS0 = 0;
		
        INTCON1bits.DMACERR = 0;        //Clear the trap flag
        while (1);
}


/* Alternate Exception Vector handlers:
   These routines are used if INTCON2bits.ALTIVT = 1.
   All trap service routines in this file simply ensure that device
   continuously executes code within the trap service routine. Users
   may modify the basic framework provided here to suit to the needs
   of their application. */

void __attribute__((__interrupt__, no_auto_psv)) _AltOscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;
        while (1);
}

void __attribute__((__interrupt__, no_auto_psv)) _AltAddressError(void)
{
        INTCON1bits.ADDRERR = 0;
        while (1);
}

void __attribute__((__interrupt__, no_auto_psv)) _AltStackError(void)
{
        INTCON1bits.STKERR = 0;
        while (1);
}

void __attribute__((__interrupt__, no_auto_psv)) _AltMathError(void)
{
        INTCON1bits.MATHERR = 0;
        while (1);
}


void __attribute__((__interrupt__, no_auto_psv)) _AltDMACError(void)
{
		/* reset status bits, real app should check which ones */
		DMACS0 = 0;
		
        INTCON1bits.DMACERR = 0;        //Clear the trap flag
        while (1);
}

/**********************************************************************
* © 2006 Microchip Technology Inc.
*
* FileName:        clrClkFail.s
* Dependencies:    Header (.h/.inc) files if applicable, see below
* Processor:       dsPIC33Fxxxx/PIC24Hxxxx
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
* RK                03/22/06  First release of source file
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
**********************************************************************/

.ifdef __dsPIC33F
.include "p33fxxxx.inc"
.endif

.ifdef __PIC24H
.include "p24hxxxx.inc"
.endif

	.global _clrClkFail
 
;
_clrClkFail:
        mov     OSCCON, w0              ;Read OSCCON in to w0
        bclr    w0, #CF                 ;Before clearing the OSCFAIL trap status flag
                                        ;we should clear the Clock Fail(CF) bit in
                                        ;OSCCONL

                                        ;Load w0 with a byte value to write to
                                        ;OSCCONL so that the CF bit is cleared
        mov     #OSCCONL, w1            ;Point w1 to OSCCONL byte
        disi    #6                      ;Disable interrupts for the next
                                        ;5 instruction cycles

        mov     #0x46, w2               ;Move first OSCCONL unlock code byte to w2
        mov     #0x57, w3               ;Move second OSCCONL unlock code byte to w3
        mov.b   w2, [w1]                ;Perform byte-wide move of w2 to OSCCONL
        mov.b   w3, [w1]                ;Perform byte-wide move of w2 to OSCCONL
        mov.b   w0, [w1]                ;Write parameter in w0 (lower byte)to
                                        ;OSCCONL within one instruction cycle
                                        ;of unlocking OSCCONL using MOV instruction
        return

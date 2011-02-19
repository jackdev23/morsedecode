/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        firinit.s
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx
* Compiler:        MPLAB® C30 v3.01 or higher
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
**********************************************************************/

	; Local inclusions.
	.nolist
	.include	"dspcommon.inc"		; FIR filter structure
	.list

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.section .libdsp, code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _FIRStructInit: initialization of FIR filter structure.
;
; Operation:
;	FIRFilter->numCoeffs = numCoeffs;
;	FIRFilter->coeffsBase = coeffsBase;
;	FIRFilter->coeffsEnd = coeffsBase+numCoeffs-(1 byte);
;	FIRFilter->coeffsPage = coeffsPage;
;	FIRFilter->delayBase = delayBase;
;	FIRFilter->delayEnd = delayBase+numCoeffs-(1 byte);
;	FIRFilter->delay = delayBase;
;
; Input:
;	w0 = h, ptr FIR filter structure (see included file)
;	w1 = numCoeffs;
;	w2 = coeffsBase;
;	w3 = coeffsPage;
;	w4 = delayBase;
; Return:
;	(void)
;
; System resources usage:
;	{w0..w5}	used, not restored
;
; DO and REPEAT instruction usage.
;	no DO intructions
;	no REPEAT intruction
;
; Program words (24-bit instructions):
;	10
;
; Cycles (including C-function call and return overheads):
;	19
;............................................................................

	.global	_FIRStructInit	; export
_FIRStructInit:

;............................................................................

	; Prepare for initialization.
	sl	w1,w5				; w5 = numCoeffs*sizeof(coeffs)
	dec	w5,w5				; w5 =
						;   numCoeffs*sizeof(coeffs)-1

;............................................................................

	; Set up filter structure.
	mov	w1,[w0++]		; FIRFilter->numCoeffs = numCoeffs
					; w0 =&(FIRFilter->coeffsBase)
	mov	w2,[w0++]		; FIRFilter->coeffsBase = coeffsBase
					; w0 =&(FIRFilter->coeffsEnd)
	add	w2,w5,[w0++]		; FIRFilter->coeffsEnd initialized
					; w0 =&(FIRFilter->coeffsPage)
	mov	w3,[w0++]		; FIRFilter->coeffsPage = coeffsPage
					; w0 =&(FIRFilter->delayBase)
	mov	w4,[w0++]		; FIRFilter->delayBase = delayBase
					; w0 =&(FIRFilter->delayEnd)
	add	w4,w5,[w0++]		; FIRFilter->delayEnd initialized
					; w0 =&(FIRFilter->delay)
	mov	w4,[w0]			; FIRFilter->delay initialized

;............................................................................

	return	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF

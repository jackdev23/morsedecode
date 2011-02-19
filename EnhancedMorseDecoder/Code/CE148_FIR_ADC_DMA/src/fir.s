/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        fir.s
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
	.include	"dspcommon.inc"		; MODCON, XMODSRT, XMODEND,
						; YMODSRT, YMODEND, CORCON,
						; PSVPAG, COEFFS_IN_DATA,
						; FIRStruct
	.list

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.section .libdsp, code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _FIR: FIR block filtering.
;
; Operation:
;	y[n] = sum_(m=0:M-1){h[m]*x[n-m]}, 0 <= n < N.
;
; x[n] defined for 0 <= n < N,
; y[n] defined for 0 <= n < N,
; h[m] defined for 0 <= m < M as an increasing circular buffer,
; NOTE: delay defined for 0 <= m < M as an increasing circular buffer.
;
; Input:
;	w0 = number of samples to generate (numSamps, N)
;	w1 = ptr to output samples (dstSamps, y)
;	w2 = ptr to input samples (srcSamps, x)
;	w3 = filter structure (FIRStruct, h)
;
; Return:
;	w0 = ptr to output samples (dstSamps, y)
;
; System resources usage:
;	{w0..w6}	used, not restored
;	{w8,w10}	saved, used, restored
;	 AccuA		used, not restored
;	 CORCON		saved, used, restored
;	 PSVPAG		saved, used, restored (if coeffs in P memory)
;	 MODCON		saved, used, restored
;	 XMODSRT	saved, used, restored
;	 XMODEND	saved, used, restored
;	 YMODSRT	saved, used, restored
;	 YMODEND	saved, used, restored
;
; DO and REPEAT instruction usage.
;	1 DO instructions
;	1 REPEAT intructions
;
; Program words (24-bit instructions):
;	56
;
; Cycles (including C-function call and return overheads):
;	53 + N*(5+M), or
;	56 + N*(9+M) if coefficients in P memory.
;............................................................................

	.global	_FIR	; export
_FIR:

;............................................................................

	; Save working registers.
	push	w8				; w8 to TOS
	push	w10				; w10 to TOS

;............................................................................

	; Prepare CORCON for fractional computation.
	push	CORCON
	fractsetup	w8

;............................................................................

	; Prepare CORCON and PSVPAG for possible access of data
	; located in program memory, using the PSV.
	push	PSVPAG

	mov	[w3+oCoeffsPage],w10		; w10= coefficients page
	mov	#COEFFS_IN_DATA,w8		; w8 = COEFFS_IN_DATA
	cp	w8,w10				; w8 - w10
	bra	z,_noPSV			; if w10 = COEFFS_IN_DATA
						; no PSV management
						; else
	psvaccess	w8			; enable PSV bit in CORCON
	mov	w10,PSVPAG			; load PSVPAG with program
						; space page offset
_noPSV:

;............................................................................

	; Prepare core registers for modulo addressing.
	push	MODCON
	push	XMODSRT
	push	XMODEND
	push	YMODSRT
	push	YMODEND

;............................................................................

	; Setup registers for modulo addressing.
	mov	#0xC0A8,w10			; XWM = w8, YWM = w10
						; set XMODEND and YMODEND bits
	mov	w10,MODCON			; enable X,Y modulo addressing

	mov	[w3+oCoeffsEnd],w8		; w8 -> last byte of h[M-1]
	mov	w8,XMODEND			; init'ed to coeffs end address
	mov	[w3+oCoeffsBase],w8		; w8 -> h[0]
	mov	w8,XMODSRT			; init'ed to coeffs base address
						; (increasing buffer,
						;  2^n aligned)
	mov	[w3+oDelayEnd],w10		; w10-> last byte of d[M-1]
	mov	w10,YMODEND			; init'ed to delay end address
	mov	[w3+oDelayBase],w10		; w10 -> d[0]
	mov	w10,YMODSRT			; init'ed to delay base address
						; (increasing buffer,
						;  2^n aligned)

;............................................................................

	push	w1				; save return value (y)

;............................................................................

	; Perpare to all filter.
	mov	[w3+oDelay],w10			; w10 points at current delay
						; sample d[m], 0 <= m < M
	mov	[w3+oNumCoeffs],w4		; w4 = M
	sub	w4,#3,w4			; W4 = M-3
	dec	w0,w0				; w0 = N-1

;............................................................................

	; Perform filtering of all samples.
	do	w0,_endFilter		; {	; do (N-1)+1 times

	; Prepare to filter sample.
	mov	[w2++],[w10]			; store new sample into delay

	clr	a,[w8]+=2,w5,[w10]+=2,w6	; a  = 0
						; w5 = h[0]
						; w8-> h[1]
						; w6 = d[current]
						; w10->d[next]

	; Filter each sample.
	; (Perform all but two last MACs.)
	repeat	w4			; {	; do (M-3)+1 times
	mac	w5*w6,a,[w8]+=2,w5,[w10]+=2,w6	; a += h[m]*d[current]
						; w5 = h[m+1]
						; w8-> h[m+2]
						; w6 = d[next]
						; w10->d[next+1]
; }
	; (Perform second last MAC.)
	mac	w5*w6,a,[w8]+=2,w5,[w10],w6	; a += h[M-2]*d[current]
						; w5 = h[M-1]
						; w8-> h[0]
						; w6 = d[next]
						; w10->d[next]
	; (Perform last MAC.)
	mac	w5*w6,a				; a += h[M-1]*d[current]

_endFilter:
	; Save filtered result.
	sac.r	a,[w1++]			; y[n] =
						;   sum_{m=0:M-1}(h[m]*x[n-m])
						; w1-> y[n+1]
; }

;............................................................................

	; Update delay pointer.
	mov	w10,[w3+oDelay]			; note that the delay pointer
						; may wrap several times around
						; d[m], 0 <= m < M, depending
						; on the value of N

;............................................................................

	pop	w0				; restore return value

;............................................................................

	; Restore core registers for modulo addressing.
	pop	YMODEND
	pop	YMODSRT
	pop	XMODEND
	pop	XMODSRT
	pop	MODCON

;............................................................................

	; Restore PSVPAG and CORCON.
	pop	PSVPAG
	pop	CORCON

;............................................................................

	; Restore working registers.
	pop	w10				; w10 from TOS
	pop	w8				; w8 from TOS

;............................................................................

	return	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF

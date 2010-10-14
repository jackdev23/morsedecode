#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "debug.h"
#include "encoder.h"

prog_uchar morse_table[54][7] = {
		{'.','-', 0 , 0 , 0 , 0 , 0 }, // A
		{'-','.','.','.', 0 , 0 , 0 }, // B
		{'-','.','-','.', 0 , 0 , 0 }, // C
		{'-','.','.', 0 , 0 , 0 , 0 }, // D
		{'.', 0 , 0 , 0 , 0 , 0 , 0 }, // E
		{'.','.','-','.', 0 , 0 , 0 }, // F
		{'-','-','.', 0 , 0 , 0 , 0 }, // G
		{'.','.','.','.', 0 , 0 , 0 }, // H
		{'.','.', 0 , 0 , 0 , 0 , 0 }, // I
		{'.','-','-','-', 0 , 0 , 0 }, // J
		{'-','.','-', 0 , 0 , 0 , 0 }, // K
		{'.','-','.','.', 0 , 0 , 0 }, // L
		{'-','-', 0 , 0 , 0 , 0 , 0 }, // M
		{'-','.', 0 , 0 , 0 , 0 , 0 }, // N
		{'-','-','-', 0 , 0 , 0 , 0 }, // O
		{'.','-','-','.', 0 , 0 , 0 }, // P
		{'-','-','.','-', 0 , 0 , 0 }, // Q
		{'.','-','.', 0 , 0 , 0 , 0 }, // R
		{'.','.','.', 0 , 0 , 0 , 0 }, // S
		{'-', 0 , 0 , 0 , 0 , 0 , 0 }, // T
		{'.','.','-', 0 , 0 , 0 , 0 }, // U
		{'.','.','.','-', 0 , 0 , 0 }, // V
		{'.','-','-', 0 , 0 , 0 , 0 }, // W
		{'-','.','.','-', 0 , 0 , 0 }, // X
		{'-','.','-','-', 0 , 0 , 0 }, // Y
		{'-','-','.','.', 0 , 0 , 0 }, // Z

		{'-','-','-','-','-', 0 , 0 }, // 0
		{'.','-','-','-','-', 0 , 0 }, // 1
		{'.','.','-','-','-', 0 , 0 }, // 2
		{'.','.','.','-','-', 0 , 0 }, // 3
		{'.','.','.','.','-', 0 , 0 }, // 4
		{'.','.','.','.','.', 0 , 0 }, // 5
		{'-','.','.','.','.', 0 , 0 }, // 6
		{'-','-','.','.','.', 0 , 0 }, // 7
		{'-','-','-','.','.', 0 , 0 }, // 8
		{'-','-','-','-','.', 0 , 0 }, // 9

		{'.','-','.','-','.','-', 0 }, // .
		{'-','-','.','.','-','-', 0 }, // ,
		{'.','.','-','-','.','.', 0 }, // ?
		{'.','-','-','-','-','.', 0 }, // '
		{'-','.','-','.','-','-', 0 }, // !
		{'-','.','.','-','.', 0 , 0 }, // /
		{'-','.','-','-','.', 0 , 0 }, // (
		{'-','.','-','-','.','-', 0 }, // )
		{'.','-','.','.','.', 0 , 0 }, // &
		{'-','-','-','.','.','.', 0 }, // :
		{'-','.','-','.','-','.', 0 }, // ;
		{'-','.','.','.','-', 0 , 0 }, // |
		{'.','-','.','-','.', 0 , 0 }, // +
		{'-','.','.','.','.','-', 0 }, // -
		{'.','.','-','-','.','-', 0 }, // _
		{'.','-','.','.','-','.', 0 }, // "
		{ 0 , 0 , 0 , 0 , 0 , 0 , 0 }, // $
		{'.','-','-','.','-','.', 0 }  // @

};

// morse global variables
typedef struct {
	char message_buf[MORSE_BUFFER_LENGTH];

	// timing constraints
	uint16_t dit_time_ticks;

	// cycles
	uint8_t in_chr_index;
	uint8_t in_msg_index;

	uint8_t busy;

	PORT_t *port;
	uint8_t pin;

	uint8_t init;
} morse_global_t;

static volatile morse_global_t morse;

#define MORSE_PIN_ON(__port, __pin) (__port)->OUT &= ~(__pin)
#define MORSE_PIN_OFF(__port, __pin) (__port)->OUT |= (__pin)
#define MORSE_SET_TIMER(__pri, __pw) \
	do{ \
		TCD1.PER = morse.dit_time_ticks * (__pri); \
		TCD1.CCA = morse.dit_time_ticks * (__pw); \
		TCD1.CTRLFSET = TC_CMD_RESTART_gc; \
	}while(0)

void encoder_init(uint8_t downlink_wpm, PORT_t *port, uint8_t pin)
{
	// counts at 0.001Hz (per=1ms)
	TCD1.CTRLA = TC_CLKSEL_DIV64_gc;

	morse.port = port;
	morse.pin  = pin;

	// set up timing (250 ticks = 1ms)
	morse.dit_time_ticks = (F_CPU / 64) * (1.2 / downlink_wpm);

	debug_text("Morse Encoding Rate: %d\n", morse.dit_time_ticks);

	// set morse pin as an output and turn it off
	morse.port->DIR |= morse.pin;
	MORSE_PIN_OFF(morse.port, morse.pin);
}

uint8_t encoder_is_busy(){
	return morse.busy;
}

/**
 * maps to the table above
 */
static int8_t morse_encoding_index(char ascii_char) {
	if ((ascii_char >= 'A') && (ascii_char <= 'Z')) {
		return ascii_char - 'A';
	}else if ((ascii_char >= '0') && (ascii_char <= '9')) {
		return ascii_char - '0' + ('Z' - 'A' + 1);
	}else{
		switch (ascii_char) {
		case '.':
			return 36;
		case ',':
			return 37;
		case '?':
			return 38;
		case '\'':
			return 39;
		case '!':
			return 40;
		case '/':
			return 41;
		case '(':
			return 42;
		case ')':
			return 43;
		case '&':
			return 44;
		case ':':
			return 45;
		case ';':
			return 46;
		case '=':
			return 47;
		case '+':
			return 48;
		case '-':
			return 49;
		case '_':
			return 50;
		case '"':
			return 51;
		case '$':
			return 52;
		case '@':
			return 53;
		default:
			return -1;
		}
	}

}

static void encoder_send_morse_message();

/**
 * returns 0 on failure, 1 on success
 */
uint8_t encoder_send(char *fmt, ...)
{
	va_list ap;

	if(morse.busy){
		return 0;
	}

	morse.busy = 1;

	va_start(ap, fmt);
	vsnprintf((char*)morse.message_buf, MORSE_BUFFER_LENGTH, fmt, ap);
	va_end(ap);

	morse.in_chr_index = 0;
	morse.in_msg_index = 0;

	// send within 2 dits of receiving
	MORSE_SET_TIMER(1,1);
	TCD1.INTCTRLA = TC_OVFINTLVL_MED_gc;
	TCD1.INTCTRLB = TC_CCAINTLVL_MED_gc;

	return 1;
}

static inline void encoder_cleanup(){
	TCD1.INTCTRLA = 0;
	TCD1.INTCTRLB = 0;
	morse.in_msg_index = 0;
	morse.in_chr_index = 0;
	morse.busy = 0;
}

static void encoder_send_morse_message(){
	char letter = morse.message_buf[morse.in_msg_index];
	int8_t enc;
	char sym;

	// determine dit, dah or space:
	if(letter == '\0'){
		// finished message, cleanup
		encoder_cleanup();
		return;
	}

	if(letter != ' '){
		enc = morse_encoding_index(toupper(letter));
		if(enc >= 0){
			// valid encoding, get the underlying morse symbol
			sym = pgm_read_byte(&(morse_table[enc][morse.in_chr_index++]));
		}else{
			sym = -1;
		}
	}else{
		// word space
		sym = ' ';
	}

	switch(sym){
	case '.':
		// dit
		MORSE_SET_TIMER(2, 1);
		MORSE_PIN_ON(morse.port, morse.pin);
		debug_led_3(1);
		break;
	case '-':
		// dah
		MORSE_SET_TIMER(4, 3);
		MORSE_PIN_ON(morse.port, morse.pin);
		debug_led_3(1);
		break;
	case 0:
		// letter space (terminating symbol for a letter)
		// note we go 2 dit lengths here because 1 dit
		// length will have been used for the previous letter
		MORSE_SET_TIMER(2, 1);
		// increment letter
		morse.in_msg_index++;
		morse.in_chr_index = 0;
		break;
	case ' ':
		// word space
		// note we go 6 dit lengths here because 1 dit
		// length will have been used for the previous
		// letter
		MORSE_SET_TIMER(6, 2);
		morse.in_msg_index++;
		morse.in_chr_index = 0;
		break;
	default:
		// clean up (error due to invalid symbol)
		// 0, 0 turns off the timer
		encoder_cleanup();
		debug_led_r(1);
		break;
	}
}

ISR(TCD1_OVF_vect, ISR_NOBLOCK)
{
	encoder_send_morse_message();
}

ISR(TCD1_CCA_vect, ISR_NOBLOCK)
{
	// time for this symbol is up, turn the pin low
	MORSE_PIN_OFF(morse.port, morse.pin);
	debug_led_3(0);
}

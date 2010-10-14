/*
 * morse_decoder.c
 *
 *  Created on: Jan 6, 2010
 *      Author: MAELLIOT
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "debug.h"
#include "adc_driver.h"
#include "adc_util.h"

#include "encoder.h"

#include "decoder.h"

#define HF_MAX_RX_BUFFER_CHARS 128
#define HF_MAX_MESG_NAME_LEN   8
#define HF_MAX_MESSAGE_TYPES   10

#define HF_SAMPLE_BUFFER_SIZE  4096
#define HF_SAMPLE_AVERAGE_SIZE 16

static char callsign[] = "k2ram";
static uint8_t callsign_len = 5;

typedef enum {
	DECODER_STATE_NO_PULSE,
	DECODER_STATE_DETECT
} decoder_state_t;

/**
 * detect ~ noise level + 5dB
 * reject ~ noise level + 7dB
 *
 * measure these in samples:
 * morse_time_unit       = base morse time unit
 * morse_time_char_space ~ 5 * morse_time_unit (between character space)
 *
 * max_pulse_width       = max possible pulse width
 *                         recommend 5*morse_time_unit (a whole space)
 */
typedef struct {
	uint16_t detect_threshold;
	uint16_t reject_threshold;
	uint16_t morse_time_unit;
	uint16_t morse_time_char_space;
	uint16_t max_pulse_width;
} decoder_global_t;

typedef struct {
	uint8_t buf[HF_SAMPLE_BUFFER_SIZE];

	uint8_t avg[HF_SAMPLE_AVERAGE_SIZE];

	uint16_t read;
	uint16_t write;

	uint8_t avg_idx;
} adc_buffer_t;

typedef struct {
	char message_letter;
	//hf_link_msg_decoder_t decoder;
	hf_link_msg_handler_t handler;
	//void* data;
} hf_parser_t;

typedef struct {
	char rx_buffer[HF_MAX_RX_BUFFER_CHARS];
	uint8_t read;
	uint8_t write;
} hf_rx_buf_t;

typedef struct {
	// messaging
	hf_rx_buf_t rx;
	int8_t msg_counter;
	uint8_t air_segment_id;

	// up to 10 decoders
	hf_parser_t msgs[HF_MAX_MESSAGE_TYPES];
	uint8_t num_msg_types;

	// decoder
	decoder_global_t dec;

	// adc
	adc_buffer_t adc;
	int8_t offset;
} hf_global_t;

// global hf decoding link state
static volatile hf_global_t hf;

// internal functions (note static declaration localizes
// these functions to the "decoder" compilation unit)
static char decoder_decode_symbols(char *symbols, uint8_t len);
static decoder_state_t decoder_mag_detect(uint16_t val);

void decoder_init(uint8_t uplink_wpm, uint8_t downlink_wpm, PORT_t *downlink_port, uint8_t downlink_pin)
{
	// initial values
	hf.dec.detect_threshold = 90;

	// number of samples per time unit
	hf.dec.morse_time_unit       = (1.2f / uplink_wpm) * F_CPU / 512;
	hf.dec.morse_time_char_space = hf.dec.morse_time_unit * 2.5;

	debug_text("Morse Dit Length: %d\n", hf.dec.morse_time_unit);

	memset((uint8_t*)hf.adc.buf, 0, HF_SAMPLE_BUFFER_SIZE);
	memset((uint8_t*)hf.adc.avg, 0, HF_SAMPLE_AVERAGE_SIZE);

	hf.adc.read  = 0;
	hf.adc.write = 1;

	hf.adc.avg_idx = 0;

	hf.msg_counter = 0;

	PORTA.DIRSET |= PIN7_bm;

	encoder_init(downlink_wpm, downlink_port, downlink_pin);

	// Move stored calibration values to ADC A.
	ADC_CalibrationValues_Load(&ADCA);

	// Set up ADC A to have signed conversion mode and 12 bit resolution.
	ADC_ConvMode_and_Resolution_Config(&ADCA, ADC_ConvMode_Unsigned, ADC_RESOLUTION_12BIT_gc);

	// Set sample rate (~4KHz)
	ADC_Prescaler_Config(&ADCA, ADC_PRESCALER_DIV512_gc);

	// Set reference voltage on ADC A to be VCC/1.6 V.
	ADC_Reference_Config(&ADCA, ADC_REFSEL_VCC_gc);

	/* Get offset value for ADC A. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
			ADC_CH_INPUTMODE_SINGLEENDED_gc,
			ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMux_Config(&ADCA.CH0, 0, ADC_CH_MUXPOS_PIN7_gc);

	ADC_Enable(&ADCA);

	// Wait until common mode voltage is stable. Default clk is 2MHz and
	// therefore below the maximum frequency to use this function.
	ADC_Wait_8MHz(&ADCA);
	hf.offset = ADC_Offset_Get_Signed(&ADCA, &ADCA.CH0, 1);
	ADC_Disable(&ADCA);

	debug_text("ADC Offset %d\n", (int)hf.offset);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
			ADC_CH_INPUTMODE_SINGLEENDED_gc,
			ADC_DRIVER_CH_GAIN_NONE);

	// Bind virtual channel 0 to pin 0.
	ADC_Ch_InputMux_Config(&ADCA.CH0, 0, ADC_CH_MUXPOS_PIN7_gc);

	// Setup sweep of just virtual channel 0
	ADC_SweepChannels_Config(&ADCA, ADC_SWEEP_0_gc);

	// Enable low level interrupts on ADCA channel 3, on conversion complete.
	ADC_Ch_Interrupts_Config(&ADCA.CH0, ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);

	// Enable ADC A with free running mode, VCC reference and signed conversion.
	ADC_Enable(&ADCA);

	// Wait until common mode voltage is stable. Default clock is 2MHz and
	// therefore below the maximum frequency to use this function.
	ADC_Wait_8MHz(&ADCA);

	// Enable free running mode.
	ADC_FreeRunning_Enable(&ADCA);

	// Configure A/D converter on PORTA.7 (here to end of function)
	PORTA.DIRCLR |= PIN7_bm;
}

void decoder_consume_signal()
{
	uint16_t local_wrt_index = 0;
	uint8_t curr_sample;
	static uint16_t average = 0;

	SUPPRESS_ADC(&ADCA){
		local_wrt_index = hf.adc.write;
	}

	while(hf.adc.read != local_wrt_index){
		// subtract away the sample about to be overwritten
		curr_sample = hf.adc.buf[hf.adc.read];

		// 0 out
		hf.adc.buf[hf.adc.read] = 0;

		average -= hf.adc.avg[hf.adc.avg_idx];
		hf.adc.avg[hf.adc.avg_idx] = curr_sample;
		average += hf.adc.avg[hf.adc.avg_idx];

		hf.adc.avg_idx++;
		if(hf.adc.avg_idx >= HF_SAMPLE_AVERAGE_SIZE){
			hf.adc.avg_idx = 0;
		}

		SUPPRESS_ADC(&ADCA){
			hf.adc.read++;
		}

		// cycle on circular buffer for read ptr
		if(hf.adc.read >= HF_SAMPLE_BUFFER_SIZE){
			SUPPRESS_ADC(&ADCA){
				hf.adc.read = 0;
			}
		}

		decoder_mag_detect(average / HF_SAMPLE_AVERAGE_SIZE);
	}
}

static decoder_state_t decoder_mag_detect(uint16_t val)
{
	static decoder_state_t state = DECODER_STATE_NO_PULSE;
	static uint16_t pulse_width;
	static uint16_t space_width = 0;
	static char symbols[7] = {0};
	static uint8_t sym_len = 0;

	switch(state)
	{
	// no pulse detected
	case DECODER_STATE_NO_PULSE:
		if((sym_len != 0 || space_width > hf.dec.morse_time_unit)
				&& val > hf.dec.detect_threshold){
			// detected a rising edge, move to qualification
			state = DECODER_STATE_DETECT;
			pulse_width = 0;

			// debug_text("%d>", space_width);
		}
		break;

	// pulse detected and qualified
	case DECODER_STATE_DETECT:
		pulse_width++;

		if(val < hf.dec.detect_threshold / 4){
			// pulse dropped to half detect threshold, call this a
			// falling edge and exit pulse detection.
			state = DECODER_STATE_NO_PULSE;

			if((hf.dec.morse_time_unit * 0.5) < pulse_width
					&& pulse_width < (hf.dec.morse_time_unit * 1.5)){
				symbols[sym_len] = '.';
				sym_len++;
				space_width = 0;
			}else if((hf.dec.morse_time_unit * 2.5) < pulse_width
					&& pulse_width < (hf.dec.morse_time_unit * 3.5)){
				symbols[sym_len] = '-';
				sym_len++;
				space_width = 0;
			}
		}
		break;
	}

	// track spaces
	switch(state){
	case DECODER_STATE_NO_PULSE:
		if(space_width < 4096){
			space_width++;
		}

		// parse characters as required
		if(sym_len >= 6 || (sym_len > 0 && space_width > hf.dec.morse_time_char_space)){
			// either we're out of room to store more symbols (max 5 symbols)
			// or we've exceeded the char space - indicating we've got a between
			// character break.
			char sym;

			symbols[sym_len] = '\0';
			sym = decoder_decode_symbols(symbols, sym_len);

			debug_text("%c", sym);

			// add letters to circular rx buffer (while space allows)
			if(hf.rx.write + 1 != hf.rx.read){
				hf.rx.rx_buffer[hf.rx.write++] = sym;
				if(hf.rx.write >= HF_MAX_RX_BUFFER_CHARS){
					hf.rx.write = 0;
				}
			}
			// else, drop characters on the floor (we're out of room!)

			// reset
			sym_len     = 0;
			symbols[0]  = '\0';
			space_width = 0;
		}
		break;
	default:
		// do nothing
		break;
	}

	// return true when we're in a pulse
	return state;
}

ISR(ADCA_CH0_vect, ISR_NOBLOCK)
{
	uint16_t sample;
#if DOWNSAMP_NUM > 0
	static uint8_t downsampler = 0;

	downsampler++;
	if(downsampler < DOWNSAMP_NUM){
		// drop sample on the floor
		return;
	}else{
		downsampler = 0;
	}
#endif



	// if we're about to overwrite the read index drop the sample while
	// we catch up:
	if(encoder_is_busy() == 0
			&& (hf.adc.read != 0 || hf.adc.write != HF_SAMPLE_BUFFER_SIZE-1)
			&& hf.adc.write + 1 != hf.adc.read){
		sample = ADCA.CH0.RES - hf.offset;
		if(sample >= 4096){
			sample = 4095;
		}

		hf.adc.buf[hf.adc.write] = (4096 - sample) >> 4;

		// debug_text("%d\n", hf.adc.buf[hf.adc.write]);

		hf.adc.write++;
		if(hf.adc.write >= HF_SAMPLE_BUFFER_SIZE){
			hf.adc.write = 0;
		}
	}

}

static int8_t decoder_decode_data_char(char c)
{
	switch(c)
	{
	case 'b': return 0;
	case 'c': return 1;
	case 'f': return 2;
	case 'h': return 3;
	case 'j': return 4;
	case 'l': return 5;
	case 'p': return 6;
	case 'q': return 7;
	case 'v': return 8;
	case 'x': return 9;
	}
	return -1;
}

typedef enum {
	RX_STATE_NONE,
	RX_STATE_CALLSIGN,
	RX_STATE_CMD,
	RX_STATE_DATA,
	RX_STATE_ID
} rx_state_t;

uint8_t decoder_register_handler(char message, hf_link_msg_handler_t handler)
{
	if(hf.num_msg_types >= HF_MAX_MESSAGE_TYPES){
		return 0;
	}

	hf.msgs[hf.num_msg_types].handler = handler;
	hf.msgs[hf.num_msg_types].message_letter = tolower(message);

	hf.num_msg_types++;
	return 1;
}

void decoder_process_rx(){
	static rx_state_t rx_state;

	// temporary fields for parsing
	static uint8_t cs_ind = 0;
	static uint8_t dt_ind = 0;
	static char text_data[2] = {0};

	// message fields
	static int8_t message_id;
	static char command;
	static int8_t data;

	uint8_t i;
	uint8_t local_wrt_index = hf.rx.write;

	while(hf.rx.read != local_wrt_index){

		switch(rx_state){
		case RX_STATE_NONE:
			if(callsign[0] == hf.rx.rx_buffer[hf.rx.read]){
				rx_state = RX_STATE_CALLSIGN;
				cs_ind = 1;
			}
			break;

		case RX_STATE_CALLSIGN:
			if(callsign[cs_ind] == hf.rx.rx_buffer[hf.rx.read]){
				cs_ind++;
				if(cs_ind == callsign_len){
					rx_state = RX_STATE_CMD;
				}
			}else if(callsign[0] == hf.rx.rx_buffer[hf.rx.read]){
				rx_state = RX_STATE_CALLSIGN;
				cs_ind = 1;
			}else{
				rx_state = RX_STATE_NONE;
			}
			break;

		case RX_STATE_CMD:
			command = hf.rx.rx_buffer[hf.rx.read];
			dt_ind = 0;
			rx_state = RX_STATE_DATA;
			break;

		case RX_STATE_DATA:
			text_data[dt_ind++] = hf.rx.rx_buffer[hf.rx.read];
			if(dt_ind == 2){
				if(text_data[0] == text_data[1]){
					dt_ind = 0;
					data = decoder_decode_data_char(text_data[0]);
					rx_state = RX_STATE_ID;
				}else{
					rx_state = RX_STATE_NONE;
				}
			}
			break;

		case RX_STATE_ID:
			text_data[dt_ind++] = hf.rx.rx_buffer[hf.rx.read];
			if(dt_ind == 2){
				rx_state = RX_STATE_NONE;

				hf_message_response_t reply_indicator;
				int8_t tens = decoder_decode_data_char(text_data[0]);
				int8_t ones = decoder_decode_data_char(text_data[1]);

				message_id = 10*tens + ones;

				// failure conditions:
				//  message_id < 0    (invalid symbol)
				//  data < 0          (invalid symbol)
				//  command not found (invalid command)
				//  message_id > last_known_msg_id + 10 (invalid increment)

				reply_indicator = HF_MSG_REPLY_ILL;
				if(tens >= 0
						&& ones >= 0
						&& data >= 0
						&& message_id > hf.msg_counter
						&& message_id <= (hf.msg_counter + 10)){

					hf.msg_counter = message_id;
					if(hf.msg_counter >= 99){
						hf.msg_counter = 0;
					}

					// valid message id, valid data field
					for(i = 0; i < hf.num_msg_types; i++){
						if(command == hf.msgs[i].message_letter){
							if(hf.msgs[i].handler != 0){
								reply_indicator = hf.msgs[i].handler(data);
							}else{
								reply_indicator = HF_MSG_REPLY_ACK;
							}
							break;
						}
					}
				}

				switch(reply_indicator){
				case HF_MSG_REPLY_ILL:
					encoder_send("i %d %d %s", message_id, hf.msg_counter, callsign);
					debug_text("\nill %d\n", message_id);
					break;
				case HF_MSG_REPLY_ACK:
					encoder_send("k %d %s", message_id, callsign);
					debug_text("\nack %d\n", message_id);
					break;
				default:
					// do nothing on suppress
					debug_text("\nsuppress %d\n", message_id, callsign);
					break;
				}
			}
			break;
		}

		hf.rx.read++;

		// roll over
		if(hf.rx.read >= HF_MAX_RX_BUFFER_CHARS){
			hf.rx.read = 0;
		}
	}
}

// Generic decoder function
// we might be able to do better with a custom limited decoder set
static char decoder_decode_symbols(char *symbols, uint8_t len)
{
	switch(len)
	{
	case 1:
		if(symbols[0] == '.'){
			return 'e';
		}else{
			return 't';
		}
		break;
	case 2:
		if(strncmp_P(symbols, PSTR(".."), 2) == 0){
			return 'i';
		}else if(strncmp_P(symbols, PSTR(".-"), 2) == 0){
			return 'a';
		}else if(strncmp_P(symbols, PSTR("-."), 2) == 0){
			return 'n';
		}else if(strncmp_P(symbols, PSTR("--"), 2) == 0){
			return 'm';
		}
		break;
	case 3:
		if(strncmp_P(symbols, PSTR("..."), 3) == 0){
			return 's';
		}else if(strncmp_P(symbols, PSTR("..-"), 3) == 0){
			return 'u';
		}else if(strncmp_P(symbols, PSTR(".-."), 3) == 0){
			return 'r';
		}else if(strncmp_P(symbols, PSTR(".--"), 3) == 0){
			return 'w';
		}else if(strncmp_P(symbols, PSTR("-.."), 3) == 0){
			return 'd';
		}else if(strncmp_P(symbols, PSTR("-.-"), 3) == 0){
			return 'k';
		}else if(strncmp_P(symbols, PSTR("--."), 3) == 0){
			return 'g';
		}else if(strncmp_P(symbols, PSTR("---"), 3) == 0){
			return 'o';
		}
		break;
	case 4:
		if(strncmp_P(symbols, PSTR("...."), 4) == 0){
			return 'h';
		}else if(strncmp_P(symbols, PSTR("...-"), 4) == 0){
			return 'v';
		}else if(strncmp_P(symbols, PSTR("..-."), 4) == 0){
			return 'f';
		}else if(strncmp_P(symbols, PSTR("-..."), 4) == 0){
			return 'b';
		}else if(strncmp_P(symbols, PSTR("-.-."), 4) == 0){
			return 'c';
		}else if(strncmp_P(symbols, PSTR(".---"), 4) == 0){
			return 'j';
		}else if(strncmp_P(symbols, PSTR(".-.."), 4) == 0){
			return 'l';
		}else if(strncmp_P(symbols, PSTR(".--."), 4) == 0){
			return 'p';
		}else if(strncmp_P(symbols, PSTR("--.-"), 4) == 0){
			return 'q';
		}else if(strncmp_P(symbols, PSTR("-..-"), 4) == 0){
			return 'x';
		}else if(strncmp_P(symbols, PSTR("-.--"), 4) == 0){
			return 'y';
		}else if(strncmp_P(symbols, PSTR("--.."), 4) == 0){
			return 'z';
		}
		break;
	case 5:
		if(strncmp_P(symbols, PSTR(".----"), 5) == 0){
			return '1';
		}else if(strncmp_P(symbols, PSTR("..---"), 5) == 0){
			return '2';
		}else if(strncmp_P(symbols, PSTR("...--"), 5) == 0){
			return '3';
		}else if(strncmp_P(symbols, PSTR("....-"), 5) == 0){
			return '4';
		}else if(strncmp_P(symbols, PSTR("....."), 5) == 0){
			return '5';
		}else if(strncmp_P(symbols, PSTR("-...."), 5) == 0){
			return '6';
		}else if(strncmp_P(symbols, PSTR("--..."), 5) == 0){
			return '7';
		}else if(strncmp_P(symbols, PSTR("---.."), 5) == 0){
			return '8';
		}else if(strncmp_P(symbols, PSTR("----."), 5) == 0){
			return '9';
		}else if(strncmp_P(symbols, PSTR("-----"), 5) == 0){
			return '0';
		}
		break;
	}
	// brick the character, it didn't match any of our returns
	return '#';
}

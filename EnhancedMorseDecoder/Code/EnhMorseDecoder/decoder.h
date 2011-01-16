/*
 * decoder.h
 *
 *  Created on: Jan 10, 2010
 *      Author: maelliot
 */

#ifndef DECODER_H_
#define DECODER_H_

#define DECODER_SAMPLING_FREQUENCY 3906.25

#include "hf_types.h"
#include "c_def_types.h"

void decoder_init(uint8_t uplink_wpm, uint8_t downlink_wpm );  //sww removed  /*, PORT_t *downlink_port, uint8_t downlink_pin)*/
void decoder_consume_signal();
void decoder_process_rx();
uint8_t decoder_register_handler(char message, hf_link_msg_handler_t handler);


#endif /* DECODER_H_ */

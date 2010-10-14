/*
 * hfm_encoders.c
 *
 *  Created on: Mar 21, 2010
 *      Author: maelliot
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <avr/pgmspace.h>

#include "encoder.h"
#include "hfm_encoders.h"
#include "debug.h"

#define rabs(x) ((x)>=0?(long)((x)+0.5):(long)(-(x)+0.5))

void hfm_send_periodic_status(float lat, float lon, uint32_t alt, float voltage, float temp, uint8_t inc_counter)
{
	static uint32_t periodic_index = 0;
	uint16_t multiplier = (alt < 10000) ? 10000 : 1000;

	encoder_send("%s %lu %c %ld  %c %ld  %lu  %u  %u",
			"K2RAM",
			(inc_counter ? periodic_index++ : 0),
			lat > 0 ? 'N' : 'S', rabs(lat * multiplier),
			lon < 0 ? 'W' : 'E', rabs(lon * multiplier),
			(uint32_t)(alt/100),
			(uint16_t)(voltage*10),
			(uint16_t)(temp/10));

}

void hfm_send_other_vehicle_status(uint8_t id, float lat, float lon, uint32_t alt)
{
	uint16_t multiplier = (alt < 10000) ? 10000 : 1000;

	encoder_send("%s %c %ld  %c %ld  %d  %d  %d",
			"N2XE",
			lat > 0 ? 'N' : 'S', rabs(lat * multiplier),
			lon < 0 ? 'W' : 'E', rabs(lon * multiplier),
			(unsigned int)(alt/100));

}

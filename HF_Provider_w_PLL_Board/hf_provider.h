/*
 * hf_provider.h
 *
 *  Created on: Jan 3, 2010
 *      Author: maelliot
 */

#ifndef HF_PROVIDER_H_
#define HF_PROVIDER_H_

#define NUM_TRACKED_VEH 4

typedef struct {
	uint8_t id;
	long utc;
	float lat;
	float lon;
	float alt;
} location_t;

/**
 * payload_abort_waiting: -2  abort commanded, executed, ack from power received
 *                        -1  no abort commanded
 *                         0+ time until next abort request shall be sent
 */
typedef struct {
	location_t curr_loc;
	location_t vehicles[NUM_TRACKED_VEH];
	float battery_voltage;
	float temperature;
	uint16_t next_transmission_sec;
	uint16_t transmission_period;
	int8_t payload_abort_waiting;
	int8_t ballast_abort_waiting;
	uint8_t edit_lock;
	uint8_t transmit_now;
	uint8_t num_vehicles;
} hf_global_data_t;

#endif /* HF_PROVIDER_H_ */

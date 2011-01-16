/*
 * gps_provider.c
 *
 *  Created on: Dec 26, 2009
 *      Author: maelliot
 */
// AV1: 3/9
// AV2: -
// AV3: 3/9/5
// AV4: -
#define AS_NUMBER           3
#define PAYLOAD_ABORT_CODE  9
#define BALLAST_ABORT_CODE  5

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdio.h>

#include "debug.h"
#include "wdt_util.h"
#include "bus.h"

#include "mesg_gps.h"
#include "mesg_power.h"
#include "mesg_abort.h"
#include "mesg_voltage.h"
#include "mesg_drop.h"
#include "mesg_temp.h"
#include "mesg_veh.h"

#include "hfm_encoders.h"

#include "decoder.h"
#include "encoder.h"
#include "hf_provider.h"

#define BUS_ADDRESS 15
#define HEARTBEAT   0
#define DL_OFFSET_SEC (300+60*(AS_NUMBER-3))

static volatile hf_global_data_t globdat;

void gps_topic_cb(mesg_gps_t *gpsdata)
{
	// if we're not transmitting data right now...
	if(globdat.edit_lock == 0 && gpsdata->utc != globdat.curr_loc.utc && gpsdata->alt != 0){
		globdat.curr_loc.lat = gpsdata->lat;
		globdat.curr_loc.lon = gpsdata->lon;
		globdat.curr_loc.alt = gpsdata->alt;
		globdat.curr_loc.utc = gpsdata->utc;

		// 10 minute mark? (note we ignore this if alt == 0)
		if((gpsdata->utc + DL_OFFSET_SEC) % 600 == 0){
			globdat.transmit_now = 1;
		}
	}
}

void bat_topic_cb(mesg_voltage_t *voltdata){
	globdat.battery_voltage = voltdata->voltage;
}

void temp_topic_cb(mesg_temp_t *temp){
	globdat.temperature = temp->temperature;
}

void veh_topic_cb(mesg_veh_t *data){
	uint8_t i;
	int8_t index = -1;
	for(i = 0; i < globdat.num_vehicles; i++){
		if(data->id == globdat.vehicles[i].id){
			index = i;
			break;
		}
	}
	if(index < 0 && globdat.num_vehicles < NUM_TRACKED_VEH){
		index = globdat.num_vehicles++;
	}
	if(index >= 0){
		globdat.vehicles[index].id  = data->id;
		globdat.vehicles[index].utc = data->utc;
		globdat.vehicles[index].lat = data->lat;
		globdat.vehicles[index].lon = data->lon;
		globdat.vehicles[index].alt = data->alt;
	}
}

void abort_ack_topic_cb(mesg_abort_ack_t *ack){
	// got abort ack, turn off waiting flag, indicate abort received
	switch(ack->device){
	case ABORT_CMD_BALLAST:
		globdat.ballast_abort_waiting = -2;
		break;
	case ABORT_CMD_PAYLOAD:
		globdat.payload_abort_waiting = -2;
		break;
	}
}

uint8_t hfp_abort_payload_handler(uint8_t data){
	if(data == PAYLOAD_ABORT_CODE){
		globdat.payload_abort_waiting = 0;
		return HF_MSG_REPLY_ACK;
	}
	return HF_MSG_REPLY_ILL;
}

uint8_t hfp_abort_ballast_handler(uint8_t data){
	if(data == BALLAST_ABORT_CODE){
		globdat.ballast_abort_waiting = 0;
		return HF_MSG_REPLY_ACK;
	}
	return HF_MSG_REPLY_ILL;
}


typedef struct {
	uint8_t quantity;
	int8_t waiting;
} hf_drop_cmd_t;
static hf_drop_cmd_t hf_ballast_drop;
uint8_t hfp_drop_ballast_handler(uint8_t quantity){
	hf_ballast_drop.quantity = quantity;
	hf_ballast_drop.waiting  = 0;
	return HF_MSG_REPLY_ACK;
}
void hfp_failed_ballast_drop_handler(){
	hf_ballast_drop.waiting = 1;
}

// Power Commands:
typedef struct{
	uint8_t command;
	int8_t waiting;
} hf_power_cmd_t;
static hf_power_cmd_t hf_power_on;
static hf_power_cmd_t hf_power_off;
void hfp_failed_power_on_handler(){
	hf_power_on.waiting = 1;
}
uint8_t hfp_power_on_handler(uint8_t device){
	hf_power_on.command = device;
	hf_power_on.waiting = 0;
	return HF_MSG_REPLY_ACK;
}

void hfp_failed_power_off_handler(){
	hf_power_off.waiting = 1;
}
uint8_t hfp_power_off_handler(uint8_t device){
	hf_power_off.command = device;
	hf_power_off.waiting = 0;
	return HF_MSG_REPLY_ACK;
}

uint8_t hfp_status_request_handler(uint8_t junk){
	hfm_send_periodic_status(globdat.curr_loc.lat,
			globdat.curr_loc.lon,
			globdat.curr_loc.alt,
			globdat.battery_voltage,
			globdat.temperature, 0);
	return HF_MSG_REPLY_SUPPRESS;
}

uint8_t hfp_other_veh_position_handler(uint8_t id){
	uint8_t i;
	for(i = 0; i < globdat.num_vehicles; i++){
		if(globdat.vehicles[i].id == id){
			hfm_send_other_vehicle_status(
					globdat.vehicles[i].id,
					globdat.vehicles[i].lat,
					globdat.vehicles[i].lon,
					globdat.vehicles[i].alt);
			break;
		}
	}
	return HF_MSG_REPLY_SUPPRESS;
}

int main(int argc, char **argv)
{

	// Message Data Records (for outbound bus messages)
	mesg_abort_t payload_abort;
	mesg_abort_t ballast_abort;
	mesg_power_t power_off_frame;
	mesg_power_t power_on_frame;
	mesg_drop_t  drop_frame;

	// counts at 1Hz
	TCC0.PER = 31250;
	TCC0.CNT = 0x0;
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;

	// initialize debug output
	debug_init();

	debug_led_r(1);

	// initialize morse link
	decoder_init(16, 15, &PORTF, PIN6_bm);

	decoder_register_handler('x', &hfp_abort_payload_handler); // abort payload
	decoder_register_handler('b', &hfp_abort_ballast_handler); // abort ballast
	decoder_register_handler('c', &hfp_drop_ballast_handler); // drop ballast
	decoder_register_handler('f', &hfp_status_request_handler); // request status
	decoder_register_handler('r', 0); // ack (no handler, suppress)
	decoder_register_handler('l', 0); // illegal command (no handler, suppress)
	decoder_register_handler('p', &hfp_power_on_handler); // power on device
	decoder_register_handler('q', &hfp_power_off_handler); // power off device
	decoder_register_handler('v', &hfp_other_veh_position_handler); // other vehicle position

	// initialize service bus
	bus_init(BUS_ADDRESS);

	bus_register(MESG_GPS_NAME,     &gps_topic_cb);
	bus_register(MESG_VOLTAGE_NAME, &bat_topic_cb);
	bus_register(MESG_ABACK_NAME,   &abort_ack_topic_cb);
	bus_register(MESG_TEMP_NAME,    &temp_topic_cb);
	bus_register(MESG_VEH_NAME,     &veh_topic_cb);

	// enable all interrupt levels (this doesn't turn them on)
	PMIC.CTRL = PMIC_RREN_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;

	globdat.ballast_abort_waiting = -1;
	globdat.payload_abort_waiting = -1;

	globdat.next_transmission_sec = 105;  // transmit 30s after power up
	globdat.transmission_period   = 610; // 10 minutes + 10s

	globdat.edit_lock = 0;
	globdat.transmit_now = 0;

	globdat.num_vehicles = 0;

	hf_power_on.waiting     = -1;
	hf_power_off.waiting    = -1;
	hf_ballast_drop.waiting = -1;

	debug_text("HF Provider Initialized\n");

	_delay_ms(250);
	debug_led_r(0);

	// turn interrupts on
	sei();

	wdt_enable(WDT_PER_8KCLK_gc);

	while (1) {


		// transmit now! (either we got a flag to transmit, or 11 minutes has expired)
		if(globdat.transmit_now || globdat.next_transmission_sec == 0){
			globdat.edit_lock = 1;
			// transmit HF status message:
			debug_text("Sending Periodic Status\n");
			hfm_send_periodic_status(globdat.curr_loc.lat,
					globdat.curr_loc.lon, globdat.curr_loc.alt,
					globdat.battery_voltage, globdat.temperature, 1);
			globdat.transmit_now = 0;
			globdat.next_transmission_sec = globdat.transmission_period;
			globdat.edit_lock = 0;
		}

		if(globdat.payload_abort_waiting == 0){
			// transmit payload abort on bus
			busm_send_abort(BUS_ADDRESS, &payload_abort, ABORT_CMD_PAYLOAD, 0, 0);
			// do this every 5 seconds until we receive an ack
			globdat.payload_abort_waiting = 5;
		}

		if(globdat.ballast_abort_waiting == 0){
			// transmit payload abort on bus
			busm_send_abort(BUS_ADDRESS, &ballast_abort, ABORT_CMD_BALLAST, 0, 0);
			// do this every 5 seconds until we receive an ack
			globdat.ballast_abort_waiting = 5;
		}

		if(hf_power_on.waiting == 0){
			hf_power_on.waiting = -1;
			busm_send_power(BUS_ADDRESS, &power_on_frame, hf_power_on.command, 1, 0, &hfp_failed_power_on_handler);
		}

		if(hf_power_off.waiting == 0){
			hf_power_off.waiting = -1;
			busm_send_power(BUS_ADDRESS, &power_off_frame, hf_power_off.command, 0, 0, &hfp_failed_power_off_handler);
		}

		if(hf_ballast_drop.waiting == 0){
			hf_ballast_drop.waiting = -1;
			busm_send_drop(BUS_ADDRESS, &drop_frame, hf_ballast_drop.quantity, 0, &hfp_failed_ballast_drop_handler);
		}

		if(bus_time_since_last_message() == 255){
			debug_text("Resetting for lack of bus traffic\n");
			for(;;);
		}

		wdt_reset();

		// consume signal as fast as possible (every time through loop)
		decoder_consume_signal();
		decoder_process_rx();

		wdt_reset();
	}

	return 0;
}

ISR(TCC0_OVF_vect)
{
	// if we're not transmitting
	if (globdat.transmit_now == 0 && globdat.next_transmission_sec > 0)
		globdat.next_transmission_sec--;

	if(hf_power_on.waiting > 0)
		hf_power_on.waiting--;

	if(hf_power_off.waiting > 0)
		hf_power_off.waiting--;

	if(hf_ballast_drop.waiting > 0)
		hf_ballast_drop.waiting--;
}

ISR(BADISR_vect, ISR_NOBLOCK)
{
	debug_text("Bad ISR Fired\n");
}

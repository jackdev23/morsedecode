/*
 * hfm_encoders.h
 *
 *  Created on: Mar 21, 2010
 *      Author: maelliot
 */

#ifndef HFM_ENCODERS_H_
#define HFM_ENCODERS_H_

void hfm_send_periodic_status(float lat, float lon, uint32_t alt, float voltage, float temp, uint8_t inc_counter);
void hfm_send_other_vehicle_status(uint8_t id, float lat, float lon, uint32_t alt);

#endif /* HFM_ENCODERS_H_ */

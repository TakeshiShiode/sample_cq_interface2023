/*
 * safeguard.h
 *
 *  Created on: 2022/03/18
 *      Author: assistmotion01
 */

#ifndef SAFEGUARD_H_
#define SAFEGUARD_H_

void initialize_poe(void);
void initialize_wdt(void);
void clear_wdt(void);
void clear_overcurrent_req(void);
void check_bldc_error(float iu, float iv, float iw, float vdc, float speed);
unsigned short get_bldc_error(void);
void reset_bldc_error(void);
#endif /* SAFEGUARD_H_ */

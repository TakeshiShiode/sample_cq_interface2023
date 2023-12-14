/*
 * sns.h
 *
 *  Created on: 2022/03/18
 *      Author: assistmotion01
 */

#ifndef SNS_H_
#define SNS_H_

void initialize_sns(void);
void get_mot_current(float *iu, float *iw, float *vdc);
float get_mot_vdc(void);
float get_vr_speed(void);
unsigned char get_start_sw(void);
unsigned char get_reset_sw(void);
void led1_on(void);
void led2_on(void);
void led1_off(void);
void led2_off(void);

#endif /* SNS_H_ */

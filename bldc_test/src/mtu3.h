/*
 * mtu3.h
 *
 *  Created on: 2022/03/16
 *      Author: sshio
 */

#ifndef MTU3_H_
#define MTU3_H_

#define CURRENT_ADJ_CNT (100)

void initialize_mtu34(void);
void initialize_pwm(void);
void initialize_pwm(void);
void initialize_mot_interrupt(void);
void initialize_encoder(void);
void start_pwm(void);
void stop_pwm(void);
float get_bldc_angle(void);
float get_bldc_speed(void);
void set_encd_pos(float set_pos);
int get_current_adjust_cnt(void);

#endif /* MTU3_H_ */

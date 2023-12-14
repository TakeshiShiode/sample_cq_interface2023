/*
 * cmt.h
 *
 *  Created on: 2022/03/16
 *      Author: sshio
 */

#ifndef CMT_H_
#define CMT_H_

#define		CURADJUST					(1)
#define		START1						(2)
#define		ADJUST1						(3)
#define		START2						(4)
#define		ADJUST2						(5)
#define		DRIVE						(6)
#define		BRAKE						(7)
#define		START_WAIT					(8)
#define		STANBY						(0)


void initialize_cmt(void);
void start_cmt(void);
void stop_cmt(void);
void reset_bldc_drive(void);
void get_dq_ref(float *pid_ref, float *piq_ref);
int get_bldc_drive_state(void);
void set_bldc_drive_state(int drive_state);
float get_bldc_refspeed(void);
void bldc_speed_interrupt(void);

#endif /* CMT_H_ */

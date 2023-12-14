/*
 * cmt.c
 *
 *  Created on: 2022/03/01
 *      Author: assistmotion01
 */
#include <machine.h>
#include "iodefine.h"
#include "mtu3.h"
#include "cmt.h"
#include "bldc_test.h"
#include "sns.h"

#define     START_CURRENT              	(1.8f)                  /* 始動制御時ID電流 */
#define     START_TIME         			(128)                   /* 始動時間ms		 */
#define     DELTA_CURRENT            	(START_CURRENT/START_TIME)	/* 電流加算量 */
#define		ADJUST_CNT					(128)					/* 相合わせ時間ms*/

#define		BLDC_MAX_SPEED				(1466.077f)				/* 電気角での最高速度rad (2000rpm)*/
#define		BLDC_MIN_SPEED				(439.823f)				/* 電気角での最低速度rad (600rpm)*/
#define		BLDC_ACC					((BLDC_MAX_SPEED - BLDC_MIN_SPEED) / 2048.0f)
#define		BLDC_DCC					(BLDC_ACC)

#define 	BLDC_IQ_LIMIT				(3.0f)
#define		KP_SPD						(0.001)
#define		KI_SPD						(0.0005)

/* グローバル変数 */
static int gBLDCDriveST;
static float gId_ref;
static float gIq_ref;
static float gSpdsum;
static float gBLDCRefSpeed;
static float gBLDCTargeSpeedlog;
/* 割込み関数宣言 */
//#pragma interrupt (bldc_speed_interrupt(vect=28))
//static void bldc_speed_interrupt(void);

void initialize_cmt(void)
{
#if 0
	/* コンペアタイマ初期化 */
	MSTP_CMT0 = 0; 						/* CMT用モジュール停止解除 */
    CMT.CMSTR0.BIT.STR0 = 0;            /* CMT0停止 */
    CMT0.CMCR.BIT.CKS = 0;              /* クロック6MHz = 周辺クロック/8 */
    CMT0.CMCOR = 499;                  /* 1ms */

    /* 割り込み設定     */
    CMT0.CMCR.BIT.CMIE = 1;                             /* compare match interrupt (CMI0) enabled */
    ICU.IPR[0x04].BIT.IPR = 13;
    ICU.IR[28].BIT.IR = 0;
    ICU.IER[0x03].BIT.IEN4 = 1;
#endif
}

void reset_cmt(void)
{
	gBLDCDriveST = STANBY;
	gId_ref		 = 0;
	gIq_ref		 = 0;
	gSpdsum		 = 0;

}

void start_cmt(void)
{
	//CMT.CMSTR0.BIT.STR0 = 1;                            /* タイマ開始 */
}

void stop_cmt(void)
{
	//CMT.CMSTR0.BIT.STR0 = 1;                            /* タイマ停止 */
}

void clear_cmt0_interrupt(void)
{
//    ICU.IR[28].BIT.IR = 0;                              /* cmt0割込み要求クリア */
}

void reset_bldc_drive(void)
{
	/* 状態遷移初期化 */
	gBLDCDriveST = STANBY;
	/* 変数初期化 */
}

void get_dq_ref(float *pid_ref, float *piq_ref)
{
	*pid_ref = gId_ref;
	*piq_ref = gIq_ref;
}

float speed_pi_ctrl(float speed_ref, float speed, float kp, float ki, float limit, float *psum)
{
	/* ------------- vd出力---------------- */
	float iref, iref_p, iref_i;
	float spd_diff;

	spd_diff = speed_ref - speed;

	/* P項 */
 	iref_p =kp * spd_diff;

 	/* I項 */
 	iref_i = *psum;
 	iref_i = iref_i + ki * spd_diff;

	if(iref_i > limit){
		iref_i = limit;
	}
	else if(iref_i < (-limit) ){
		iref_i = -limit;
	}
	*psum = iref_i;
	/* PI出力 */
	iref = iref_p + iref_i;
	if(iref > limit){
		iref = limit;
	}
	else if(iref < (-limit) ){
		iref = -limit;
	}
	return iref;
}

void bldc_speed_interrupt(void)
{
	float	bldc_speed = 0;
	static int sBLDCAdustCnt = 0;
	static float sBLDCTargetSpeed = 0;
	int cur_adjust_cnt = 0;
	static int sStartWaitCnt = 0;

	//clrpsw_i();
	switch (gBLDCDriveST){
		case STANBY:
			reset_mtu3();
			reset_cmt();
			gBLDCTargeSpeedlog = 0; 
			gBLDCDriveST = CURADJUST;
			break;
		case CURADJUST:
			cur_adjust_cnt = get_current_adjust_cnt();
			if( (cur_adjust_cnt >= CURRENT_ADJ_CNT) && (get_bldc_start() == BLDC_START_REQ) ){
				gBLDCDriveST = START1;
				bldc_start();
			}
			break;
		case START1:
			if(gId_ref > START_CURRENT){
				sBLDCAdustCnt = 0;
				gBLDCDriveST = ADJUST1;
				//tprintf("gBLDCDrive_ST=%d\n",gBLDCDriveST);
			}
			else{
				gId_ref = gId_ref + DELTA_CURRENT;
			}
			break;
		case ADJUST1:
			if(sBLDCAdustCnt > ADJUST_CNT){
				set_encd_pos(0);
				gId_ref = 0;
				gBLDCDriveST = START2;
				//tprintf("gBLDCDrive_ST=%d\n",gBLDCDriveST);
			}
			else{
				sBLDCAdustCnt++;
			}
			break;
		case START2:
			if(gId_ref > START_CURRENT){
				sBLDCAdustCnt = 0;
				gBLDCDriveST = ADJUST2;
				//tprintf("gBLDCDrive_ST=%d\n",gBLDCDriveST);

			}
			else{
				gId_ref = gId_ref + DELTA_CURRENT;
			}
			break;
		case ADJUST2:
			if(sBLDCAdustCnt > ADJUST_CNT){
				gId_ref = 0;
				
				clrpsw_i();
				sBLDCTargetSpeed = get_vr_speed();
				setpsw_i();
				
				gBLDCRefSpeed = 0;
				//gBLDCDriveST = DRIVE;
				gBLDCDriveST = START_WAIT;
				sStartWaitCnt = 0;
				//tprintf("gBLDCDrive_ST=%d\n",gBLDCDriveST);
			}
			else{
				sBLDCAdustCnt++;
			}
			break;
		case START_WAIT:
			if(sStartWaitCnt > 1){
				gBLDCDriveST = DRIVE;
			}
			sStartWaitCnt++;
			break;
		case DRIVE:
		
			clrpsw_i();
			sBLDCTargetSpeed = get_vr_speed();
			setpsw_i();
			
			
			if(sBLDCTargetSpeed > gBLDCRefSpeed){
				gBLDCRefSpeed = gBLDCRefSpeed + BLDC_ACC;
				if(gBLDCRefSpeed > sBLDCTargetSpeed){
					gBLDCRefSpeed = sBLDCTargetSpeed;
				}
			}
			else if(sBLDCTargetSpeed < gBLDCRefSpeed){
				gBLDCRefSpeed = gBLDCRefSpeed - BLDC_DCC;
				if(gBLDCRefSpeed < sBLDCTargetSpeed){
					gBLDCRefSpeed = sBLDCTargetSpeed;
				}
			}
			
			bldc_speed = get_bldc_speed();

			if(gBLDCTargeSpeedlog < bldc_speed){
				gBLDCTargeSpeedlog = bldc_speed;
			}
						
			gIq_ref = speed_pi_ctrl(gBLDCRefSpeed, bldc_speed, KP_SPD, KI_SPD, BLDC_IQ_LIMIT, &gSpdsum);
			if(get_bldc_start() == BLDC_STOP_REQ){
				gBLDCDriveST = BRAKE;
				//tprintf("gBLDCDrive_ST=%d\n",gBLDCDriveST);
			}
			break;
		case BRAKE:
			gBLDCRefSpeed = gBLDCRefSpeed - BLDC_DCC;
			if(gBLDCRefSpeed < 0){
				gBLDCRefSpeed = 0;
			}
			bldc_speed = get_bldc_speed();
			gIq_ref = speed_pi_ctrl(gBLDCRefSpeed, bldc_speed, KP_SPD, KI_SPD, BLDC_IQ_LIMIT, &gSpdsum);
			if(get_bldc_start() == BLDC_START_REQ){
				
				clrpsw_i();
				sBLDCTargetSpeed = get_vr_speed();
				setpsw_i();
				
				gBLDCRefSpeed = 0;
				gBLDCDriveST = DRIVE;
				//tprintf("gBLDCDrive_ST=%d",gBLDCDriveST);
			}
			break;
		default:
			bldc_stop();
			break;
	}
	//tprintf("refspeed=%f, speed = %f, iq_ref=%f\n", gBLDCRefSpeed, bldc_speed, gIq_ref);
	clear_cmt0_interrupt();
	//setpsw_i();
}

void set_bldc_drive_state(int drive_state)
{
	gBLDCDriveST = drive_state;
}

int get_bldc_drive_state(void)
{
	return gBLDCDriveST;
}

float get_bldc_refspeed(void)
{
	return gBLDCRefSpeed;
}

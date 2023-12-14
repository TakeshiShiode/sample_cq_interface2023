/*
 * sns.c
 *
 *  Created on: 2022/03/01
 *      Author: assistmotion01
 */


#include <iodefine.h>
#include "bldc_test.h"
#include "sns.h"

#define     NT_VDC_OFFSET               (12)                            /* vdc/2 */
#define     NT_CURRENT_OFFSET           (0x7FF)                         /* 0[A]点オフセット */
#define     AD_CURRENT_COF         		(20.0f/4095.0f)                 /* AD→電流変換係数 */
#define     AD_VDC_COF             		(30.0f/4095.0f)  				/* AD→電圧変換係数*/
/* 抵抗→速度変換係数 */
#define		VR_SPD_COF					((BLDC_MARGIN_MAX_RPM - BLDC_MARGIN_MIN_RPM) / 4095.0f)
#define		RPM_RAD_COF					((2*3.14159265f)/60)

#define VDCCNT (1000)
float gTgSpd;
void initialize_sns(void)
{
//	int i;
//	/* port側で設定 */
//    DebugCnt = 0;
//    for(i = 0; i<VDCCNT; i++){
//		DebugVdc[i] = 0.0;;
//	}
}

void get_mot_current(float *iu, float *iw, float *vdc)
{
	float ad_iu;
	float ad_iw;
  	float ad_vdc;
  	
  	S12AD0.ADCSR.BIT.ADST = 1;   
	while (S12AD0.ADCSR.BIT.ADST == 1);		/* 変換待ち */
    ad_iu = (float)S12AD0.ADDR0A;				/* センサ値取得 */
    ad_iw = (float)S12AD0.ADDR1;
    ad_vdc = (float)S12AD0.ADDR2;

    //f(DebugCnt < VDCCNT){
	//	DebugVdc[DebugCnt] = ad_iw;
	//
	//ebugCnt++;
	
    /* AD→電流値変換 */
    ad_iu = ad_iu - NT_CURRENT_OFFSET;
    ad_iw = ad_iw - NT_CURRENT_OFFSET;
    ad_iu = ad_iu * AD_CURRENT_COF;
    ad_iw = ad_iw * AD_CURRENT_COF;
	/* AD→電圧変換 */
    ad_vdc = ad_vdc * AD_VDC_COF;
    
    *iu = ad_iu;
	*iw = ad_iw;
	*vdc = ad_vdc;
}

float get_mot_vdc(void)
{
	float ad_vdc;

	S12AD0.ADCSR.BIT.ADST = 1;
    while (S12AD0.ADCSR.BIT.ADST == 1);		/* 変換待ち */
    ad_vdc = (float)S12AD0.ADDR2;
    /* AD→電圧変換 */
    ad_vdc = ad_vdc * AD_VDC_COF;

    return ad_vdc;
}

float get_vr_speed(void)
{
    unsigned short temp_vr;
    float trgt_speed;

    S12AD1.ADCSR.BIT.ADST = 1;
    while (S12AD1.ADCSR.BIT.ADST == 1);		 /* 変換中... */
    temp_vr = S12AD1.ADDR2;					 /* 抵抗値取得*/

    trgt_speed = (float)temp_vr;
    trgt_speed = trgt_speed * VR_SPD_COF + BLDC_MARGIN_MIN_RPM;
    gTgSpd = trgt_speed;
    /* radに変換 */
    trgt_speed = trgt_speed * RPM_RAD_COF;
    /* 速度検査 */
    if(trgt_speed > BLDC_MAX_RAD){
    	trgt_speed = BLDC_MAX_RAD;
    }
    else if(trgt_speed < BLDC_MIN_RAD){
    	trgt_speed = BLDC_MIN_RAD;
    }
    trgt_speed = trgt_speed * BLDC_MGPOLE_PAIR;
    return (trgt_speed);
}


unsigned char get_start_sw(void)
{
	unsigned char tmp_port;

    tmp_port = PORT9.PORT.BIT.B1;
    /* 論理反転 */
    tmp_port = 1 - tmp_port;
    return (tmp_port);
}


unsigned char get_reset_sw(void)
{
	unsigned char tmp_port;

    tmp_port = PORT9.PORT.BIT.B2;
    /* 論理反転 */
    tmp_port = 1 - tmp_port;
    return (tmp_port);
}

void led1_on(void)
{
	PORTA.DR.BIT.B2 = 0;
}


void led2_on(void)
{
	PORTA.DR.BIT.B3 = 0;
}


void led1_off(void)
{
	PORTA.DR.BIT.B2 = 1;
}


void led2_off(void)
{
	PORTA.DR.BIT.B3 = 1;
}

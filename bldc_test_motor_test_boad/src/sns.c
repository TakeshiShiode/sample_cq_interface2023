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
#define     NT_CURRENT_OFFSET           (0x7FF)                         /* 0[A]�_�I�t�Z�b�g */
#define     AD_CURRENT_COF         		(20.0f/4095.0f)                 /* AD���d���ϊ��W�� */
#define     AD_VDC_COF             		(30.0f/4095.0f)  				/* AD���d���ϊ��W��*/
/* ��R�����x�ϊ��W�� */
#define		VR_SPD_COF					((BLDC_MARGIN_MAX_RPM - BLDC_MARGIN_MIN_RPM) / 4095.0f)
#define		RPM_RAD_COF					((2*3.14159265f)/60)

#define VDCCNT (1000)
float gTgSpd;
void initialize_sns(void)
{
//	int i;
//	/* port���Őݒ� */
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
	while (S12AD0.ADCSR.BIT.ADST == 1);		/* �ϊ��҂� */
    ad_iu = (float)S12AD0.ADDR0A;				/* �Z���T�l�擾 */
    ad_iw = (float)S12AD0.ADDR1;
    ad_vdc = (float)S12AD0.ADDR2;

    //f(DebugCnt < VDCCNT){
	//	DebugVdc[DebugCnt] = ad_iw;
	//
	//ebugCnt++;
	
    /* AD���d���l�ϊ� */
    ad_iu = ad_iu - NT_CURRENT_OFFSET;
    ad_iw = ad_iw - NT_CURRENT_OFFSET;
    ad_iu = ad_iu * AD_CURRENT_COF;
    ad_iw = ad_iw * AD_CURRENT_COF;
	/* AD���d���ϊ� */
    ad_vdc = ad_vdc * AD_VDC_COF;
    
    *iu = ad_iu;
	*iw = ad_iw;
	*vdc = ad_vdc;
}

float get_mot_vdc(void)
{
	float ad_vdc;

	S12AD0.ADCSR.BIT.ADST = 1;
    while (S12AD0.ADCSR.BIT.ADST == 1);		/* �ϊ��҂� */
    ad_vdc = (float)S12AD0.ADDR2;
    /* AD���d���ϊ� */
    ad_vdc = ad_vdc * AD_VDC_COF;

    return ad_vdc;
}

float get_vr_speed(void)
{
    unsigned short temp_vr;
    float trgt_speed;

    S12AD1.ADCSR.BIT.ADST = 1;
    while (S12AD1.ADCSR.BIT.ADST == 1);		 /* �ϊ���... */
    temp_vr = S12AD1.ADDR2;					 /* ��R�l�擾*/

    trgt_speed = (float)temp_vr;
    trgt_speed = trgt_speed * VR_SPD_COF + BLDC_MARGIN_MIN_RPM;
    gTgSpd = trgt_speed;
    /* rad�ɕϊ� */
    trgt_speed = trgt_speed * RPM_RAD_COF;
    /* ���x���� */
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
    /* �_�����] */
    tmp_port = 1 - tmp_port;
    return (tmp_port);
}


unsigned char get_reset_sw(void)
{
	unsigned char tmp_port;

    tmp_port = PORT9.PORT.BIT.B2;
    /* �_�����] */
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

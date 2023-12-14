/*
 * safeguard.c
 *
 *  Created on: 2022/03/03
 *      Author: assistmotion01
 */

#include "iodefine.h"
#include "bldc_test.h"

/******************************************************************************
* Interrupt vector define
******************************************************************************/
#pragma interrupt (bldc_over_current_interrupt(vect=170))
static void bldc_over_current_interrupt(void);

/* �G���[�ԍ� */
#define BLDC_NO_ERROR				(0x00)
#define BLDC_ERROR_OVERCURRENT_U	(0x01)
#define BLDC_ERROR_OVERCURRENT_V	(0x02)
#define BLDC_ERROR_OVERCURRENT_W	(0x03)
#define BLDC_ERROR_OVERVOLTAGE		(0x04)
#define BLDC_ERROR_UNDERVOLTAGE		(0x05)
#define BLDC_ERROR_OVERSPEED		(0x06)
#define BLDC_ERROR_OVERCURRENT_POE	(0x07)

/* �G���[���肵�����l */
#define BLDC_CURRENT_LIMIT			(4.0f)
#define BLDC_HIGHVOLT_LIMIT			(28.0f)
#define BLDC_LOWVOLT_LIMIT			(0)
#define BLDC_SPEED_LIMIT			(1600)

/* �ÓI�ϐ� */
unsigned int gBldc_Error;

void initialize_poe(void)
{
	/* �ߓd���G���[���o�ݒ�(MTU3,4��POE0���S��)*/
    IOPORT.PFMPOE.BIT.POE0E = 1;                        /* POE���͒[�q�Ƃ��Đݒ�  */
    POE.ICSR1.BIT.POE0M = 0x00;							/* ��������G�b�W�Ō��o */
    POE.ICSR1.BIT.PIE1 = 1;								/* ���o�������݋��� */
    ICU.IER[0x15].BIT.IEN2 = 1;							/* POE����̊����݂����� */
    ICU.IPR[0x67].BIT.IPR = 15;							/* �����ݗD��x�ŋ� */
    POE.OCSR1.BIT.OCE1 = 1;								/* MTU3�A4�o�͒[�q�n�C�C���s�[�_���X���� */
    POE.OCSR1.BIT.OIE1 = 1;								/* �o�͒Z�������݂����� */
    POE.POECR2.BIT.MTU3BDZE = 1;						/* ICSR1_POE0F�Ƀn�C�C���s�[�_���X�v������������ */
    POE.POECR2.BIT.MTU4ACZE = 1;						/* �e�o�͒[�q���n�C�C���s�[�_���X�ɂ��� */
    POE.POECR2.BIT.MTU4BDZE = 1;
}


void initialize_wdt(void)
{
	/* WDT�ݒ� */
    WDT.WRITE.WINA = 0x5A00;                            /* TCNT������ */
    WDT.WRITE.WINB = 0xA500;							/* WDT�����Ȃ� */
    WDT.WRITE.WINB = 0x5A5F;							/* TCNT�I�[�o�[�t���[��LSI���Z�b�g */
    WDT.WRITE.WINA = 0xA5FF;							/* TCSR PCLK/131072���� */
    													/* TCNT�J�n */
    													/* WDT���[�h */
}

void reset_bldc_error(void)
{
	gBldc_Error = BLDC_NO_ERROR;
}

void clear_wdt(void)
{
     WDT.WRITE.WINA = 0x5A00;                           /* WDT�N���A */
}

void clear_overcurrent_req(void)
{
    ICU.IR[170].BIT.IR = 0;                             /* �����ݗv�����N���A */
    POE.ICSR1.BIT.POE0F = 0;							/* MTU3,4�[�q�̃n�C�C���s�[�_���X��艺�� */
    POE.OCSR1.BIT.OSF1 = 0;								/* POE��A�N�e�B�u��Ԃɐݒ�	*/
}

void check_bldc_error(float iu, float iv, float iw, float vdc, float speed)
{
	/* �d������ */
    if ((iu > BLDC_CURRENT_LIMIT) || (iu < -BLDC_CURRENT_LIMIT)){
    	gBldc_Error = BLDC_ERROR_OVERCURRENT_U;     /* U���d������ */
    }
    if ((iv > BLDC_CURRENT_LIMIT) || (iv < -BLDC_CURRENT_LIMIT)){
    	gBldc_Error = BLDC_ERROR_OVERCURRENT_V;     /* V���d������ */
    }
    if ((iw > BLDC_CURRENT_LIMIT) || (iw < -BLDC_CURRENT_LIMIT)){
    	gBldc_Error = BLDC_ERROR_OVERCURRENT_W;     /* W���d������ */
    }

    /* �d���m�F */
    if (vdc > BLDC_HIGHVOLT_LIMIT){
    	gBldc_Error = BLDC_ERROR_OVERVOLTAGE;     	/* �[�q�d������ */
    }

    if (vdc < BLDC_LOWVOLT_LIMIT){
    	gBldc_Error = BLDC_ERROR_UNDERVOLTAGE;     	/* �[�q�d������ */
    }

    /* ���x���� */
    if (speed > BLDC_SPEED_LIMIT){
    	gBldc_Error = BLDC_ERROR_OVERSPEED;     	/*�@���x���� */
    }

    if(gBldc_Error != BLDC_NO_ERROR){
    	bldc_stop();
    }

}

void bldc_over_current_interrupt(void)
{
	bldc_stop();
	gBldc_Error = BLDC_ERROR_OVERCURRENT_POE;
	clear_overcurrent_req();
}

unsigned short get_bldc_error(void)
{
	return gBldc_Error;
}



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

/* エラー番号 */
#define BLDC_NO_ERROR				(0x00)
#define BLDC_ERROR_OVERCURRENT_U	(0x01)
#define BLDC_ERROR_OVERCURRENT_V	(0x02)
#define BLDC_ERROR_OVERCURRENT_W	(0x03)
#define BLDC_ERROR_OVERVOLTAGE		(0x04)
#define BLDC_ERROR_UNDERVOLTAGE		(0x05)
#define BLDC_ERROR_OVERSPEED		(0x06)
#define BLDC_ERROR_OVERCURRENT_POE	(0x07)

/* エラー判定しきい値 */
#define BLDC_CURRENT_LIMIT			(4.0f)
#define BLDC_HIGHVOLT_LIMIT			(28.0f)
#define BLDC_LOWVOLT_LIMIT			(0)
#define BLDC_SPEED_LIMIT			(1600)

/* 静的変数 */
unsigned int gBldc_Error;

void initialize_poe(void)
{
	/* 過電流エラー検出設定(MTU3,4はPOE0が担当)*/
    IOPORT.PFMPOE.BIT.POE0E = 1;                        /* POE入力端子として設定  */
    POE.ICSR1.BIT.POE0M = 0x00;							/* 立下がりエッジで検出 */
    POE.ICSR1.BIT.PIE1 = 1;								/* 検出時割込み許可 */
    ICU.IER[0x15].BIT.IEN2 = 1;							/* POEからの割込みを許可 */
    ICU.IPR[0x67].BIT.IPR = 15;							/* 割込み優先度最強 */
    POE.OCSR1.BIT.OCE1 = 1;								/* MTU3、4出力端子ハイインピーダンス許可 */
    POE.OCSR1.BIT.OIE1 = 1;								/* 出力短絡割込みを許可 */
    POE.POECR2.BIT.MTU3BDZE = 1;						/* ICSR1_POE0Fにハイインピーダンス要求が入った際 */
    POE.POECR2.BIT.MTU4ACZE = 1;						/* 各出力端子をハイインピーダンスにする */
    POE.POECR2.BIT.MTU4BDZE = 1;
}


void initialize_wdt(void)
{
	/* WDT設定 */
    WDT.WRITE.WINA = 0x5A00;                            /* TCNT初期化 */
    WDT.WRITE.WINB = 0xA500;							/* WDT発生なし */
    WDT.WRITE.WINB = 0x5A5F;							/* TCNTオーバーフローでLSIリセット */
    WDT.WRITE.WINA = 0xA5FF;							/* TCSR PCLK/131072分周 */
    													/* TCNT開始 */
    													/* WDTモード */
}

void reset_bldc_error(void)
{
	gBldc_Error = BLDC_NO_ERROR;
}

void clear_wdt(void)
{
     WDT.WRITE.WINA = 0x5A00;                           /* WDTクリア */
}

void clear_overcurrent_req(void)
{
    ICU.IR[170].BIT.IR = 0;                             /* 割込み要求をクリア */
    POE.ICSR1.BIT.POE0F = 0;							/* MTU3,4端子のハイインピーダンス取り下げ */
    POE.OCSR1.BIT.OSF1 = 0;								/* POE非アクティブ状態に設定	*/
}

void check_bldc_error(float iu, float iv, float iw, float vdc, float speed)
{
	/* 電流検査 */
    if ((iu > BLDC_CURRENT_LIMIT) || (iu < -BLDC_CURRENT_LIMIT)){
    	gBldc_Error = BLDC_ERROR_OVERCURRENT_U;     /* U相電流超過 */
    }
    if ((iv > BLDC_CURRENT_LIMIT) || (iv < -BLDC_CURRENT_LIMIT)){
    	gBldc_Error = BLDC_ERROR_OVERCURRENT_V;     /* V相電流超過 */
    }
    if ((iw > BLDC_CURRENT_LIMIT) || (iw < -BLDC_CURRENT_LIMIT)){
    	gBldc_Error = BLDC_ERROR_OVERCURRENT_W;     /* W相電流超過 */
    }

    /* 電圧確認 */
    if (vdc > BLDC_HIGHVOLT_LIMIT){
    	gBldc_Error = BLDC_ERROR_OVERVOLTAGE;     	/* 端子電圧超過 */
    }

    if (vdc < BLDC_LOWVOLT_LIMIT){
    	gBldc_Error = BLDC_ERROR_UNDERVOLTAGE;     	/* 端子電圧超過 */
    }

    /* 速度検査 */
    if (speed > BLDC_SPEED_LIMIT){
    	gBldc_Error = BLDC_ERROR_OVERSPEED;     	/*　速度超過 */
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



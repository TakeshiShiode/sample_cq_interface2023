/************************************************************************
*
* Device     : RX/RX600/RX62T
*
* File Name  : hwsetup.c
*
* Abstract   : Hardware Setup file.
*
* History    : 1.00  (2010-06-08)  [Hardware Manual Revision : 0.20]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
*  Copyright (C) 2010 Renesas Electronics Corporation and
*  Renesas Solutions Corp. All rights reserved.
*
************************************************************************/

#include "iodefine.h"
#include "sns.h"
#include "port.h"
#include "mtu3.h"
#include "cmt.h"
#include "safeguard.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void HardwareSetup(void);
#ifdef __cplusplus
}
#endif

void HardwareSetup(void)
{
/*
 BSC.CS0MOD.WORD = 0x1234;
 BSC.CS7CNT.WORD = 0x5678;
  
 SCI0.SCR.BIT.TE  = 0;
 SCI0.SCR.BIT.RE  = 0;
 SCI0.SCR.BIT.TE  = 1;
 SCI2.SSR.BIT.PER = 0;

 TMR0.TCR.BYTE = 0x12;
 TMR1.TCR.BYTE = 0x12;
 TMR2.TCR.BYTE = 0x12;
 
 P0.DDR.BYTE = 0x12;
 P1.DDR.BYTE = 0x12;
*/

	/* システムクロック初期化 */
	SYSTEM.SCKCR.BIT.ICK = 0x00;        /* メイン = 振動子12MHz×8倍(96MHz) */
    SYSTEM.SCKCR.BIT.PCK = 0x01;        /* 周辺 = 振動子12MHz×4倍(48MHz) */

	MSTP_MTU = 0;			/* MTU用モジュール停止解除 */
    MSTP_S12AD0 = 0;
    MSTP_S12AD1 = 0;
    
	/* モータタイマ初期化(電流制御) */
    initialize_mtu34();
    initialize_pwm();
    initialize_mot_interrupt();
    initialize_encoder();
    /* モータタイマ初期化(速度制御) */
    initialize_cmt();
    /* IOポート初期化 */
    initialize_port();
	/* センサ類初期化 */
    initialize_sns();
    /* 安全機能初期化 */
    initialize_poe();
    initialize_wdt();
    /* SW,LED初期化*/
    initialize_ui();
}

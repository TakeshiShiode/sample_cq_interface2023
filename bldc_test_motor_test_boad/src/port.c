/*
 * port.c
 *
 *  Created on: 2022/03/02
 *      Author: assistmotion01
 */
#include "iodefine.h"

void initialize_port(void)
{
	/* PWM出力端子 */
    PORT7.DDR.BIT.B1 = 1;                               /* U相H出力設定 */
    PORT7.DDR.BIT.B2 = 1;                               /* V相H出力設定 */
    PORT7.DDR.BIT.B3 = 1;                               /* W相H出力設定 */
    PORT7.DDR.BIT.B4 = 1;                               /* U相L出力設定 */
    PORT7.DDR.BIT.B5 = 1;                               /* V相L出力設定 */
    PORT7.DDR.BIT.B6 = 1;   
                               							 /* W相L出力設定 */
	PORT7.DR.BIT.B1 = 0;                             /* HWマニュアル図(表15.27)P71/MTIOC3B(Up) = "L" */
	PORT7.DR.BIT.B4 = 0;                             /* HWマニュアル図(表15.27)P74/MTIOC3D(Un) = "L" */
	PORT7.DR.BIT.B2 = 0;                             /* HWマニュアル図(表15.27)P72 MTIOC4A(Vp) = "L" */
	PORT7.DR.BIT.B5 = 0;                             /* HWマニュアル図(表15.27)P75/MTIOC4C(Vn) = "L" */
	PORT7.DR.BIT.B3 = 0;                             /* HWマニュアル図(表15.27)P73/MTIOC4B(Wp) = "L" */
	PORT7.DR.BIT.B6 = 0;    
	
    /* エンコーダ端子 */
    PORT3.ICR.BIT.B3 = 1;                               /* A相の入力バッファ有効 */
    PORT3.DDR.BIT.B3 = 0;								/* データ入力方向 */
    PORT3.ICR.BIT.B2 = 1;                               /* B相の入力バッファ有効 */
    PORT3.DDR.BIT.B2 = 0;								/* データ入力方向 */

    PORT4.ICR.BIT.B0 = 1;                               /* the input buffer is enabled for ADC */
    PORT4.ICR.BIT.B1 = 1;
    PORT4.ICR.BIT.B2 = 1;
    PORT4.ICR.BIT.B6 = 1;

    /* 電流、電圧センサ */
    S12AD0.ADCSR.BIT.TRGE = 0;                          /* ADSTRGR設定でAD変換 */
    S12AD0.ADCSR.BIT.CKS = 3;							/* 周辺クロック使用(48MHz)*/
    S12AD0.ADCSR.BIT.ADIE = 0;							/* 変換後の割込みなし */
    S12AD0.ADCSR.BIT.ADCS = 1;							/* 1サイクルスキャンモード */
    S12AD0.ADANS.BIT.CH = 2;							/* AN000~AN002をスキャン */
    S12AD0.ADCER.BIT.SHBYP = 0;							/* サンプルホールド回路使用 */
    S12AD0.ADCER.BIT.ADRFMT = 0;						/* データフォーマット右詰め */
    //S12AD0.ADSTRGR.BIT.ADSTRS0 = 5;						/* MTU4.TCNTのアンダーフローで変換 */

    /* 速度制御用可変抵抗センサ */
    S12AD1.ADCSR.BIT.TRGE = 0;                          /*ADSTRGR設定でAD変換 */
    S12AD1.ADCSR.BIT.CKS = 3;							/* 周辺クロック使用(48MHz)*/
    S12AD1.ADCSR.BIT.ADIE = 0;							/* 変換後の割込みなし */
    S12AD1.ADCSR.BIT.ADCS = 0;							/* シングルモード */
    S12AD1.ADANS.BIT.CH = 2;							/* AN102チャンネル選択 */
    S12AD1.ADCER.BIT.ADRFMT = 0;						/* データフォーマット右詰め */
    //S12AD1.ADSTRGR.BIT.ADSTRS0 = 5;						/* MTU4.TCNTのアンダーフローで変換 */

}

void reset_port(void)
{
	PORT7.DR.BIT.B1 = 0;                             /* HWマニュアル図(表15.27)P71/MTIOC3B(Up) = "L" */
	PORT7.DR.BIT.B4 = 0;                             /* HWマニュアル図(表15.27)P74/MTIOC3D(Un) = "L" */
	PORT7.DR.BIT.B2 = 0;                             /* HWマニュアル図(表15.27)P72 MTIOC4A(Vp) = "L" */
	PORT7.DR.BIT.B5 = 0;                             /* HWマニュアル図(表15.27)P75/MTIOC4C(Vn) = "L" */
	PORT7.DR.BIT.B3 = 0;                             /* HWマニュアル図(表15.27)P73/MTIOC4B(Wp) = "L" */
	PORT7.DR.BIT.B6 = 0;                             /* HWマニュアル図(表15.27)P76/MTIOC4D(Wn) = "L" */
}

void initialize_ui(void)
{
    /*************** SW1,2 ***************/
    PORT9.ICR.BIT.B1 = 1;							 /* SWポート入力バッファ有効 */
    PORT9.ICR.BIT.B2 = 1;
    PORT9.DDR.BIT.B1 = 0;							 /* SWポートを入力設定 */
    PORT9.DDR.BIT.B2 = 0;
    /*************** LED1,2 ***************/
    PORTA.DR.BIT.B2 = 1;							/* LEDポート消灯 */
    PORTA.DR.BIT.B3 = 1;
    PORTA.DDR.BIT.B2 = 1;							/* LEDポート出力設定 */
    PORTA.DDR.BIT.B3 = 1;
}

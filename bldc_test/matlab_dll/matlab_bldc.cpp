
#include "pch.h"
#include "framework.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "matlab_bldc.h"
#include "matlab_bldc.h"

#include <windows.h>
#include <stdio.h>

static BYTE* pBLDCMemory = NULL;		//BLDC用共有レジスタアドレス
static double gEncCnt;
static double gBLDCPos;
static int debug_cnt;

struct debugST {
	double EncCnt;
	double pos;
}debug_ST;
static debugST debugdata[6000];

void SetBLDCMemoryAddress(BYTE* Adr)
{
	pBLDCMemory = Adr;
}

double MatlabGetBLDC_PWM_U(void)
{
	double ret;
	double duty;
	signed short* pMTU3_TGRD = (signed short*)(pBLDCMemory + MTU3_TGRD);
	unsigned char* pMTU_TOERA = (unsigned char*)(pBLDCMemory + MTU_TOERA_OFFSET);

	duty = (float)*pMTU3_TGRD;

	if (*pMTU_TOERA == 0xFF) {
		ret = (duty - PWM_PERIOD) * (-1) / PWM_PERIOD;
	}
	else {
		ret = 0;
	}
	return ret;
}

double MatlabGetBLDC_PWM_V(void)
{
	double ret;
	double duty;
	signed short* pMTU4_TGRC = (signed short*)(pBLDCMemory + MTU4_TGRC);
	unsigned char* pMTU_TOERA = (unsigned char*)(pBLDCMemory + MTU_TOERA_OFFSET);

	duty = (float)*pMTU4_TGRC;
	if (*pMTU_TOERA == 0xFF) {
		ret = (duty - PWM_PERIOD) * (-1) / PWM_PERIOD;
	}
	else {
		ret = 0;
	}
	return ret;
}

double MatlabGetBLDC_PWM_W(void)
{
	double ret;
	double duty;
	signed short* pMTU4_TGRD = (signed short*)(pBLDCMemory + MTU4_TGRD);
	unsigned char* pMTU_TOERA = (unsigned char*)(pBLDCMemory + MTU_TOERA_OFFSET);

	duty = (float)*pMTU4_TGRD;
	if (*pMTU_TOERA == 0xFF) {
		ret = (duty - PWM_PERIOD) * (-1) / PWM_PERIOD;
	}
	else {
		ret = 0;
	}
	return ret;
}

double MatlabSetBLDCData(double pos, double vdc, double iu, double iw, double omega)
{
	double ret;
	signed short* pTCNT1 = (signed short*)(pBLDCMemory + MTU1_TCNT);
	signed short* pS12AD0_ADDR0A = (signed short*)(pBLDCMemory + S12AD0_ADDR0A_OFFSET);
	signed short* pS12AD0_ADDR1 = (signed short*)(pBLDCMemory + S12AD0_ADDR1_OFFSET);
	signed short* pS12AD0_ADDR2 = (signed short*)(pBLDCMemory + S12AD0_ADDR2_OFFSET);
	signed short* pS12AD1_ADDR2 = (signed short*)(pBLDCMemory + S12AD1_ADDR2_OFFSET);
	unsigned char* pMTU1_CLEAR = (unsigned char*)(pBLDCMemory + TCNT1_CLEAR_OFFSET);

	signed short s16_i = 0;
	double enc_step = pos - gEncCnt;
	gBLDCPos = gBLDCPos - enc_step;
	*pTCNT1 = (unsigned short)(gBLDCPos * 1200.0 / (2 * M_PI));
	gEncCnt = pos;

	//if (debug_cnt < 6000) {
	//	debugdata[debug_cnt].EncCnt = gBLDCPos;
	//	debugdata[debug_cnt].pos = *pos;
	//	debug_cnt++;
	//}


	if (*pMTU1_CLEAR == 0) {
		gBLDCPos = 0;
		gEncCnt = 0;
	}

	s16_i = (signed short)((iu + 10.0) * 4095 / 20);
	if (s16_i > 4095) {
		s16_i = 4095;
	}
	if (s16_i < 0) {
		s16_i = 0;
	}
	*pS12AD0_ADDR0A = (-1) * s16_i;

	s16_i = (signed short)((iw + 10.0) * 4095 / 20);
	if (s16_i > 4095) {
		s16_i = 4095;
	}
	if (s16_i < 0) {
		s16_i = 0;
	}
	*pS12AD0_ADDR1 = (-1) * s16_i;
	*pS12AD0_ADDR2 = (signed short)((24 - vdc) * 4095 / 30);

	//if (*omega < 0.0) {
	//	*omega = *omega * (-1);
	//}
	//if (*omega < 62.832) {
	//	*omega = 0.0;
	//}
	//if (*omega > 209.44) {
	//	*omega = 209.44;
	//}
	//*pS12AD1_ADDR2 = (signed short)( (*omega * 60 / (2 * M_PI) - 600) * 4095/1400);
	//* pS12AD1_ADDR2 = (short)(1158.290588 * 4095 / 1400);

	ret = *pMTU1_CLEAR;
	return ret;
}

void MatlabInitBLDCData(void)
{
	//	int i;
	*(unsigned short*)(pBLDCMemory + MTU1_TCNT) = 0;
	*(unsigned char*)(pBLDCMemory + TCNT1_CLEAR_OFFSET) = 0;
	gBLDCPos = 0;
	//debug_cnt = 0;

	//for (i = 0; i < 6000; i++) {
	//	debugdata[debug_cnt].EncCnt = 0;
	//	debugdata[debug_cnt].pos = 0;
	//}

}



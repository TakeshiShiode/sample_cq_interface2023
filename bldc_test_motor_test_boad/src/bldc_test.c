/***********************************************************************/
/*                                                                     */
/*  FILE        : Main.c                                   */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif
#include <stdio.h>
#include <stdarg.h>

#include "bldc_test.h"
#include "safeguard.h"
#include "port.h"
#include "sns.h"
#include "cmt.h"
#include "mtu3.h"

/* BLDC全体シーケンス */
#define RESET	(0)
#define READY	(1)
#define START	(2)
#define STOP	(3)

/* 状態判定回数 */
#define LATCHCNT	(10)

/* グローバル変数 */
int gBLDCState;
unsigned char gBLDCStartReq;
int gBLDCResetCnt;

void check_bldc_reset(void);
void check_bldc_start(void);

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

void main(void)
{
	gBLDCState = RESET;
	gBLDCResetCnt = 0;
	gBLDCStartReq = BLDC_STOP_REQ;
	while(1){
		check_bldc_reset();
		check_bldc_start();
		clear_wdt();
	}
}

void bldc_start(void)
{
	/* pwmポートリセット */
	reset_port();
	/* PWM出力、割込みタイマ開始 */
	start_pwm();
	/* 速度制御開始 */
	start_cmt();
}

void bldc_stop(void)
{
	/* PWM、タイマ停止 */
	stop_pwm();
	/* 速度制御停止 */
	stop_cmt();
	/* pwmポートリセット */
	reset_port();
}

unsigned char get_bldc_start(void)
{
	return gBLDCStartReq;
}

void check_bldc_start(void)
{
	static int sBldcStartCnt = 0;
	switch(gBLDCState){
		case RESET:
			sBldcStartCnt = 0;
			gBLDCStartReq = BLDC_STOP_REQ;
			set_bldc_drive_state(STANBY);
			gBLDCState = READY;
			break;
		case READY:
			if(get_start_sw() == 1){
				sBldcStartCnt++;
			}
			else{
				sBldcStartCnt = 0;
			}
			if(sBldcStartCnt > LATCHCNT){
				sBldcStartCnt = 0;
				gBLDCStartReq = BLDC_START_REQ;
				reset_bldc_error();
				//bldc_start();
				gBLDCState = START;
			}
			break;
		case START:
			if(get_start_sw() == 0){
				sBldcStartCnt++;
			}
			else{
				sBldcStartCnt = 0;
			}
			if(sBldcStartCnt > LATCHCNT){
				sBldcStartCnt = 0;
				gBLDCStartReq = BLDC_STOP_REQ;
				gBLDCState = STOP;
			}
			break;

		case STOP:
			if(get_start_sw() == 1){
				sBldcStartCnt++;
			}
			else{
				sBldcStartCnt = 0;
			}
			if(sBldcStartCnt > LATCHCNT){
				sBldcStartCnt = 0;
				gBLDCStartReq = BLDC_START_REQ;
				gBLDCState = START;
			}
			break;
		default:
			sBldcStartCnt = 0;
			gBLDCStartReq = BLDC_STOP_REQ;
			gBLDCState = READY;
			break;
	}

}

void check_bldc_reset(void)
{
	if(get_reset_sw() == 1){
		gBLDCResetCnt++;
	}
	else{
		gBLDCResetCnt = 0;
	}
	if(gBLDCResetCnt > LATCHCNT){
		bldc_stop();
		gBLDCResetCnt = 0;
		gBLDCStartReq = BLDC_STOP_REQ;
		gBLDCState = RESET;
		/* 各種変数初期化 */
		reset_mtu3();
		reset_cmt();
		reset_bldc_error();
	}
}

void tprintf(const char *fmt, ...)
{
#if 0
	va_list va;
	char s[256] = {'\0'};
	char* psbuf;
	char* pbuf;
	unsigned long i;

	pbuf = (char *)RX62T_KEYTEST_SENTENCE_BUFSTART;

	va_start(va, fmt);
	vprintf(fmt, va);
	vsprintf(s, fmt, va);
	va_end(va);

	psbuf = s;

	for(i=0;i<256;i++){
		*pbuf = *psbuf;
		pbuf++;
		psbuf++;
		if(*psbuf == '\0'){
			*pbuf = *psbuf;
			break;
		}
	}
	*RX62T_KEYTEST_FLUSH_ON = 1;
#endif
}


#ifdef __cplusplus
void abort(void)
{

}
#endif

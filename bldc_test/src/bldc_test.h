/*
 * bldc_test.h
 *
 *  Created on: 2022/03/17
 *      Author: assistmotion01
 */

#ifndef BLDC_TEST_H_
#define BLDC_TEST_H_

/* BLDCãÏìÆóvãÅ */
#define BLDC_STOP_REQ	(0)
#define BLDC_START_REQ	(1)

#define BLDC_MGPOLE_PAIR (7)

#define BLDC_MAX_RPM      (2000)                         /* ã@äBäpÇ≈ÇÃç≈çÇâÒì]êîrpm */
#define BLDC_MIN_RPM      (600)                          /* ã@äBäpÇ≈ÇÃç≈í·âÒì]êîrpm */
#define MARGIN_RPM        (50)                           /* É}Å[ÉWÉìó  */
#define BLDC_MARGIN_MAX_RPM    (BLDC_MAX_RPM + MARGIN_RPM)
#define BLDC_MARGIN_MIN_RPM    (BLDC_MIN_RPM - MARGIN_RPM)
#define	BLDC_MAX_RAD		(209.43951f)				/* ã@äBäpÇ≈ÇÃç≈çÇâÒì]êîrad */
#define BLDC_MIN_RAD		(62.831853f)				/* ã@äBäpÇ≈ÇÃç≈í·âÒì]êîrad */

unsigned char get_bldc_start(void);
void bldc_stop(void);


/* ÉfÉoÉbÉOèoóÕ */
#define RX62T_KEYTEST_TOP        			0x000FFD00
#define RX62T_KEYTEST_KEYSTATUS             ((volatile unsigned char *)(RX62T_KEYTEST_TOP + 0x0000))
#define RX62T_KEYTEST_KEYTEST               ((volatile unsigned char *)(RX62T_KEYTEST_TOP + 0x0001))
#define RX62T_KEYTEST_FLUSH_ON              ((volatile unsigned char *)(RX62T_KEYTEST_TOP + 0x0002))
#define RX62T_KEYTEST_SENTENCE_BUFSTART     ((volatile unsigned char *)(RX62T_KEYTEST_TOP + 0x0004))
#define RX62T_KEYTEST_SENTENCE_BUFEND       ((volatile unsigned char *)(RX62T_KEYTEST_TOP + 0x0104))
void tprintf(const char *fmt, ...);

#endif /* BLDC_TEST_H_ */

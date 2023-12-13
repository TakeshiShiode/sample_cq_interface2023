/*
 * Renesas system timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_SYSTEM_H
#define HW_RENESAS_SYSTEM_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_SYSTEM "renesas-system"
//#define RSYSTEM(obj) OBJECT_CHECK(RSYSTEMState, (obj), TYPE_RENESAS_SYSTEM)
typedef struct RSYSTEMState RSYSTEMState;
DECLARE_INSTANCE_CHECKER(RSYSTEMState, RSYSTEM,
                         TYPE_RENESAS_SYSTEM)

/* register offset address */
/* SYSTEM*/
#define R_MDMONR	(0x0000)
#define R_SYSCR1	(0x0008)
#define R_SBYCR		(0x000C)
#define R_MSTPCRA	(0x0010)
#define R_MSTPCRB	(0x0014)
#define R_MSTPCRC	(0x0018)
#define R_SYSKCR	(0x0020)
#define R_SYSKCR3	(0x0026)
#define R_PLLCR		(0x0028)
#define R_PLLCR2	(0x002A)
#define R_MEMWAIT	(0x0031)
#define R_MOSCCR	(0x0032)
#define R_LOCOCR	(0x0034)
#define R_ILOCOCR	(0x0035)
#define R_HOCOCR	(0x0036)
#define R_HOCOCR2	(0x0037)
#define R_OSCOVFSR	(0x003C)
#define R_OSTDCR	(0x0040)
#define R_OSTDSR	(0x0041)
#define R_OPCCR		(0x00A0)
#define R_MOSCWTCR	(0x00A2)
#define R_HOCOWTCR	(0x00A5)
#define R_RSTSR2	(0x00C0)
#define R_SWRR		(0x00C2)
#define R_LVD1CR1	(0x00E0)
#define R_LVD1SR	(0x00E1)
#define R_LVD2CR1	(0x00E2)
#define R_LVD2SR	(0x00E3)
#define R_PRCR		(0x03FE)


typedef struct RSYSTEMState {
    SysBusDevice parent_obj;

    uint64_t input_freq;
    MemoryRegion memory;

    uint16_t	mdmonr;
    uint16_t 	syscr1;
    uint16_t 	sbycr;
    uint32_t 	mstpcra;
    uint32_t 	mstpcrb;
    uint32_t 	mstpcrc;
    uint32_t 	syskcr;
    uint16_t	syskcr3;
    uint16_t 	pllcr;
    uint8_t 	pllcr2;
    uint8_t		memwait;
    uint8_t		mosccr;
    uint8_t		lococr;
    uint8_t		ilococr;
    uint8_t		hococr;
    uint8_t		hococr2;
    uint8_t		oscovfsr;
    uint8_t		ostdcr;
    uint8_t		ostdsr;
    uint8_t		opccr;
    uint8_t		moscwtc;
    uint8_t		hocowtc;
    uint8_t		rstsr2;
    uint16_t	swrr;
    uint8_t		lvd1cr1;
    uint8_t		lvd1sr;
    uint8_t		lvd2cr1;
    uint8_t		lvd2sr;
    uint16_t	prcr;
} RSYSTEMState;


RSYSTEMState * Get_RSYSTEMState(void);
#endif

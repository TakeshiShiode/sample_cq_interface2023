/*
 * Renesas s12ad timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_S12AD_H
#define HW_RENESAS_S12AD_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_S12AD "renesas-s12ad"
//#define RS12AD(obj) OBJECT_CHECK(RS12ADState, (obj), TYPE_RENESAS_S12AD)
typedef struct RS12ADState RS12ADState;
DECLARE_INSTANCE_CHECKER(RS12ADState, RS12AD,
                         TYPE_RENESAS_S12AD)

/* register offset address */
/* S12AD0*/
#define R_ADCSR_0	(0x00)
#define R_ADANS_0	(0x04)
#define R_ADPG_0	(0x0A)
#define R_ADCER_0	(0x0E)
#define R_ADSTRGR_0	(0x10)
#define R_ADRD_0	(0x1E)
#define R_ADDR0A_0  (0x20)
#define R_ADDR1_0	(0x22)
#define R_ADDR2_0	(0x24)
#define	R_ADDR3_0	(0x26)
#define	R_ADDR0B_0	(0x30)
#define	R_ADSSTR_0	(0x60)
/* S12AD1*/
#define R_ADCSR_1	(0x80)
#define R_ADANS_1	(0x84)
#define R_ADPG_1	(0x8A)
#define R_ADCER_1	(0x8E)
#define R_ADSTRGR_1	(0x90)
#define R_ADRD_1	(0x9E)
#define R_ADDR0A_1  (0xA0)
#define R_ADDR1_1	(0xA2)
#define R_ADDR2_1	(0xA4)
#define	R_ADDR3_1	(0xA6)
#define	R_ADDR0B_1	(0xB0)
#define	R_ADSSTR_1	(0xE0)
/* S12AD */
#define R_ADCMPMD0	(0x12)
#define R_ADCMPMD1	(0x14)
#define R_ADCMPNR0	(0x16)
#define R_ADCMPNR1	(0x18)
#define R_ADCMPFR	(0x1A)
#define R_ADCMPSEL	(0x1C)


enum {
    S12AD_CH = 2,
    S12AD_NR_IRQ = 1 * S12AD_CH,
};

typedef struct RS12ADState {
    SysBusDevice parent_obj;

    uint64_t input_freq;
    MemoryRegion memory;

    uint8_t  	adscr_0;
    uint16_t  	adans_0;
    uint16_t 	adpg_0;
    uint16_t 	adcer_0;
    uint16_t 	adstrgr_0;
    uint16_t  	adrd_0;
    uint16_t 	addr0a_0;
    uint16_t	addr1_0;
    uint16_t 	addr2_0;
    uint16_t 	addr3_0;
    uint16_t 	addr0b_0;
    uint8_t		adsstr_0;

    uint8_t  	adscr_1;
    uint16_t  	adans_1;
    uint16_t 	adpg_1;
    uint16_t 	adcer_1;
    uint16_t 	adstrgr_1;
    uint16_t  	adrd_1;
    uint16_t 	addr0a_1;
    uint16_t	addr1_1;
    uint16_t 	addr2_1;
    uint16_t 	addr3_1;
    uint16_t 	addr0b_1;
    uint8_t		adsstr_1;
    
    uint16_t	adcmpmd0;
    uint16_t	adcmpmd1;
    uint16_t	adcmpnr0;
    uint16_t	adcmpnr1;
    uint16_t	adcmpfr;
    uint16_t	adcmpsel;
 
    qemu_irq 	s12adi0;
    qemu_irq 	s12adi1;
} RS12ADState;

#define RX62T_S12AD_IRQBASE (102)

#define S12AD0_ADDR0A_OFFSET (0x0C)
#define S12AD0_ADDR1_OFFSET (0x0E)
#define S12AD0_ADDR2_OFFSET (0x10)
#define S12AD0_ADDR3_OFFSET (0x12)

#define S12AD1_ADDR0A_OFFSET (0x14)
#define S12AD1_ADDR1_OFFSET (0x16)
#define S12AD1_ADDR2_OFFSET (0x18)
#define S12AD1_ADDR3_OFFSET (0x1A)

RS12ADState * Get_RS12ADState(void);
#endif

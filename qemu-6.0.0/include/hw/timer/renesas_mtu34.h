/*
 * Renesas mtu34 timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_MTU34_H
#define HW_RENESAS_MTU34_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_MTU34 "renesas-mtu34"
//#define RMTU34(obj) OBJECT_CHECK(RMTU34State, (obj), TYPE_RENESAS_MTU34)
typedef struct RMTU34State RMTU34State;
DECLARE_INSTANCE_CHECKER(RMTU34State, RMTU34,
                         TYPE_RENESAS_MTU34)
/* register offset address */
/*MTU3*/
#define R_TCR_3   (0x00)
#define R_TMDR1_3 (0x02)
#define R_TIORH_3 (0x04)
#define R_TIORL_3 (0x05)
#define R_TIER_3  (0x08)
#define R_TCNT_3  (0x10)
#define R_TGRA_3  (0x18)
#define R_TGRB_3  (0x1A)
#define R_TGRC_3  (0x24)
#define R_TGRD_3  (0x26)
#define R_TGRE_3  (0x72)
#define R_TSR_3   (0x2C)
#define R_TBTM_3  (0x38)
/*MTU4*/
#define R_TCR_4       (0x01)
#define R_TMDR1_4     (0x03)
#define R_TIORH_4     (0x06)
#define R_TIORL_4     (0x07)
#define R_TIER_4      (0x09)
#define R_TCNT_4      (0x12)
#define R_TGRA_4      (0x1C)
#define R_TGRB_4      (0x1E)
#define R_TGRC_4      (0x28)
#define R_TGRD_4      (0x2A)
#define R_TGRE_4      (0x74)
#define R_TGRF_4      (0x76)
#define R_TSR_4       (0x2D)
#define R_TBTM_4      (0x39)
#define R_TADCR_4     (0x40)
#define R_TADCORA_4   (0x44)
#define R_TADCORB_4   (0x46)
#define R_TADCOBRA_4  (0x48)
#define R_TADCOBRB_4  (0x4A)

/*MTU_COMMON*/
#define R_TOERA     (0x0A)
#define R_TGCRA     (0x0D)
#define R_TOCR1A    (0x0E)
#define R_TOCR2A    (0x0F)
#define R_TCDRA     (0x14)
#define R_TDDRA     (0x16)
#define R_TDERA     (0x34)
#define R_TCNTSA    (0x20)
#define R_TCBRA     (0x22)
#define R_TITCR1A   (0x30)
#define R_TITCR2A   (0x3B)
#define R_TITCNT1A  (0x31)
#define R_TITCNT2A  (0x3C)
#define R_TBTERA    (0x32)
#define R_TOLBRA    (0x36)
#define R_TMDR2A    (0x70)
#define R_TITMRA    (0x3A)
#define R_TWCRA     (0x60)
#define R_TSTRA     (0x80)
#define R_TSYRA     (0x81)
#define R_TCSYSTR   (0x82)
#define R_TRWERA    (0x84)

#define RX62T_MTU34_IRQBASE 129

enum {
    MTU34_CH = 10,
    MTU34_NR_IRQ = 1 * MTU34_CH,
};

typedef struct RMTU34State {
    SysBusDevice parent_obj;

    uint64_t input_freq;
    int64_t div_round_3;
    int64_t div_round_4;
    int64_t tick_3;
    int64_t tick_4;
    MemoryRegion memory;
    /*MTU3*/
    uint8_t  tcr_3;
    uint8_t  tmdr1_3;
    uint8_t  tiorh_3;
    uint8_t  tiorl_3;
    uint8_t  tier_3;
    uint16_t tcnt_3;
    uint16_t tgra_3;
    uint16_t tgrb_3;
    uint16_t tgrc_3;
    uint16_t tgrd_3;
    uint16_t tgre_3;
    uint8_t  tsr_3;
    uint8_t  tbtm_3;
    /*MTU4*/
    uint8_t  tcr_4;
    uint8_t  tmdr1_4;
    uint8_t  tiorh_4;
    uint8_t  tiorl_4;
    uint8_t  tier_4;
    uint16_t tcnt_4;
    uint16_t tgra_4;
    uint16_t tgrb_4;
    uint16_t tgrc_4;
    uint16_t tgrd_4;
    uint16_t tgre_4;
    uint16_t tgrf_4;
    uint8_t  tsr_4;
    uint8_t  tbtm_4;
    uint16_t tadcr_4;
    uint16_t tadcora_4;
    uint16_t tadcorb_4;
    uint16_t tadcobra_4;
    uint16_t tadcobrb_4;
    /*/*MTU_COMMON*/
    uint8_t  toera;
    uint8_t  tgcra;
    uint8_t  tocr1a;
    uint8_t  tocr2a;
    uint16_t tcdra;
    uint16_t tddra;
    uint8_t  tdera;
    uint16_t tcntsa;
    uint16_t tcbra;
    uint8_t  titcr1a;
    uint8_t  titcr2a;
    uint8_t  titcnt1a;
    uint8_t  titcnt2a;
    uint8_t  tbtera;
    uint8_t  tolbra;
    uint8_t  tmdr2a;
    uint8_t  titmra;
    uint8_t  twcra;
    uint8_t  tstra;
    uint8_t  tsyra;
    uint8_t  tcsystr;
    uint8_t  trwera;
    qemu_irq tgia_3;
    qemu_irq tgib_3;
    qemu_irq tgic_3;
    qemu_irq tgid_3;
    qemu_irq tciv_3;
    qemu_irq tgia_4;
    qemu_irq tgib_4;
    qemu_irq tgic_4;
    qemu_irq tgid_4;
    qemu_irq tciv_4;
    QEMUTimer timer3;
    QEMUTimer timer4;
    QemuThread thread;
} RMTU34State;

#define TGRD3_OFFSET (0x06)
#define TGRC4_OFFSET (0x08)
#define TGRD4_OFFSET (0x0A)
#define TOERA_OFFSET (0x01C)
#define Mat_COM_OFFSET (0x1D)

uint8_t *GetMatBLDCComMemory(void);
void ReleaseMatBLDCCommemory(void);
#endif

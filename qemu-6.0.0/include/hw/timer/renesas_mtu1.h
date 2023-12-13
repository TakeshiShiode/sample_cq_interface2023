/*
 * Renesas mtu1 timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_MTU1_H
#define HW_RENESAS_MTU1_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_MTU1 "renesas-mtu1"
//#define RMTU1(obj) OBJECT_CHECK(RMTU1State, (obj), TYPE_RENESAS_MTU1)
typedef struct RMTU1State RMTU1State;
DECLARE_INSTANCE_CHECKER(RMTU1State, RMTU1,
                         TYPE_RENESAS_MTU1)

/* register offset address */
#define R_TCR_1  (0x00)
#define R_TMDR1_1 (0x01)
#define R_TIOR_1 (0x02)
#define R_TIER_1  (0x04)
#define R_TSR_1   (0x05)
#define R_TCNT_1  (0x06)
#define R_TGRA_1  (0x08)
#define R_TGRB_1  (0x0A)
#define R_TICCR_1  (0x10)

enum {
    MTU1_CH = 2,
    MTU1_NR_IRQ = 1 * MTU1_CH,
};

typedef struct RMTU1State {
    SysBusDevice parent_obj;

    uint64_t input_freq;
    int64_t div_round;
    int64_t tick;
    MemoryRegion memory;

    uint8_t  tcr;
    uint8_t  tmdr1;
    uint8_t  tiorh;
    uint8_t  tiorl;
    uint8_t  tier;
    uint8_t  tsr;
    uint32_t tcnt;
    uint16_t tgra;
    uint16_t tgrb;
    uint16_t ticcr;
    qemu_irq tgia;
    qemu_irq tgib;
    QEMUTimer timer;
} RMTU1State;

#define RX62T_MTU1_IRQBASE (121)
#define TCNT1_OFFSET (0x02)
#define TCNT1_CLEAR_OFFSET (0x1E)
RMTU1State * Get_RMTU1State(void);
#endif

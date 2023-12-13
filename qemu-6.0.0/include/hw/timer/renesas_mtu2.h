/*
 * Renesas mtu2 timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_MTU2_H
#define HW_RENESAS_MTU2_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_MTU2 "renesas-mtu2"
//#define RMTU2(obj) OBJECT_CHECK(RMTU2State, (obj), TYPE_RENESAS_MTU2)
typedef struct RMTU2State RMTU2State;
DECLARE_INSTANCE_CHECKER(RMTU2State, RMTU2,
                         TYPE_RENESAS_MTU2)

/* register offset address */
#define R_TCR_2  (0x00)
#define R_TMDR1_2 (0x01)
#define R_TIOR_2 (0x02)
#define R_TIER_2  (0x04)
#define R_TSR_2   (0x05)
#define R_TCNT_2  (0x06)
#define R_TGRA_2  (0x08)
#define R_TGRB_2  (0x0A)
#define R_TICCR_2  (0x10)

enum {
    MTU2_CH = 2,
    MTU2_NR_IRQ = 1 * MTU2_CH,
};

typedef struct RMTU2State {
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
} RMTU2State;

#define RX62T_MTU2_IRQBASE 125
#define TCNT2_OFFSET (0x04)
RMTU2State * Get_RMTU2State(void);
#endif

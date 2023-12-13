/*
 * Renesas mtu0 timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_MTU0_H
#define HW_RENESAS_MTU0_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_MTU0 "renesas-mtu0"
//#define RMTU0(obj) OBJECT_CHECK(RMTU0State, (obj), TYPE_RENESAS_MTU0)
typedef struct RMTU0State RMTU0State;
DECLARE_INSTANCE_CHECKER(RMTU0State, RMTU0,
                         TYPE_RENESAS_MTU0)

/* register offset address */
#define R_TCR   (0x00)
#define R_TMDR1 (0x01)
#define R_TIORH (0x02)
#define R_TIORL (0x03)
#define R_TIER  (0x04)
#define R_TSR   (0x05)
#define R_TCNT  (0x06)
#define R_TGRA  (0x08)
#define R_TGRB  (0x0A)
#define R_TGRC  (0x0C)
#define R_TGRD  (0x0E)
#define R_TGRE  (0x20)
#define R_TGRF  (0x22)
#define R_TIER2 (0x24)
#define R_TSR2  (0x25)
#define R_TBTM  (0x26)

enum {
    MTU0_CH = 7,
    MTU0_NR_IRQ = 1 * MTU0_CH,
};

typedef struct RMTU0State {
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
    uint16_t tgrc;
    uint16_t tgrd;
    uint16_t tgre;
    uint16_t tgrf;
    uint8_t  tier2;
    uint8_t  tsr2;
    uint8_t  tbtm;
    qemu_irq tgia;
    qemu_irq tgib;
    qemu_irq tgic;
    qemu_irq tgid;
    qemu_irq tgie;
    qemu_irq tgif;
    qemu_irq tciv;
    QEMUTimer timer;
} RMTU0State;

#define RX62T_MTU0_IRQBASE 114

RMTU0State * Get_RMTU0State(void);
#endif

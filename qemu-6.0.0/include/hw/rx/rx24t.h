/*
 * RX24T MCU Object
 *
 * Datasheet: RX24T Group, RX241 Group User's Manual: Hardware
 *            (Rev.1.40 R01UH0033EJ0140)
 *
 * Copyright (c) 2019 Yoshinori Sato
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HW_RX_RX24T_MCU_H
#define HW_RX_RX24T_MCU_H

#include "target/rx/cpu.h"
#include "hw/misc/rx_system.h"
#include "hw/intc/rx_icu.h"
#include "hw/timer/renesas_tmr.h"
#include "hw/timer/renesas_cmt.h"
#include "hw/char/renesas_sci.h"
#include "qemu/units.h"
#include "qom/object.h"

#include "hw/timer/renesas_mtu0.h"
#include "hw/timer/renesas_mtu1.h"
#include "hw/timer/renesas_mtu2.h"
#include "hw/timer/renesas_mtu34.h"
#include "hw/adc/renesas_s12ad.h"
#include "hw/gpio/rx_ioport.h"
#include "hw/timer/rx62t_keytest.h"

#define TYPE_RX24T_MCU "rx24t-mcu"
typedef struct RX24TState RX24TState;
DECLARE_INSTANCE_CHECKER(RX24TState, RX24T_MCU,
                         TYPE_RX24T_MCU)

#define TYPE_R5F524TAADFP_MCU "r5f524taadfp-mcu"

#define EXT_CS_BASE         0x01000000
#define VECTOR_TABLE_BASE   0xffffff80
#define RX24T_CFLASH_BASE   0xfff80000

#define RX24T_NR_TMR    2
#define RX24T_NR_CMT    2
#define RX24T_NR_SCI    6

struct RX24TState {
    /*< private >*/
    DeviceState parent_obj;
    /*< public >*/

    RXCPU cpu;
    RSYSTEMState system;
    RXICUState icu;
    RTMRState tmr[RX24T_NR_TMR];
    RCMTState cmt[RX24T_NR_CMT];
    RSCIState sci[RX24T_NR_SCI];

    RMTU0State mtu0;
    RMTU1State mtu1;
    RMTU2State mtu2;
	RMTU34State mtu34;
	RS12ADState s12ad;
	RIOPORTState ioport;
	RKEYTESTState keytest;
	
    MemoryRegion *sysmem;
    bool kernel;

    MemoryRegion iram;
    MemoryRegion iomem1;
    MemoryRegion d_flash;
    MemoryRegion iomem2;
    MemoryRegion iomem3;
    MemoryRegion c_flash;
    qemu_irq irq[NR_IRQS];

    /* Input Clock (XTAL) frequency */
    uint32_t xtal_freq_hz;
    /* Peripheral Module Clock frequency */
    uint32_t pclk_freq_hz;
};


#define RX24T_ICLK (96 * 1000 * 1000)
#define RX24T_PCLK (12 * 1000 * 1000)

#endif

/*
 * Renesas mtu_0 timer
 *
 * Datasheet: RX62N Group, RX621 Group User's Manual: Hardware
 * (Rev.1.40 R01UH0033EJ0140)
 *
 * Copyright (c) 2022 Takeshi Shiode
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

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "qemu/log.h"
#include "qapi/error.h"
#include "qemu/timer.h"
#include "qemu/bitops.h"
#include "cpu.h"
#include "hw/hw.h"
#include "hw/irq.h"
#include "hw/sysbus.h"
#include "hw/registerfields.h"
#include "hw/qdev-properties.h"
#include "hw/timer/renesas_mtu.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static void timer_events(RMTU0State *mtu0);

static uint64_t mtu_read(void *opaque, hwaddr addr, unsigned size)
{
    RmtuState *mtu = opaque;
    uint64_t ret;
    switch(addr){
        case R_TOERA    ret = mtu->toera;      break;
        case R_TOERB    ret = mtu->toerb;      break;
        case R_TGCRA    ret = mtu->tgcra;      break;
        case R_TOCR1A   ret = mtu->tocr1a;     break;
        case R_TOCR1B   ret = mtu->tocr1b;     break;
        case R_TOCR2A   ret = mtu->tocr2a;     break;
        case R_TOCR2B   ret = mtu->tocr2b;     break;
        case R_TCDRA    ret = mtu->tcdra;      break;
        case R_TCDRB    ret = mtu->tcdrb;      break;
        case R_TDDRA    ret = mtu->tddra;      break;
        case R_TDERA    ret = mtu->tdera;      break;
        case R_TDDRB    ret = mtu->tddrb;      break;
        case R_TDERB    ret = mtu->tderb;      break;
        case R_TCNTSA   ret = mtu->tcntsa;     break;
        case R_TCNTSB   ret = mtu->tcntsb;     break;
        case R_TCBRA    ret = mtu->tcbra;      break;
        case R_TCBRB    ret = mtu->tcbrb;      break;
        case R_TITCR1A  ret = mtu->titcr1a;    break;
        case R_TITCR1B  ret = mtu->titcr1b;    break;
        case R_TITCR2A  ret = mtu->titcr2a;    break;
        case R_TITCR2B  ret = mtu->titcr2b;    break;
        case R_TITCNT1A ret = mtu->titcnt1a;   break;
        case R_TITCNT1B ret = mtu->titcnt1b;   break;
        case R_TITCNT2A ret = mtu->titcnt2a;   break;
        case R_TITCNT2B ret = mtu->titcnt2b;   break;
        case R_TBTERA   ret = mtu->tbtera;     break;
        case R_TBTERB   ret = mtu->tbterb;     break;
        case R_TOLBRA   ret = mtu->tolbra;     break;
        case R_TOLBRB   ret = mtu->tolbrb;     break;
        case R_TMDR2A   ret = mtu->tmdr2a;     break;
        case R_TMDR2B   ret = mtu->tmdr2b;     break;
        case R_TITMRA   ret = mtu->titmra;     break;        
        case R_TITMRB   ret = mtu->titmrb;     break;
        case R_TWCRA    ret = mtu->twcra;      break;   
        case R_TWCRB    ret = mtu->twcrb;      break;
        case R_TSTRA    ret = mtu->tstra;      break;
        case R_TSTRB    ret = mtu->tstrb;      break;
        case R_TSYRA    ret = mtu->tsyra;      break;
        case R_TSYRB    ret = mtu->tsyrb;      break;
        case R_TCSYSTR  ret = mtu->tcsystr;    break;
        case R_TRWERA   ret = mtu->trwera;     break;     
        case R_TRWERB   ret = mtu->trwerb;     break;
        default: 
                ret= 0;
            break;
    }
    
    switch(size){
        case 1: ret = ret; break;
        case 2: ret = (uint16_t)ret; break;
        case 4: ret = (uint32_t)ret; break;
        case 8: ret = (uint64_t)ret; break;
        default: 
                ret= 0;
            break;
    }
    
    return ret;
}

static void mtu_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RMTU0State *mtu0 = opaque;
    uint64_t latval;
    
    switch(size){
        case 1: latval = val; break;
        case 2: latval = (uint16_t)val; break;
        case 4: latval = (uint32_t)val; break;
        case 8: latval = (uint64_t)val; break;
        default: 
                latval = 0;
            break;
    }
    
    switch(addr){
        case R_TOERA    mtu->toera     = latval; break;
        case R_TOERB    mtu->toerb     = latval; break;
        case R_TGCRA    mtu->tgcra     = latval; break;
        case R_TOCR1A   mtu->tocr1a    = latval; break;
        case R_TOCR1B   mtu->tocr1b    = latval; break;
        case R_TOCR2A   mtu->tocr2a    = latval; break;
        case R_TOCR2B   mtu->tocr2b    = latval; break;
        case R_TCDRA    mtu->tcdra     = latval; break;
        case R_TCDRB    mtu->tcdrb     = latval; break;
        case R_TDDRA    mtu->tddra     = latval; break;
        case R_TDERA    mtu->tdera     = latval; break;
        case R_TDDRB    mtu->tddrb     = latval; break;
        case R_TDERB    mtu->tderb     = latval; break;
        case R_TCNTSA   mtu->tcntsa    = latval; break;
        case R_TCNTSB   mtu->tcntsb    = latval; break;
        case R_TCBRA    mtu->tcbra     = latval; break;
        case R_TCBRB    mtu->tcbrb     = latval; break;
        case R_TITCR1A  mtu->titcr1a   = latval; break;
        case R_TITCR1B  mtu->titcr1b   = latval; break;
        case R_TITCR2A  mtu->titcr2a   = latval; break;
        case R_TITCR2B  mtu->titcr2b   = latval; break;
        case R_TITCNT1A mtu->titcnt1a  = latval; break;
        case R_TITCNT1B mtu->titcnt1b  = latval; break;
        case R_TITCNT2A mtu->titcnt2a  = latval; break;
        case R_TITCNT2B mtu->titcnt2b  = latval; break;
        case R_TBTERA   mtu->tbtera    = latval; break;
        case R_TBTERB   mtu->tbterb    = latval; break;
        case R_TOLBRA   mtu->tolbra    = latval; break;
        case R_TOLBRB   mtu->tolbrb    = latval; break;
        case R_TMDR2A   mtu->tmdr2a    = latval; break;
        case R_TMDR2B   mtu->tmdr2b    = latval; break;
        case R_TITMRA   mtu->titmra    = latval; break;        
        case R_TITMRB   mtu->titmrb    = latval; break;
        case R_TWCRA    mtu->twcra     = latval; break;   
        case R_TWCRB    mtu->twcrb     = latval; break;
        case R_TSTRA    mtu->tstra     = latval; break;
        case R_TSTRB    mtu->tstrb     = latval; break;
        case R_TSYRA    mtu->tsyra     = latval; break;
        case R_TSYRB    mtu->tsyrb     = latval; break;
        case R_TCSYSTR  mtu->tcsystr   = latval; break;
        case R_TRWERA   mtu->trwera    = latval; break;     
        case R_TRWERB   mtu->trwerb    = latval; break;
        default: 
            break;
    }
}

static const MemoryRegionOps mtu_ops = {
    .write = mtu_write,
    .read  = mtu_read,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 2,
    },
    .valid = {
        .min_access_size = 1,
        .max_access_size = 2,
    },
};


static void rmtu_reset(DeviceState *dev)
{
   
}

static void rmtu_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RMTUState *mtu = RMTU(obj);
    int i;

    memory_region_init_io(&mtu->memory, OBJECT(mtu), &mtu_ops,
                          mtu, "renesas-mtur", 0x100);
    sysbus_init_mmio(d, &mtu->memory);

    //for (i = 0; i < ARRAY_SIZE(mtu->ovi); i++) {
    //    sysbus_init_irq(d, &mtu->cmia[i]);
    //    sysbus_init_irq(d, &mtu->cmib[i]);
    //    sysbus_init_irq(d, &mtu->ovi[i]);
    //}
    //mtu->timer[0] = timer_new_ns(QEMU_CLOCK_VIRTUAL, timer_event0, mtu);
    //mtu->timer[1] = timer_new_ns(QEMU_CLOCK_VIRTUAL, timer_event1, mtu);
}

static const VMStateDescription vmstate_rmtu = {
    .name = "rx-mtu",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rmtu_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RTMRState, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rmtu_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->props = rmtu_properties;
    dc->vmsd = &vmstate_rmtu;
    dc->reset = rmtu_reset;
}

static const TypeInfo rmtu_info = {
    .name       = TYPE_RENESAS_MTU,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RMTUState),
    .instance_init = rmtu_init,
    .class_init = rmtu_class_init,
};

static void rmtu_register_types(void)
{
    type_register_static(&rmtu_info);
}

type_init(rmtu_register_types)

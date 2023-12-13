/*
 * Renesas mtu3_0 timer
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
#include "hw/hw.h"
#include "hw/irq.h"
#include "hw/sysbus.h"
#include "hw/registerfields.h"
#include "hw/qdev-properties.h"
#include "hw/timer/renesas_mtu0.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static RMTU0State *gpRMTU0State = NULL;
static void timer_events(RMTU0State *mtu0);

static uint64_t mtu0_read(void *opaque, hwaddr addr, unsigned size)
{
    RMTU0State *mtu0 = opaque;
    uint64_t ret;
    switch(addr){
        case R_TCR:     ret = mtu0->tcr;    break;
        case R_TMDR1:   ret = mtu0->tmdr1;  break;
        case R_TIORH:   ret = mtu0->tiorh;  break;
        case R_TIORL:   ret = mtu0->tiorl;  break;
        case R_TIER:    ret = mtu0->tier;   break;
        case R_TSR:     ret = mtu0->tsr;    break;
        case R_TCNT:    ret = mtu0->tcnt;   break;
        case R_TGRA:    ret = mtu0->tgra;   break;
        case R_TGRB:    ret = mtu0->tgrb;   break;
        case R_TGRC:    ret = mtu0->tgrc;   break;
        case R_TGRD:    ret = mtu0->tgrd;   break;
        case R_TGRE:    ret = mtu0->tgre;   break;
        case R_TGRF:    ret = mtu0->tgrf;   break;
        case R_TIER2:   ret = mtu0->tier2;  break;
        case R_TSR2:    ret = mtu0->tsr2;   break;
        case R_TBTM:    ret = mtu0->tbtm;   break;
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


static void mtu0_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
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
    
    switch (addr) {
        case R_TCR:      mtu0->tcr   = latval;   break;
        case R_TMDR1:    mtu0->tmdr1 = latval;   break;
        case R_TIORH:    mtu0->tiorh = latval;   break;
        case R_TIORL:    mtu0->tiorl = latval;   break;
        case R_TIER:     mtu0->tier  = latval;   break;
        case R_TSR:      mtu0->tsr   = latval;   break;
        case R_TCNT:
		{     
			mtu0->tcnt  = latval;
			if(latval == 0){
				mtu0->tick = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    			timer_mod(&mtu0->timer, mtu0->tick + mtu0->tgra);
			}
	 	 	break;

		}
        case R_TGRA:     mtu0->tgra  = latval;   break;
        case R_TGRB:     mtu0->tgrb  = latval;   break;
        case R_TGRC:     mtu0->tgrc  = latval;   break;
        case R_TGRD:     mtu0->tgrd  = latval;   break;
        case R_TGRE:     mtu0->tgre  = latval;   break;
        case R_TGRF:     mtu0->tgrf  = latval;   break;
        case R_TIER2:    mtu0->tier2 = latval;   break;
        case R_TSR2:     mtu0->tsr2  = latval;   break;
       // case R_TBTM:     mtu0->tbtm  = latval;   break;
       case R_TBTM:
            mtu0->tbtm  = latval;
            qemu_irq_pulse(mtu0->tgia);
        break;
        
        default:
            break;
        }
}

static const MemoryRegionOps mtu0_ops = {
    .write = mtu0_write,
    .read  = mtu0_read,
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


static void tgra_event(RMTU0State *mtu0)
{
    /* TGRA */
    if(mtu0->tcnt >= mtu0->tgra){
        /* interrupt */
        if(mtu0->tier & 0x01){
            qemu_irq_pulse(mtu0->tgia);
            mtu0->tsr |= 0x01;
        }
        /* tcnt clear */
        if( (mtu0->tcr & 0xE0) == 0x20 ){
            mtu0->tcnt = mtu0->tcnt - mtu0->tgra;
        }
    }

    /* TGRB */
    if(mtu0->tcnt >= mtu0->tgrb){
        /* interrupt */
        if(mtu0->tier & 0x02){
            qemu_irq_pulse(mtu0->tgib);
            mtu0->tsr |= 0x02;
        }
        /* tcnt clear */
        if( (mtu0->tcr & 0xE0) == 0x40 ){
            mtu0->tcnt = mtu0->tcnt - mtu0->tgrb;
        }
    }

    /* TGRC */
    if(mtu0->tcnt >= mtu0->tgrc){
        /* interrupt */
        if(mtu0->tier & 0x04){
            qemu_irq_pulse(mtu0->tgic);
            mtu0->tsr |= 0x04;
        }
        /* tcnt clear */
        if( (mtu0->tcr & 0xE0) == 0xA0 ){
            mtu0->tcnt = mtu0->tcnt - mtu0->tgrc;
        }
    }

    /* TGRD */
    if(mtu0->tcnt >= mtu0->tgrd){
        /* interrupt */
        if(mtu0->tier & 0x08){
            qemu_irq_pulse(mtu0->tgid);
            mtu0->tsr |= 0x08;
        }
        /* tcnt clear */
        if( (mtu0->tcr & 0xE0) == 0xC0 ){
            mtu0->tcnt = mtu0->tcnt - mtu0->tgrd;
        }
    }

   // /* TGRE */
   // if(mtu0->tcnt >= mtu0->tgre){
   //     /* interrupt */
   //     if(mtu0->tier2 & 0x01){
   //         qemu_irq_pulse(mtu0->tgie);
   //         mtu0->tsr2 |= 0x01;
   //     }
   // }

   // /* TGRF */
   // if(mtu0->tcnt >= mtu0->tgrf){
   //     /* interrupt */
   //     if(mtu0->tier2 & 0x02){
   //         qemu_irq_pulse(mtu0->tgif);
   //         mtu0->tcnt = mtu0->tcnt - mtu0->tgre;
   //     }
   // }
}

static void timer_events(RMTU0State *mtu0)
{
    int64_t delta, now = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    int64_t tpsc = (mtu0->tcr & 0x07);
    int64_t next_time = 0;
    int64_t min_time = 0;
    int64_t i;
	uint32_t diff[4] = {0};
    int divrate = 1;
    const int iclk = 96;
    
	delta = (now - mtu0->tick) * iclk;
//  タイマスタートレジスタチェック
    switch(tpsc){
        case 0: divrate = 1;	break;    /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 1)*/
        case 1: divrate = 4;	break;    /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 4)*/
        case 2: divrate = 16;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 16)*/
        case 3: divrate = 64;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
//      case 4: divrate = 256;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
//      case 5: divrate = 1024;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        default:				break;
    }
    
    mtu0->div_round += delta; 
	mtu0->tick = now;
	 //経過時間計算
    next_time = mtu0->div_round / divrate;
    mtu0->tcnt += next_time;
    mtu0->div_round %= divrate;
    tgra_event(mtu0);
	/* 次回割り込み時間 タイマ割を使わない場合は0xFFFFFFFFとする */
	diff[0] = ((mtu0->tier & 0x01) == 0) ? 0xFFFFFFFF : (mtu0->tgra - mtu0->tcnt);
	diff[1] = ((mtu0->tier & 0x02) == 0) ? 0xFFFFFFFF : (mtu0->tgrb - mtu0->tcnt);
	diff[2] = ((mtu0->tier & 0x04) == 0) ? 0xFFFFFFFF : (mtu0->tgrc - mtu0->tcnt);
	diff[3] = ((mtu0->tier & 0x08) == 0) ? 0xFFFFFFFF : (mtu0->tgrd - mtu0->tcnt);

    /* 最も短い時間を次回割り込みとする*/	
	for (min_time = diff[0], i = 1; i < 4; i++) {
        if (min_time > diff[i]) {
            min_time = diff[i];
        }
    }
	next_time = (int64_t)(min_time * divrate);
	next_time /= iclk;
	next_time += qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    timer_mod(&mtu0->timer, next_time);
	printf("mtu0->tgra = %d, mtu0->tcnt = %d, next_time = %lld\n",mtu0->tgra, mtu0->tcnt, next_time);
}

static void timer_event0(void *opaque)
{
    RMTU0State *mtu0 = opaque;

    timer_events(mtu0);
}

static void rmtu0_reset(DeviceState *dev)
{
    RMTU0State *mtu0 = RMTU0(dev);
    mtu0->tcr   = 0x00;
    mtu0->tmdr1 = 0x00;
    mtu0->tiorh = 0x00;
    mtu0->tiorl = 0x00;
    mtu0->tier  = 0x00;
    mtu0->tsr   = 0xC0;
    mtu0->tcnt  = 0x00;
    mtu0->tgra  = 0xFFFF;
    mtu0->tgrb  = 0xFFFF;
    mtu0->tgrc  = 0xFFFF;
    mtu0->tgrd  = 0xFFFF;
    mtu0->tgre  = 0xFFFF;
    mtu0->tgrf  = 0xFFFF;
    mtu0->tier2 = 0x00;
    mtu0->tsr2  = 0xC0;
    mtu0->tbtm  = 0x00;
}

static void rmtu0_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RMTU0State *mtu0 = RMTU0(obj);

    memory_region_init_io(&mtu0->memory, OBJECT(mtu0), &mtu0_ops,
                          mtu0, "renesas-mtu0", 0x28);
    sysbus_init_mmio(d, &mtu0->memory);

    sysbus_init_irq(d, &mtu0->tgia);
    sysbus_init_irq(d, &mtu0->tgib);
    sysbus_init_irq(d, &mtu0->tgic);
    sysbus_init_irq(d, &mtu0->tgid);
    sysbus_init_irq(d, &mtu0->tciv);
    sysbus_init_irq(d, &mtu0->tgie);
    sysbus_init_irq(d, &mtu0->tgif);
    
//    mtu0->timer = timer_new_us(QEMU_CLOCK_VIRTUAL, timer_event0, mtu0);
	timer_init_us(&mtu0->timer, QEMU_CLOCK_VIRTUAL, timer_event0, mtu0);
	gpRMTU0State = mtu0;
}

RMTU0State * Get_RMTU0State(void)
{
	return gpRMTU0State;
}

static const VMStateDescription vmstate_rmtu0 = {
    .name = "rx-mtu0",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rmtu0_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RMTU0State, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rmtu0_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rmtu0;
    dc->reset = rmtu0_reset;
    device_class_set_props(dc, rmtu0_properties);
}

static const TypeInfo rmtu0_info = {
    .name       = TYPE_RENESAS_MTU0,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RMTU0State),
    .instance_init = rmtu0_init,
    .class_init = rmtu0_class_init,
};

static void rmtu0_register_types(void)
{
    type_register_static(&rmtu0_info);
}

type_init(rmtu0_register_types)

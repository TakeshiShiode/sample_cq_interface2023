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
#include "hw/timer/renesas_mtu2.h"
#include "hw/timer/renesas_mtu34.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static RMTU2State *gpRMTU2State = NULL;
static void timer_events(RMTU2State *mtu2);
static uint64_t read_tcnt(RMTU2State *mtu2);

static uint64_t mtu2_read(void *opaque, hwaddr addr, unsigned size)
{
    RMTU2State *mtu2 = opaque;
    uint64_t ret;
    switch(addr){
        case R_TCR_2:     ret = mtu2->tcr;    break;
        case R_TMDR1_2:   ret = mtu2->tmdr1;  break;
        case R_TIOR_2:    ret = mtu2->tiorh;  break;
        case R_TIER_2:    ret = mtu2->tier;   break;
        case R_TSR_2:     ret = mtu2->tsr;    break;
        case R_TCNT_2:    ret = read_tcnt(mtu2);   break;
        case R_TGRA_2:    ret = mtu2->tgra;   break;
        case R_TGRB_2:    ret = mtu2->tgrb;   break;
        case R_TICCR_2:	  ret = mtu2->ticcr;  break;	
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


static void mtu2_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RMTU2State *mtu2 = opaque;
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
        case R_TCR_2:      mtu2->tcr   = latval;   break;
        case R_TMDR1_2:    mtu2->tmdr1 = latval;   break;
        case R_TIOR_2:     mtu2->tiorh = latval;   break;
        case R_TIER_2:     mtu2->tier  = latval;   break;
        case R_TSR_2:      mtu2->tsr   = latval;   break;
        case R_TCNT_2:     mtu2->tcnt = latval;	 break;
        case R_TGRA_2:     mtu2->tgra  = latval;   break;
        case R_TGRB_2:     mtu2->tgrb  = latval;   break;
        case R_TICCR_2:    mtu2->ticcr  = latval;   break;
        break;
        
        default:
            break;
        }
}

static const MemoryRegionOps mtu2_ops = {
    .write = mtu2_write,
    .read  = mtu2_read,
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


static void tgra_event(RMTU2State *mtu2)
{
    /* TGRA */
    if(mtu2->tcnt >= mtu2->tgra){
        /* interrupt */
        if(mtu2->tier & 0x01){
            qemu_irq_pulse(mtu2->tgia);
            mtu2->tsr |= 0x01;
        }
        /* tcnt clear */
        if( (mtu2->tcr & 0xE0) == 0x20 ){
            mtu2->tcnt = mtu2->tcnt - mtu2->tgra;
        }
    }

    /* TGRB */
    if(mtu2->tcnt >= mtu2->tgrb){
        /* interrupt */
        if(mtu2->tier & 0x02){
            qemu_irq_pulse(mtu2->tgib);
            mtu2->tsr |= 0x02;
        }
        /* tcnt clear */
        if( (mtu2->tcr & 0xE0) == 0x40 ){
            mtu2->tcnt = mtu2->tcnt - mtu2->tgrb;
        }
    }
}

static void timer_events(RMTU2State *mtu2)
{
    int64_t delta, now = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    int64_t tpsc = (mtu2->tcr & 0x07);
    int64_t next_time = 0;
    int64_t min_time = 0;
    int64_t i;
	uint32_t diff[4] = {0};
    int divrate = 1;
    const int iclk = 96;
    
	delta = (now - mtu2->tick) * iclk;
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
    
    mtu2->div_round += delta; 
	mtu2->tick = now;
	 //経過時間計算
    next_time = mtu2->div_round / divrate;
    mtu2->tcnt += next_time;
    mtu2->div_round %= divrate;
    tgra_event(mtu2);
	/* 次回割り込み時間 タイマ割を使わない場合は0xFFFFFFFFとする */
	diff[0] = ((mtu2->tier & 0x01) == 0) ? 0xFFFFFFFF : (mtu2->tgra - mtu2->tcnt);
	diff[1] = ((mtu2->tier & 0x02) == 0) ? 0xFFFFFFFF : (mtu2->tgrb - mtu2->tcnt);
    /* 最も短い時間を次回割り込みとする*/	
	for (min_time = diff[0], i = 1; i < 2; i++) {
        if (min_time > diff[i]) {
            min_time = diff[i];
        }
    }
	next_time = (int64_t)(min_time * divrate);
	next_time /= iclk;
	next_time += qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    timer_mod(&mtu2->timer, next_time);
	printf("mtu2->tgra = %d, mtu2->tcnt = %d, next_time = %lld\n",mtu2->tgra, mtu2->tcnt, next_time);
}

static void timer_event1(void *opaque)
{
    RMTU2State *mtu2 = opaque;

    timer_events(mtu2);
}

static uint64_t read_tcnt(RMTU2State *mtu2)
{
	uint8_t * pMatComMemory = NULL;
	uint16_t tcnt16 = 0;
	uint32_t tcnt32 = 0;
	uint64_t ret = 0;
	//位相係数モードの確認
	if( (0x04<=mtu2->tmdr1)&&(mtu2->tmdr1<=0x07) ){
		pMatComMemory = GetMatBLDCComMemory();
		//共有メモリのMTU1-TCNTのエンコーダ参照
		tcnt16 = *(uint16_t *)(pMatComMemory+TCNT2_OFFSET);
		ret = (uint64_t)tcnt16;
	}
	else{
		//通常モードの場合は内部のTCNTを回答
		tcnt32 = mtu2->tcnt;
		ret = (uint64_t)tcnt32;
	}
	
	return ret;
}


static void rmtu2_reset(DeviceState *dev)
{
    RMTU2State *mtu2 = RMTU2(dev);
    mtu2->tcr   = 0x00;
    mtu2->tmdr1 = 0x00;
    mtu2->tiorh = 0x00;
    mtu2->tiorl = 0x00;
    mtu2->tier  = 0x00;
    mtu2->tsr   = 0xC0;
    mtu2->tcnt  = 0x00;
    mtu2->tgra  = 0xFFFF;
    mtu2->tgrb  = 0xFFFF;
    mtu2->ticcr= 0x00;
}

static void rmtu2_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RMTU2State *mtu2 = RMTU2(obj);

    memory_region_init_io(&mtu2->memory, OBJECT(mtu2), &mtu2_ops,
                          mtu2, "renesas-mtu2", 0x12);
    sysbus_init_mmio(d, &mtu2->memory);

    sysbus_init_irq(d, &mtu2->tgia);
    sysbus_init_irq(d, &mtu2->tgib);
    
//    mtu2->timer = timer_new_us(QEMU_CLOCK_VIRTUAL, timer_event0, mtu2);
	timer_init_us(&mtu2->timer, QEMU_CLOCK_VIRTUAL, timer_event1, mtu2);
	gpRMTU2State = mtu2;
}

RMTU2State * Get_RMTU2State(void)
{
	return gpRMTU2State;
}

static const VMStateDescription vmstate_rmtu2 = {
    .name = "rx-mtu2",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rmtu2_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RMTU2State, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rmtu2_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rmtu2;
    dc->reset = rmtu2_reset;
    device_class_set_props(dc, rmtu2_properties);
}

static const TypeInfo rmtu2_info = {
    .name       = TYPE_RENESAS_MTU2,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RMTU2State),
    .instance_init = rmtu2_init,
    .class_init = rmtu2_class_init,
};

static void rmtu2_register_types(void)
{
    type_register_static(&rmtu2_info);
}

type_init(rmtu2_register_types)

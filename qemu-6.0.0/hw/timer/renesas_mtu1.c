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
#include "hw/timer/renesas_mtu1.h"
#include "hw/timer/renesas_mtu34.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static RMTU1State *gpRMTU1State = NULL;
static void timer_events(RMTU1State *mtu1);
static uint64_t read_tcnt(RMTU1State *mtu1);

static uint64_t mtu1_read(void *opaque, hwaddr addr, unsigned size)
{
    RMTU1State *mtu1 = opaque;
    uint64_t ret;
    switch(addr){
        case R_TCR_1:     ret = mtu1->tcr;    break;
        case R_TMDR1_1:   ret = mtu1->tmdr1;  break;
        case R_TIOR_1:    ret = mtu1->tiorh;  break;
        case R_TIER_1:    ret = mtu1->tier;   break;
        case R_TSR_1:     ret = mtu1->tsr;    break;
        case R_TCNT_1:    ret = read_tcnt(mtu1);   break;
        case R_TGRA_1:    ret = mtu1->tgra;   break;
        case R_TGRB_1:    ret = mtu1->tgrb;   break;
        case R_TICCR_1:	  ret = mtu1->ticcr;  break;	
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


static void mtu1_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RMTU1State *mtu1 = opaque;
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
        case R_TCR_1:      mtu1->tcr   = latval;   break;
        case R_TMDR1_1:    mtu1->tmdr1 = latval;   break;
        case R_TIOR_1:     mtu1->tiorh = latval;   break;
        case R_TIER_1:     mtu1->tier  = latval;   break;
        case R_TSR_1:      mtu1->tsr   = latval;   break;
        case R_TCNT_1:
        	{     
				mtu1->tcnt = latval;
				if(latval == 0){
					uint8_t * pMatComMemory = NULL;
					pMatComMemory = GetMatBLDCComMemory();
					//共有メモリ経由でMatlab側位置カウンタをクリア
					*(pMatComMemory+TCNT1_CLEAR_OFFSET) = 1;
				}
        	
        	}
        	break;
        case R_TGRA_1:     mtu1->tgra  = latval;   break;
        case R_TGRB_1:     mtu1->tgrb  = latval;   break;
        case R_TICCR_1:    mtu1->ticcr  = latval;   break;
        break;
        
        default:
            break;
        }
}

static const MemoryRegionOps mtu1_ops = {
    .write = mtu1_write,
    .read  = mtu1_read,
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


static void tgra_event(RMTU1State *mtu1)
{
    /* TGRA */
    if(mtu1->tcnt >= mtu1->tgra){
        /* interrupt */
        if(mtu1->tier & 0x01){
            qemu_irq_pulse(mtu1->tgia);
            mtu1->tsr |= 0x01;
        }
        /* tcnt clear */
        if( (mtu1->tcr & 0xE0) == 0x20 ){
            mtu1->tcnt = mtu1->tcnt - mtu1->tgra;
        }
    }

    /* TGRB */
    if(mtu1->tcnt >= mtu1->tgrb){
        /* interrupt */
        if(mtu1->tier & 0x02){
            qemu_irq_pulse(mtu1->tgib);
            mtu1->tsr |= 0x02;
        }
        /* tcnt clear */
        if( (mtu1->tcr & 0xE0) == 0x40 ){
            mtu1->tcnt = mtu1->tcnt - mtu1->tgrb;
        }
    }
}

static void timer_events(RMTU1State *mtu1)
{
    int64_t delta, now = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    int64_t tpsc = (mtu1->tcr & 0x07);
    int64_t next_time = 0;
    int64_t min_time = 0;
    int64_t i;
	uint32_t diff[4] = {0};
    int divrate = 1;
    const int iclk = 96;
    
	delta = (now - mtu1->tick) * iclk;
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
    
    mtu1->div_round += delta; 
	mtu1->tick = now;
	 //経過時間計算
    next_time = mtu1->div_round / divrate;
    mtu1->tcnt += next_time;
    mtu1->div_round %= divrate;
    tgra_event(mtu1);
	/* 次回割り込み時間 タイマ割を使わない場合は0xFFFFFFFFとする */
	diff[0] = ((mtu1->tier & 0x01) == 0) ? 0xFFFFFFFF : (mtu1->tgra - mtu1->tcnt);
	diff[1] = ((mtu1->tier & 0x02) == 0) ? 0xFFFFFFFF : (mtu1->tgrb - mtu1->tcnt);
    /* 最も短い時間を次回割り込みとする*/	
	for (min_time = diff[0], i = 1; i < 2; i++) {
        if (min_time > diff[i]) {
            min_time = diff[i];
        }
    }
	next_time = (int64_t)(min_time * divrate);
	next_time /= iclk;
	next_time += qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    timer_mod(&mtu1->timer, next_time);
	printf("mtu1->tgra = %d, mtu1->tcnt = %d, next_time = %lld\n",mtu1->tgra, mtu1->tcnt, next_time);
}

static void timer_event1(void *opaque)
{
    RMTU1State *mtu1 = opaque;

    timer_events(mtu1);
}

static uint64_t read_tcnt(RMTU1State *mtu1)
{
	uint8_t * pMatComMemory = NULL;
	uint16_t tcnt16 = 0;
	uint32_t tcnt32 = 0;
	uint64_t ret = 0;
	//位相係数モードの確認
	if( (0x04<=mtu1->tmdr1)&&(mtu1->tmdr1<=0x07) ){
		pMatComMemory = GetMatBLDCComMemory();
		//共有メモリのMTU1-TCNTのエンコーダ参照
		tcnt16 = *(uint16_t *)(pMatComMemory+TCNT1_OFFSET);
		ret = (uint64_t)tcnt16;
	}
	else{
		//通常モードの場合は内部のTCNTを回答
		tcnt32 = mtu1->tcnt;
		ret = (uint64_t)tcnt32;
	}
	
	return ret;
}

static void rmtu1_reset(DeviceState *dev)
{
    RMTU1State *mtu1 = RMTU1(dev);
    mtu1->tcr   = 0x00;
    mtu1->tmdr1 = 0x00;
    mtu1->tiorh = 0x00;
    mtu1->tiorl = 0x00;
    mtu1->tier  = 0x00;
    mtu1->tsr   = 0xC0;
    mtu1->tcnt  = 0x00;
    mtu1->tgra  = 0xFFFF;
    mtu1->tgrb  = 0xFFFF;
    mtu1->ticcr= 0x00;
}

static void rmtu1_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RMTU1State *mtu1 = RMTU1(obj);

    memory_region_init_io(&mtu1->memory, OBJECT(mtu1), &mtu1_ops,
                          mtu1, "renesas-mtu1", 0x12);
    sysbus_init_mmio(d, &mtu1->memory);

    sysbus_init_irq(d, &mtu1->tgia);
    sysbus_init_irq(d, &mtu1->tgib);
    
//    mtu1->timer = timer_new_us(QEMU_CLOCK_VIRTUAL, timer_event0, mtu1);
	timer_init_us(&mtu1->timer, QEMU_CLOCK_VIRTUAL, timer_event1, mtu1);
	gpRMTU1State = mtu1;
}

RMTU1State * Get_RMTU1State(void)
{
	return gpRMTU1State;
}

static const VMStateDescription vmstate_rmtu1 = {
    .name = "rx-mtu1",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rmtu1_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RMTU1State, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rmtu1_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rmtu1;
    dc->reset = rmtu1_reset;
    device_class_set_props(dc, rmtu1_properties);
}

static const TypeInfo rmtu1_info = {
    .name       = TYPE_RENESAS_MTU1,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RMTU1State),
    .instance_init = rmtu1_init,
    .class_init = rmtu1_class_init,
};

static void rmtu1_register_types(void)
{
    type_register_static(&rmtu1_info);
}

type_init(rmtu1_register_types)

/*
 * Renesas mtu3_34 timer
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
#include "hw/timer/renesas_mtu34.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"
#include "qemu/main-loop.h"

#include "hw/timer/renesas_mtu0.h"
#include "hw/timer/renesas_mtu1.h"
#include "hw/timer/renesas_mtu2.h"

static HANDLE gMatBLDCEvent = NULL;
static HANDLE MatBLDCComMemory = NULL;
static void *pMatBLDCComMemory = NULL;

static void tgr3_event(RMTU34State *mtu34);
static void tgr4_event(RMTU34State *mtu34);
static void rmtu34_timerstart(uint8_t tstr);
static void write_tgrd3(RMTU34State *mtu34, uint16_t latval);
static void write_tgrc4(RMTU34State *mtu34, uint16_t latval);
static void write_tgrd4(RMTU34State *mtu34, uint16_t latval);

static RMTU34State *gpRMTU34State = NULL;

static uint64_t mtu34_read(void *opaque, hwaddr addr, unsigned size)
{
    RMTU34State *mtu34 = opaque;
    uint64_t ret;
    uint8_t * pMatComMemory = GetMatBLDCComMemory();
    
    switch(addr){
        /* MTU3 */
        case R_TCR_3:       ret = mtu34->tcr_3;     break;
        case R_TMDR1_3:     ret = mtu34->tmdr1_3;   break;
        case R_TIORH_3:     ret = mtu34->tiorh_3;   break;
        case R_TIORL_3:     ret = mtu34->tiorl_3;   break;
        case R_TIER_3:      ret = mtu34->tier_3;    break;
        case R_TCNT_3:      ret = mtu34->tcnt_3;    break;
        case R_TGRA_3:      ret = mtu34->tgra_3;    break;
        case R_TGRB_3:      ret = mtu34->tgrb_3;    break;
        case R_TGRC_3:      ret = mtu34->tgrc_3;    break;
        case R_TGRD_3:      ret = mtu34->tgrd_3;    break;
        case R_TGRE_3:      ret = mtu34->tgre_3;    break;
        case R_TSR_3:       ret = mtu34->tsr_3;     break;
        case R_TBTM_3:      ret = mtu34->tbtm_3;    break;
        /* MTU4 */
        case R_TCR_4:       ret = mtu34->tcr_4;     break;
        case R_TMDR1_4:     ret = mtu34->tmdr1_4;   break;
        case R_TIORH_4:     ret = mtu34->tiorh_4;   break;
        case R_TIORL_4:     ret = mtu34->tiorl_4;   break;
        case R_TIER_4:      ret = mtu34->tier_4;    break;
        case R_TCNT_4:      ret = mtu34->tcnt_4;    break;
        case R_TGRA_4:      ret = mtu34->tgra_4;    break;
        case R_TGRB_4:      ret = mtu34->tgrb_4;    break;
        case R_TGRC_4:      ret = mtu34->tgrc_4;    break;
        case R_TGRD_4:      ret = mtu34->tgrd_4;    break;
        case R_TGRE_4:      ret = mtu34->tgre_4;    break;
        case R_TSR_4:       ret = mtu34->tsr_4;     break;
        case R_TBTM_4:      ret = mtu34->tbtm_4;    break;
        case R_TADCR_4:     ret = mtu34->tadcr_4;   break;
        case R_TADCORA_4:   ret = mtu34->tadcora_4; break;
        case R_TADCORB_4:   ret = mtu34->tadcorb_4;  break;
        case R_TADCOBRA_4:  ret = mtu34->tadcobra_4;break;
        case R_TADCOBRB_4:  ret = mtu34->tadcobrb_4;break;
        /* COMMON */
        case R_TOERA:       ret = mtu34->toera = *(uint8_t *)(pMatComMemory+TOERA_OFFSET); break; 
        case R_TGCRA:       ret = mtu34->tgcra;     break;
        case R_TOCR1A:      ret = mtu34->tocr1a;    break;
        case R_TOCR2A:      ret = mtu34->tocr2a;    break;
        case R_TCDRA:       ret = mtu34->tcdra;     break;
        case R_TDDRA:       ret = mtu34->tddra;     break;
        case R_TDERA:       ret = mtu34->tdera;     break;
        case R_TCNTSA:      ret = mtu34->tcntsa;    break;
        case R_TCBRA:       ret = mtu34->tcbra;     break;
        case R_TITCR1A:     ret = mtu34->titcr1a;   break;
        case R_TITCR2A:     ret = mtu34->titcr2a;   break;
        case R_TITCNT1A:    ret = mtu34->titcnt1a;  break;
        case R_TITCNT2A:    ret = mtu34->titcnt2a;  break;
        case R_TBTERA:      ret = mtu34->tbtera;    break;
        case R_TOLBRA:      ret = mtu34->tolbra;    break;
        case R_TMDR2A:      ret = mtu34->tmdr2a;    break;
        case R_TITMRA:      ret = mtu34->titmra;    break;
        case R_TWCRA:       ret = mtu34->twcra;     break;
        case R_TSTRA:       ret = mtu34->tstra;     break;
        case R_TSYRA:       ret = mtu34->tsyra;     break;
        case R_TCSYSTR:     ret = mtu34->tcsystr;   break;
        case R_TRWERA:      ret = mtu34->trwera;    break;
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


static void mtu34_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RMTU34State *mtu34 = opaque;
    uint64_t latval;
    uint8_t * pMatComMemory = GetMatBLDCComMemory();
    
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
        /* MTU3 */
        case R_TCR_3:       mtu34->tcr_3      = latval;   break;
        case R_TMDR1_3:     mtu34->tmdr1_3    = latval;   break;
        case R_TIORH_3:     mtu34->tiorh_3    = latval;   break;
        case R_TIORL_3:     mtu34->tiorl_3    = latval;   break;
        case R_TIER_3:      mtu34->tier_3     = latval;   break;
        case R_TCNT_3:      mtu34->tcnt_3     = latval;   break;
        case R_TGRA_3:      mtu34->tgra_3     = latval;   break;
        case R_TGRB_3:      mtu34->tgrb_3     = latval;   break;
        case R_TGRC_3:      mtu34->tgrc_3     = latval;   break;
        case R_TGRD_3:      write_tgrd3(mtu34, latval);   break;
        case R_TGRE_3:      mtu34->tgre_3     = latval;   break;
        case R_TSR_3:       mtu34->tsr_3      = latval;   break;
        case R_TBTM_3:      mtu34->tbtm_3     = latval;   break;
        /* MTU4 */
        case R_TCR_4:       mtu34->tcr_4      = latval;   break;
        case R_TMDR1_4:     mtu34->tmdr1_4    = latval;   break;
        case R_TIORH_4:     mtu34->tiorh_4    = latval;   break;
        case R_TIORL_4:     mtu34->tiorl_4    = latval;   break;
        case R_TIER_4:      mtu34->tier_4     = latval;   break;
        case R_TCNT_4:      mtu34->tcnt_4     = latval;   break;
        case R_TGRA_4:      mtu34->tgra_4     = latval;   break;
        case R_TGRB_4:      mtu34->tgrb_4     = latval;   break;
        case R_TGRC_4:      write_tgrc4(mtu34, latval);   break;
        case R_TGRD_4:      write_tgrd4(mtu34, latval);   break;
        case R_TGRE_4:      mtu34->tgre_4     = latval;   break;
        case R_TSR_4:       mtu34->tsr_4      = latval;   break;
        case R_TBTM_4:      mtu34->tbtm_4     = latval;   break;
        case R_TADCR_4:     mtu34->tadcr_4    = latval;   break;
        case R_TADCORA_4:   mtu34->tadcora_4  = latval;   break;
        case R_TADCORB_4:   mtu34->tadcorb_4   = latval;   break;
        case R_TADCOBRA_4:  mtu34->tadcobra_4 = latval;   break;
        case R_TADCOBRB_4:  mtu34->tadcobrb_4 = latval;   break;
        /* COMMON */
        case R_TOERA:       mtu34->toera = *(uint8_t *)(pMatComMemory+TOERA_OFFSET) = (uint8_t)latval;   break;
        case R_TGCRA:       mtu34->tgcra     = latval;   break;
        case R_TOCR1A:      mtu34->tocr1a    = latval;   break;
        case R_TOCR2A:      mtu34->tocr2a    = latval;   break;
        case R_TCDRA:       mtu34->tcdra     = latval;   break;
        case R_TDDRA:       mtu34->tddra     = latval;   break;
        case R_TDERA:       mtu34->tdera     = latval;   break;
        case R_TCNTSA:      mtu34->tcntsa    = latval;   break;
        case R_TCBRA:       mtu34->tcbra     = latval;   break;
        case R_TITCR1A:     mtu34->titcr1a   = latval;   break;
        case R_TITCR2A:     mtu34->titcr2a   = latval;   break;
        case R_TITCNT1A:    mtu34->titcnt1a  = latval;   break;
        case R_TITCNT2A:    mtu34->titcnt2a  = latval;   break;
        case R_TBTERA:      mtu34->tbtera    = latval;   break;
        case R_TOLBRA:      mtu34->tolbra    = latval;   break;
        case R_TMDR2A:      mtu34->tmdr2a    = latval;   break;
        case R_TITMRA:      mtu34->titmra    = latval;   break;
        case R_TWCRA:       mtu34->twcra     = latval;   break;
        case R_TSTRA:       mtu34->tstra     = latval; 	rmtu34_timerstart(mtu34->tstra);   break;
        case R_TSYRA:       mtu34->tsyra     = latval;   break;
        case R_TCSYSTR:     mtu34->tcsystr   = latval;   break;
        case R_TRWERA:      mtu34->trwera    = latval;   break;
        default:
            break;
        }
}

static const MemoryRegionOps mtu34_ops = {
    .write = mtu34_write,
    .read  = mtu34_read,
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


static void tgr3_event(RMTU34State *mtu34)
{
    /* TGRA */
    if(mtu34->tcnt_3 >= mtu34->tgra_3){
        /* interrupt */
        if(mtu34->tier_3 & 0x01){
            qemu_irq_pulse(mtu34->tgia_3);
            mtu34->tsr_3 |= 0x01;
        }
        /* tcnt clear */
        if( (mtu34->tcr_3 & 0xE0) == 0x20 ){
            mtu34->tcnt_3 = 0;
        }
    }

    /* TGRB */
    if(mtu34->tcnt_3 >= mtu34->tgrb_3){
        /* interrupt */
        if(mtu34->tier_3 & 0x02){
            qemu_irq_pulse(mtu34->tgib_3);
            mtu34->tsr_3 |= 0x02;
        }
        /* tcnt clear */
        if( (mtu34->tcr_3 & 0xE0) == 0x40 ){
            mtu34->tcnt_3 = 0;
        }
    }

    /* TGRC */
    if(mtu34->tcnt_3 >= mtu34->tgrc_3){
        /* interrupt */
        if(mtu34->tier_3 & 0x04){
            qemu_irq_pulse(mtu34->tgic_3);
            mtu34->tsr_3 |= 0x04;
        }
        /* tcnt clear */
        if( (mtu34->tcr_3 & 0xE0) == 0xA0 ){
            mtu34->tcnt_3 = 0;
        }
    }

    /* TGRD */
    if(mtu34->tcnt_3 >= mtu34->tgrd_3){
        /* interrupt */
        if(mtu34->tier_3 & 0x08){
            qemu_irq_pulse(mtu34->tgid_3);
            mtu34->tsr_3 |= 0x08;
        }
        /* tcnt clear */
        if( (mtu34->tcr_3 & 0xE0) == 0xC0 ){
            mtu34->tcnt_3 = 0;
        }
    }

    /* TGRE */
//    if(mtu34->tcnt >= mtu34->tgre){
//        /* interrupt */
//        if(mtu34->tier2 & 0x01){
//            qemu_irq_pulse(mtu->tgie);
//            mtu34->tsr2 |= 0x01;
//        }
//    }
}

static void tgr4_event(RMTU34State *mtu34)
{
    /* TGRA */
    if(mtu34->tcnt_4 >= mtu34->tgra_4){
        /* interrupt */
        if(mtu34->tier_4 & 0x01){
            qemu_irq_pulse(mtu34->tgia_4);
            mtu34->tsr_4 |= 0x01;
        }
        /* tcnt clear */
        if( (mtu34->tcr_4 & 0xE0) == 0x20 ){
            mtu34->tcnt_4 = 0;
        }
    }

    /* TGRB */
    if(mtu34->tcnt_4 >= mtu34->tgrb_4){
        /* interrupt */
        if(mtu34->tier_4 & 0x02){
            qemu_irq_pulse(mtu34->tgib_4);
            mtu34->tsr_4 |= 0x02;
        }
        /* tcnt clear */
        if( (mtu34->tcr_4 & 0xE0) == 0x40 ){
            mtu34->tcnt_4 = 0;
        }
    }

    /* TGRC */
    if(mtu34->tcnt_4 >= mtu34->tgrc_4){
        /* interrupt */
        if(mtu34->tier_4 & 0x04){
            qemu_irq_pulse(mtu34->tgic_4);
            mtu34->tsr_4 |= 0x04;
        }
        /* tcnt clear */
        if( (mtu34->tcr_4 & 0xE0) == 0xA0 ){
            mtu34->tcnt_4 = 0;
        }
    }

    /* TGRD */
    if(mtu34->tcnt_4 >= mtu34->tgrd_4){
        /* interrupt */
        if(mtu34->tier_4 & 0x08){
            qemu_irq_pulse(mtu34->tgid_4);
            mtu34->tsr_4 |= 0x08;
        }
        /* tcnt clear */
        if( (mtu34->tcr_4 & 0xE0) == 0xC0 ){
            mtu34->tcnt_4 = 0;
        }
    }

    /* TGRE */
//    if(mtu34->tcnt >= mtu34->tgre){
//        /* interrupt */
//        if(mtu34->tier2 & 0x01){
//            qemu_irq_pulse(mtu->tgie);
//            mtu34->tsr2 |= 0x01;
//        }
//    }
}

static void timer_events3(RMTU34State *mtu34)
{
//  タイマスタートレジスタチェック(RX62T_ICLK=96MHz)
    int64_t delta, now = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    int64_t next_time = 0;
    int64_t tpsc = (mtu34->tcr_3 & 0x07);
    int64_t min_time = 0;
    int64_t i;
	uint32_t diff[4] = {0};
    int divrate = 1;
    const int iclk = 96;
    
    delta = (now - mtu34->tick_3) * iclk;
    switch(tpsc){
        case 0: divrate = 1;	break;    /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 1)*/
        case 1: divrate = 4;	break;    /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 4)*/
        case 2: divrate = 16;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 16)*/
        case 3: divrate = 64;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        case 4: divrate = 256;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        case 5: divrate = 1024;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        default:				break;
    }
    
    mtu34->div_round_3 += delta; 
	mtu34->tick_3 = now;
	 //経過時間計算
    next_time = mtu34->div_round_3 / divrate;
    mtu34->tcnt_3 += next_time;
    mtu34->div_round_3 %= divrate;
    tgr3_event(mtu34);
	/* 次回割り込み時間 タイマ割を使わない場合は0xFFFFFFFFとする */
	diff[0] = ((mtu34->tier_3 & 0x01) == 0) ? 0xFFFFFFFF : (mtu34->tgra_3 - mtu34->tcnt_3);
	diff[1] = ((mtu34->tier_3 & 0x02) == 0) ? 0xFFFFFFFF : (mtu34->tgrb_3 - mtu34->tcnt_3);
	diff[2] = ((mtu34->tier_3 & 0x04) == 0) ? 0xFFFFFFFF : (mtu34->tgrc_3 - mtu34->tcnt_3);
	diff[3] = ((mtu34->tier_3 & 0x08) == 0) ? 0xFFFFFFFF : (mtu34->tgrd_3 - mtu34->tcnt_3);
    /* 最も短い時間を次回割り込みとする*/	
	for (min_time = diff[0], i = 1; i < 4; i++) {
        if (min_time > diff[i]) {
            min_time = diff[i];
        }
    }
    //if(mtu34->tmdr1_3 & 0x0E){
    //    /* complementary PWM mode */
    //}
	next_time = (int64_t)(min_time * divrate);
	next_time /= iclk;
	next_time += qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    timer_mod(&mtu34->timer3, next_time);
	printf("mtu34->tgra_3 = %d, mtu34->tcnt_3 = %d, next_time = %lld\n",mtu34->tgra_3, mtu34->tcnt_3, next_time);
}

static void timer_events4(RMTU34State *mtu34)
{
//  タイマスタートレジスタチェック(RX62T_ICLK=96MHz)
    int64_t delta, now = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    int64_t next_time = 0;
    int64_t tpsc = (mtu34->tcr_4 & 0x07);
    int64_t min_time = 0;
    int64_t i;
	uint32_t diff[4] = {0};
    int divrate = 1;
    const int iclk = 96;
    
    delta = (now - mtu34->tick_4) * iclk;
    switch(tpsc){
        case 0: divrate = 1;	break;    /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 1)*/
        case 1: divrate = 4;	break;    /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 4)*/
        case 2: divrate = 16;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 16)*/
        case 3: divrate = 64;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        case 4: divrate = 256;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        case 5: divrate = 1024;	break;   /*NANOSECONDS_PER_SECOND / (RX62T_ICLK / 64)*/
        default:				break;
    }
    
    mtu34->div_round_4 += delta; 
	mtu34->tick_4 = now;
	 //経過時間計算
    next_time = mtu34->div_round_4 / divrate;
    mtu34->tcnt_4 += next_time;
    mtu34->div_round_4 %= divrate;
    tgr4_event(mtu34);
	/* 次回割り込み時間 タイマ割を使わない場合は0xFFFFFFFFとする */
	diff[0] = ((mtu34->tier_4 & 0x01) == 0) ? 0xFFFFFFFF : (mtu34->tgra_4 - mtu34->tcnt_4);
	diff[1] = ((mtu34->tier_4 & 0x02) == 0) ? 0xFFFFFFFF : (mtu34->tgrb_4 - mtu34->tcnt_4);
	diff[2] = ((mtu34->tier_4 & 0x04) == 0) ? 0xFFFFFFFF : (mtu34->tgrc_4 - mtu34->tcnt_4);
	diff[3] = ((mtu34->tier_4 & 0x08) == 0) ? 0xFFFFFFFF : (mtu34->tgrd_4 - mtu34->tcnt_4);
    /* 最も短い時間を次回割り込みとする*/	
	for (min_time = diff[0], i = 1; i < 4; i++) {
        if (min_time > diff[i]) {
            min_time = diff[i];
        }
    }
    //if(mtu34->tmdr1_4 & 0x0E){
    //    /* complementary PWM mode */
    //}
	next_time = (int64_t)(min_time * divrate);
	next_time /= iclk;
	next_time += qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    timer_mod(&mtu34->timer3, next_time);
	printf("mtu34->tgra_4 = %d, mtu34->tcnt_4 = %d, next_time = %lld\n",mtu34->tgra_4, mtu34->tcnt_4, next_time);
}


static void *complementary_pwm(void *opaque)
{
    RMTU34State *mtu34 = opaque;
    DWORD waitResult = WAIT_FAILED;
    
    while (1) {
        waitResult =WaitForSingleObject(gMatBLDCEvent, INFINITE);
        if( (waitResult == WAIT_OBJECT_0) || (waitResult == WAIT_TIMEOUT) ){
            /* interrupt enable and complementary_pwm mode */
            if( (mtu34->tier_4 & 0x10) && ( ( 0x0D <= (mtu34->tmdr1_3&0x0F) ) && ( (mtu34->tmdr1_3&0x0F) <= 0x0F ) ) ){
                qemu_mutex_lock_iothread();
                qemu_irq_pulse(mtu34->tciv_4);
                qemu_mutex_unlock_iothread();
            }
        }
        else{
            CloseHandle(gMatBLDCEvent);
        }
    }
    return NULL;
}

static void rmtu34_timerstart(uint8_t tstr)
{
    RMTU34State *mtu34 = gpRMTU34State;
    RMTU0State *mtu0 = Get_RMTU0State();
    RMTU1State *mtu1 = Get_RMTU1State();
    RMTU2State *mtu2 = Get_RMTU2State();
    
    uint32_t next_time;

    uint8_t cst0  = tstr & 0x01;
    uint8_t cst1  = tstr & 0x02;
    uint8_t cst2  = tstr & 0x04;
    uint8_t cst3  = tstr & 0x40;
    uint8_t cst4  = tstr & 0x80;
    uint8_t cclr0 = mtu0->tcr&0xE0;
    uint8_t cclr1 = mtu1->tcr&0xE0;
    uint8_t cclr2 = mtu2->tcr&0xE0;
    uint8_t cclr3 = mtu34->tcr_3&0xE0;
    uint8_t cclr4 = mtu34->tcr_4&0xE0;
    
    //スタートビットの確認
    //tickにどのタイマを足すかは、割込みクリアする条件のタイマを想定する
    if(cst0 != 0){
		if(cclr0==0x20){
			next_time = mtu0->tgra;
		}
		else{
			next_time = mtu0->tgrb;
		}
		mtu0->tick = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    	timer_mod(&mtu0->timer, mtu0->tick + next_time);
	}
	
    if(cst1 != 0){
		if( ( 0x04 <= (mtu1->tmdr1&0x0F) )&&( (mtu1->tmdr1&0x0F) <= 0x07 ) ){
			/* 位相係数モードのためタイマスタートしない */
		}
		else{
			if(cclr1==0x20){
				next_time = mtu1->tgra;
			}
			else{
				next_time = mtu1->tgrb;
			}
			mtu1->tick = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    		timer_mod(&mtu1->timer, mtu1->tick + next_time);
    	}
	}
	
    if(cst2 != 0){
		if( ( 0x04<= (mtu2->tmdr1&0x0F) )&&( (mtu2->tmdr1&0x0F) <= 0x07 ) ){
			/* 位相係数モードのためタイマスタートしない */
		}
		else{
			if(cclr2==0x20){
				next_time = mtu2->tgra;
			}
			else{
				next_time = mtu2->tgrb;
			}
			mtu2->tick = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    		timer_mod(&mtu2->timer, mtu2->tick + next_time);
    	}
	}

    if(cst3 != 0){
		if( ( 0x0D <= (mtu34->tmdr1_3&0x0F) ) && ( (mtu34->tmdr1_3&0x0F) <= 0x0F ) ){
			/* 相補PWMモードのためタイマスタートしない */
		}
		else{
			if(cclr3==0x20){
				next_time = mtu34->tgra_3;
			}
			else if(cclr3==0x40){
				next_time = mtu34->tgrb_3;
			}
			else if(cclr3==0xA0){
				next_time = mtu34->tgrc_3;
			}
			else{
				next_time = mtu34->tgrd_3;
			}
			mtu34->tick_3 = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    		timer_mod(&mtu34->timer3, mtu34->tick_3 + next_time);
    	}
	}

    if(cst4 != 0){
		if( ( 0x0D <= (mtu34->tmdr1_3&0x0F) )&&( (mtu34->tmdr1_3&0x0F) <= 0x0F ) ){
			/* 相補PWMモードのためタイマスタートしない(mtu3に連動) */
		}
		else{
			if(cclr4==0x20){
				next_time = mtu34->tgra_4;
			}
			else if(cclr4==0x40){
				next_time = mtu34->tgrb_4;
			}
			else if(cclr4==0xA0){
				next_time = mtu34->tgrc_4;
			}
			else{
				next_time = mtu34->tgrd_4;
			}
			mtu34->tick_4 = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
    		timer_mod(&mtu34->timer4, mtu34->tick_4 + next_time);
    	}
	}
}

static void timer_event3(void *opaque)
{
    RMTU34State *mtu34 = opaque;

    timer_events3(mtu34);
}

static void timer_event4(void *opaque)
{
    RMTU34State *mtu34 = opaque;

    timer_events4(mtu34);
}

static void write_tgrd3(RMTU34State *mtu34, uint16_t latval)
{
	uint8_t * pMatComMemory = NULL;
	uint16_t * tgrd = NULL;
	//相補PWMモードの確認
	if( ( 0x0D <= (mtu34->tmdr1_3&0x0F) ) && ( (mtu34->tmdr1_3&0x0F) <= 0x0F ) ){
		pMatComMemory = GetMatBLDCComMemory();
		tgrd = (uint16_t *)(pMatComMemory+TGRD3_OFFSET);
		*tgrd = latval;
		mtu34->tgrd_3 = latval;
		mtu34->tgrb_3 = latval;
		mtu34->tgre_3 = latval;
	}
	else{
		//通常モードの場合は内部のTGRDへ書込み
		mtu34->tgrd_3 = latval;
	}
}


static void write_tgrc4(RMTU34State *mtu34, uint16_t latval)
{
	uint8_t * pMatComMemory = NULL;
	uint16_t * tgrc = NULL;
	//相補PWMモードの確認
	if( ( 0x0D <= (mtu34->tmdr1_3&0x0F) ) && ( (mtu34->tmdr1_3&0x0F) <= 0x0F ) ){
		pMatComMemory = GetMatBLDCComMemory();
		tgrc = (uint16_t *)(pMatComMemory+TGRC4_OFFSET);
		*tgrc = latval;
		mtu34->tgrc_4 = latval;
		mtu34->tgra_4 = latval;
		mtu34->tgre_4 = latval;
	}
	else{
		//通常モードの場合は内部のTGRCへ書込み
		mtu34->tgrc_4 = latval;
	}
}

static void write_tgrd4(RMTU34State *mtu34, uint16_t latval)
{
	uint8_t * pMatComMemory = NULL;
	uint16_t * tgrd = NULL;
	//相補PWMモードの確認
	if( ( 0x0D <= (mtu34->tmdr1_3&0x0F) ) && ( (mtu34->tmdr1_3&0x0F) <= 0x0F ) ){
		pMatComMemory = GetMatBLDCComMemory();
		tgrd =(uint16_t *)(pMatComMemory+TGRD4_OFFSET);
		*tgrd = latval;
		mtu34->tgrd_4 = latval;
		mtu34->tgrb_4 = latval;
		mtu34->tgrf_4 = latval;
	}
	else{
		//通常モードの場合は内部のTGRCへ書込み
		mtu34->tgrd_4 = latval;
	}
}

static void rmtu34_reset(DeviceState *dev)
{
    RMTU34State *mtu34 = RMTU34(dev);
    //MTU3
    mtu34->tcr_3   		= 0x00;
    mtu34->tmdr1_3		= 0x00;
    mtu34->tiorh_3 		= 0x00;
    mtu34->tiorl_3 		= 0x00;
    mtu34->tier_3		= 0x00;
    mtu34->tcnt_3  		= 0x0000;
    mtu34->tgra_3		= 0xFFFF;
    mtu34->tgrb_3 		= 0xFFFF;
    mtu34->tgrc_3 		= 0xFFFF;
    mtu34->tgrd_3 		= 0xFFFF;
    mtu34->tgre_3 		= 0xFFFF;
    mtu34->tsr_3		= 0xC0;
    mtu34->tbtm_3 		= 0x00;
    //MTU4
    mtu34->tcr_4 		= 0x00;
    mtu34->tmdr1_4 		= 0x00;
    mtu34->tiorh_4 		= 0x00;
    mtu34->tiorl_4 		= 0x00;
    mtu34->tier_4		= 0x00;
    mtu34->tcnt_4 		= 0x0000;
    mtu34->tgra_4 		= 0xFFFF;
    mtu34->tgrb_4 		= 0xFFFF;
    mtu34->tgrc_4 		= 0xFFFF;
    mtu34->tgrd_4 		= 0xFFFF;
    mtu34->tgre_4 		= 0xFFFF;
    mtu34->tgrf_4 		= 0xFFFF;
    mtu34->tsr_4 		= 0xC0;
    mtu34->tbtm_4 		= 0x00;
    mtu34->tadcr_4		= 0x0000;
    mtu34->tadcora_4 	= 0xFFFF;
    mtu34->tadcorb_4 	= 0xFFFF;
    mtu34->tadcobra_4 	= 0xFFFF;
    mtu34->tadcobrb_4 	= 0xFFFF;
    //MTU_COMMON
    mtu34->toera 		= 0xC0;
    mtu34->tgcra 		= 0x80;
    mtu34->tocr1a 		= 0x00;
    mtu34->tocr2a 		= 0x00;
    mtu34->tcdra 		= 0xFFFF;
    mtu34->tddra 		= 0xFFFF;
    mtu34->tdera 		= 0x01;
    mtu34->tcntsa 		= 0x0000;
    mtu34->tcbra 		= 0xFFFF;
    mtu34->titcr1a 		= 0x00;
    mtu34->titcr2a 		= 0x00;
    mtu34->titcnt1a		= 0x00;
    mtu34->titcnt2a		= 0x00;
    mtu34->tbtera 		= 0x00;
    mtu34->tolbra 		= 0x00;
    mtu34->tmdr2a 		= 0x00;
    mtu34->titmra 		= 0x00;
    mtu34->twcra 		= 0x00;
    mtu34->tstra 		= 0x00;
    mtu34->tsyra 		= 0x00;
    mtu34->tcsystr 		= 0x00;
    mtu34->trwera 		= 0x01;
}

static void rmtu34_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RMTU34State *mtu34 = RMTU34(obj);

    memory_region_init_io(&mtu34->memory, OBJECT(mtu34), &mtu34_ops,
                          mtu34, "renesas-mtu34", 0x100);
    sysbus_init_mmio(d, &mtu34->memory);
    
    /* mtu3 */
    sysbus_init_irq(d, &mtu34->tgia_3);
    sysbus_init_irq(d, &mtu34->tgib_3);
    sysbus_init_irq(d, &mtu34->tgic_3);
    sysbus_init_irq(d, &mtu34->tgid_3);
    sysbus_init_irq(d, &mtu34->tciv_3);
    /* mtu4 */
    sysbus_init_irq(d, &mtu34->tgia_4);
    sysbus_init_irq(d, &mtu34->tgib_4);
    sysbus_init_irq(d, &mtu34->tgic_4);
    sysbus_init_irq(d, &mtu34->tgid_4);
    sysbus_init_irq(d, &mtu34->tciv_4);
    
    /* create timer_event */
	timer_init_us(&mtu34->timer3, QEMU_CLOCK_VIRTUAL, timer_event3, mtu34);
	timer_init_us(&mtu34->timer4, QEMU_CLOCK_VIRTUAL, timer_event4, mtu34);
    /* create Matlab timer event */
    gMatBLDCEvent = CreateEvent(NULL, FALSE, FALSE, "MatBLDCTimerInt");
    /* create Matlab sharedmemory */
    MatBLDCComMemory = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x100, "MatBLDCSharedMemory" );
    pMatBLDCComMemory = MapViewOfFile(MatBLDCComMemory, FILE_MAP_WRITE, 0, 0, 0);
    if(pMatBLDCComMemory == NULL){
        CloseHandle(pMatBLDCComMemory);
    }
    /* 先頭1byteを同期フラグとして使用 */
    *(uint8_t *)pMatBLDCComMemory = 0;
    *(uint8_t *)(pMatBLDCComMemory + TOERA_OFFSET) = 0xC0;
    
    /* create comp_PWM Thread */
    qemu_thread_create(&mtu34->thread, "bldc_thread", complementary_pwm,
                       mtu34, QEMU_THREAD_JOINABLE);
    
    gpRMTU34State = mtu34;
}

uint8_t *GetMatBLDCComMemory(void)
{
    return pMatBLDCComMemory;
}

void ReleaseMatBLDCCommemory(void)
{
	printf("ReleaseSyncWait \n");
	*(uint8_t *)(pMatBLDCComMemory + Mat_COM_OFFSET) = 0;
}

static const VMStateDescription vmstate_rmtu34 = {
    .name = "rx-mtu34",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rmtu34_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RMTU34State, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rmtu34_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rmtu34;
    dc->reset = rmtu34_reset;
    device_class_set_props(dc, rmtu34_properties);
}

static const TypeInfo rmtu34_info = {
    .name       = TYPE_RENESAS_MTU34,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RMTU34State),
    .instance_init = rmtu34_init,
    .class_init = rmtu34_class_init,
};

static void rmtu34_register_types(void)
{
    type_register_static(&rmtu34_info);
}

type_init(rmtu34_register_types)

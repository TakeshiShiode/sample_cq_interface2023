/*
 * Renesas s12ad
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
#include "hw/adc/renesas_s12ad.h"
#include "hw/timer/renesas_mtu34.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static RS12ADState *gpRS12ADState = NULL;

static uint64_t s12ad_read(void *opaque, hwaddr addr, unsigned size)
{
    RS12ADState *s12ad = opaque;
    uint64_t ret;
    uint8_t * pMatComMemory = GetMatBLDCComMemory();
    switch(addr){
        case R_ADCSR_0:		ret = s12ad->adscr_0 = 0x00;  		break;
        case R_ADANS_0:  	ret = s12ad->adans_0;  		break;
        case R_ADPG_0:   	ret = s12ad->adpg_0;  		break;
        case R_ADCER_0:   	ret = s12ad->adcer_0;  		break;
        case R_ADSTRGR_0: 	ret = s12ad->adstrgr_0;  	break;
        case R_ADRD_0:   	ret = s12ad->adrd_0;  		break;
        case R_ADDR0A_0:   	ret = s12ad->addr0a_0 =  *(uint16_t *)(pMatComMemory+S12AD0_ADDR0A_OFFSET);  	break;
        case R_ADDR1_0:   	ret = s12ad->addr1_0 =  *(uint16_t *)(pMatComMemory+S12AD0_ADDR1_OFFSET);  		break;
        case R_ADDR2_0:	  	ret = s12ad->addr2_0 =  *(uint16_t *)(pMatComMemory+S12AD0_ADDR2_OFFSET);  		break;
        case R_ADDR3_0:		ret = s12ad->addr3_0 =  *(uint16_t *)(pMatComMemory+S12AD0_ADDR3_OFFSET);		break;
        case R_ADDR0B_0:	ret = s12ad->addr0b_0;		break;
        case R_ADSSTR_0:	ret = s12ad->adsstr_0;		break;
                                         
        case R_ADCSR_1:		ret = s12ad->adscr_1 = 0x00;		break;
        case R_ADANS_1:		ret = s12ad->adans_1;		break;
        case R_ADPG_1:		ret = s12ad->adpg_1;		break;
        case R_ADCER_1:		ret = s12ad->adcer_1;		break;
        case R_ADSTRGR_1:	ret = s12ad->adstrgr_1;		break;
        case R_ADRD_1:		ret = s12ad->adrd_1;		break;
        case R_ADDR0A_1:	ret = s12ad->addr0a_1 =  *(uint16_t *)(pMatComMemory+S12AD1_ADDR0A_OFFSET);		break;
        case R_ADDR1_1:		ret = s12ad->addr1_1 =  *(uint16_t *)(pMatComMemory+S12AD1_ADDR1_OFFSET);		break;
        case R_ADDR2_1:		ret = s12ad->addr2_1 =  *(uint16_t *)(pMatComMemory+S12AD1_ADDR2_OFFSET);		break;
        case R_ADDR3_1:		ret = s12ad->addr3_1 =  *(uint16_t *)(pMatComMemory+S12AD1_ADDR3_OFFSET);		break;
        case R_ADDR0B_1:	ret = s12ad->addr0b_1;		break;
        case R_ADSSTR_1:	ret = s12ad->adsstr_1;		break;
                                         
        case R_ADCMPMD0:	ret = s12ad->adcmpmd0;		break;
        case R_ADCMPMD1:	ret = s12ad->adcmpmd1;		break;
        case R_ADCMPNR0:	ret = s12ad->adcmpnr0;		break;
        case R_ADCMPNR1:	ret = s12ad->adcmpnr1;		break;
        case R_ADCMPFR:		ret = s12ad->adcmpfr;		break;
        case R_ADCMPSEL:	ret = s12ad->adcmpsel;		break;
        
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


static void s12ad_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RS12ADState *s12ad = opaque;
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
        case R_ADCSR_0:		s12ad->adscr_0		= latval;  		break;
        case R_ADANS_0:  	s12ad->adans_0		= latval;  		break;
        case R_ADPG_0:   	s12ad->adpg_0		= latval;  		break;
        case R_ADCER_0:   	s12ad->adcer_0		= latval;  		break;
        case R_ADSTRGR_0: 	s12ad->adstrgr_0	= latval;  		break;
        case R_ADRD_0:   	s12ad->adrd_0		= latval;  		break;
        case R_ADDR0A_0:   	s12ad->addr0a_0		= latval;  		break;
        case R_ADDR1_0:   	s12ad->addr1_0		= latval;  		break;
        case R_ADDR2_0:	  	s12ad->addr2_0		= latval;  		break;
        case R_ADDR3_0:		s12ad->addr3_0		= latval;		break;
        case R_ADDR0B_0:	s12ad->addr0b_0		= latval;		break;
        case R_ADSSTR_0:	s12ad->adsstr_0		= latval;		break;
                                   
        case R_ADCSR_1:		s12ad->adscr_1		= latval;		break;
        case R_ADANS_1:		s12ad->adans_1		= latval;		break;
        case R_ADPG_1:		s12ad->adpg_1		= latval;		break;
        case R_ADCER_1:		s12ad->adcer_1		= latval;		break;
        case R_ADSTRGR_1:	s12ad->adstrgr_1	= latval;		break;
        case R_ADRD_1:		s12ad->adrd_1		= latval;		break;
        case R_ADDR0A_1:	s12ad->addr0a_1		= latval;		break;
        case R_ADDR1_1:		s12ad->addr1_1		= latval;		break;
        case R_ADDR2_1:		s12ad->addr2_1		= latval;		break;
        case R_ADDR3_1:		s12ad->addr3_1		= latval;		break;
        case R_ADDR0B_1:	s12ad->addr0b_1		= latval;		break;
        case R_ADSSTR_1:	s12ad->adsstr_1		= latval;		break;
                                   
        case R_ADCMPMD0:	s12ad->adcmpmd0		= latval;		break;
        case R_ADCMPMD1:	s12ad->adcmpmd1		= latval;		break;
        case R_ADCMPNR0:	s12ad->adcmpnr0		= latval;		break;
        case R_ADCMPNR1:	s12ad->adcmpnr1		= latval;		break;
        case R_ADCMPFR:		s12ad->adcmpfr		= latval;		break;
        case R_ADCMPSEL:	s12ad->adcmpsel		= latval;		break;
        
        default:
            break;
        }
}

static const MemoryRegionOps s12ad_ops = {
    .write = s12ad_write,
    .read  = s12ad_read,
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



static void rs12ad_reset(DeviceState *dev)
{
    RS12ADState *s12ad = RS12AD(dev);
	s12ad->adscr_0		= 0;
	s12ad->adans_0		= 0;
	s12ad->adpg_0		= 0;
	s12ad->adcer_0		= 0;
	s12ad->adstrgr_0	= 0;
	s12ad->adrd_0		= 0;
	s12ad->addr0a_0		= 0;
	s12ad->addr1_0		= 0;
	s12ad->addr2_0		= 0;
	s12ad->addr3_0		= 0;
	s12ad->addr0b_0		= 0;
	s12ad->adsstr_0		= 0x14;
	       
	s12ad->adscr_1		= 0;
	s12ad->adans_1		= 0;
	s12ad->adpg_1		= 0;
	s12ad->adcer_1		= 0;
	s12ad->adstrgr_1	= 0;
	s12ad->adrd_1		= 0;
	s12ad->addr0a_1		= 0;
	s12ad->addr1_1		= 0;
	s12ad->addr2_1		= 0;
	s12ad->addr3_1		= 0;
	s12ad->addr0b_1		= 0;
	s12ad->adsstr_1		= 0x14;
	       
	s12ad->adcmpmd0		= 0;
	s12ad->adcmpmd1		= 0;
	s12ad->adcmpnr0		= 0;
	s12ad->adcmpnr1		= 0;
	s12ad->adcmpfr		= 0;
	s12ad->adcmpsel		= 0;

}

static void rs12ad_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RS12ADState *s12ad = RS12AD(obj);

    memory_region_init_io(&s12ad->memory, OBJECT(s12ad), &s12ad_ops,
                          s12ad, "renesas-s12ad", 0x100);
    sysbus_init_mmio(d, &s12ad->memory);

    sysbus_init_irq(d, &s12ad->s12adi0);
    sysbus_init_irq(d, &s12ad->s12adi1);
    
	gpRS12ADState = s12ad;
}

RS12ADState * Get_RS12ADState(void)
{
	return gpRS12ADState;
}

static const VMStateDescription vmstate_rs12ad = {
    .name = "rx-s12ad",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rs12ad_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RS12ADState, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rs12ad_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rs12ad;
    dc->reset = rs12ad_reset;
    device_class_set_props(dc, rs12ad_properties);
}

static const TypeInfo rs12ad_info = {
    .name       = TYPE_RENESAS_S12AD,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RS12ADState),
    .instance_init = rs12ad_init,
    .class_init = rs12ad_class_init,
};

static void rs12ad_register_types(void)
{
    type_register_static(&rs12ad_info);
}

type_init(rs12ad_register_types)

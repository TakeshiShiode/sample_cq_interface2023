/*
 * Renesas system
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
#include "hw/misc/rx_system.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static RSYSTEMState *gpRSYSTEMState = NULL;

static uint64_t system_read(void *opaque, hwaddr addr, unsigned size)
{
    RSYSTEMState *system = opaque;
    uint64_t ret;
    switch(addr){
        case R_MDMONR:	ret = system->mdmonr;	break;
        case R_SYSCR1:	ret = system->syscr1;	break;
        case R_SBYCR:	ret = system->sbycr;	break;
        case R_MSTPCRA:	ret = system->mstpcra;	break;
        case R_MSTPCRB:	ret = system->mstpcrb;	break;
        case R_MSTPCRC:	ret = system->mstpcrc;	break;
        case R_SYSKCR:	ret = system->syskcr;	break;
        case R_SYSKCR3:	ret = system->syskcr3 = 0x0400;	break;
        case R_PLLCR:	ret = system->pllcr;	break;
        case R_PLLCR2:	ret = system->pllcr2;	break;
        case R_MEMWAIT:	ret = system->memwait;	break;
        case R_MOSCCR:	ret = system->mosccr;	break;
        case R_LOCOCR:	ret = system->lococr;	break;
        case R_ILOCOCR:	ret = system->ilococr;	break;
        case R_HOCOCR:	ret = system->hococr;	break;
        case R_HOCOCR2:	ret = system->hococr2;	break;
        case R_OSCOVFSR:ret = system->oscovfsr = 0x0D;	break;
        case R_OSTDCR:  ret = system->ostdcr;	break;
        case R_OSTDSR:	ret = system->ostdsr;	break;
        case R_OPCCR:	ret = system->opccr = 0x00;	break;
        case R_MOSCWTCR:ret = system->moscwtc;	break;
        case R_HOCOWTCR:ret = system->hocowtc;	break;
        case R_RSTSR2:	ret = system->rstsr2;	break;
        case R_SWRR:	ret = system->swrr;		break;
        case R_LVD1CR1:	ret = system->lvd1cr1;	break;
        case R_LVD1SR:	ret = system->lvd1sr;	break;
        case R_LVD2CR1:	ret = system->lvd2cr1;	break;
        case R_LVD2SR:	ret = system->lvd2sr;	break;
        case R_PRCR:	ret = system->prcr;		break;
        
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
    printf("ret = %lld\n",ret);
    return ret;
}


static void system_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RSYSTEMState *system = opaque;
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
        case R_MDMONR:	system->mdmonr	 = latval;  break;
        case R_SYSCR1:	system->syscr1	 = latval;  break;
        case R_SBYCR:	system->sbycr	 = latval;  break;
        case R_MSTPCRA:	system->mstpcra	 = latval;  break;
        case R_MSTPCRB:	system->mstpcrb	 = latval;  break;
        case R_MSTPCRC:	system->mstpcrc	 = latval;  break;
        case R_SYSKCR:	system->syskcr	 = latval;  break;
        case R_SYSKCR3:	system->syskcr3	 = latval;  break;
        case R_PLLCR:	system->pllcr	 = latval;  break;
        case R_PLLCR2:	system->pllcr2	 = latval;	break;
        case R_MEMWAIT:	system->memwait	 = latval;	break;
        case R_MOSCCR:	system->mosccr	 = latval;	break;
        case R_LOCOCR:	system->lococr	 = latval;	break;
        case R_ILOCOCR:	system->ilococr  = latval;	break;
        case R_HOCOCR:	system->hococr 	 = latval;	break;
        case R_HOCOCR2:	system->hococr2  = latval;	break;
        case R_OSCOVFSR:system->oscovfsr = latval;	break;
        case R_OSTDCR:  system->ostdcr 	 = latval;	break;
        case R_OSTDSR:	system->ostdsr 	 = latval;	break;
        case R_OPCCR:	system->opccr	 = latval;	break;
        case R_MOSCWTCR:system->moscwtc  = latval;	break;
        case R_HOCOWTCR:system->hocowtc  = latval;	break;
        case R_RSTSR2:	system->rstsr2 	 = latval;	break;
        case R_SWRR:	system->swrr 	 = latval;	break;
        case R_LVD1CR1:	system->lvd1cr1  = latval;	break;
        case R_LVD1SR:	system->lvd1sr 	 = latval;	break;
        case R_LVD2CR1:	system->lvd2cr1  = latval;	break;
        case R_LVD2SR:	system->lvd2sr 	 = latval;	break;
        case R_PRCR:	system->prcr 	 = latval;	break;
  
        default:
            break;
        }
}

static const MemoryRegionOps system_ops = {
    .write = system_write,
    .read  = system_read,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 4,
    },
    .valid = {
        .min_access_size = 1,
        .max_access_size = 4,
    },
};



static void rsystem_reset(DeviceState *dev)
{
    RSYSTEMState *system = RSYSTEM(dev);
	system->mdmonr	 = 0;
	system->syscr1	 = 0;
	system->sbycr	 = 0;
	system->mstpcra	 = 0;
	system->mstpcrb	 = 0;
	system-> mstpcrc = 0;
	system->syskcr	 = 0;
	system->syskcr3	 = 0x0400;
	system->pllcr	 = 0;
	system->pllcr2	 = 0;
	system->memwait	 = 0;
	system->mosccr	 = 0;
	system->lococr	 = 0;
	system->ilococr	 = 0;
	system->hococr	 = 0;
	system->hococr2	 = 0;
	system->oscovfsr = 0x0D;
	system->ostdcr	 = 0;
	system->ostdsr	 = 0;
	system->opccr	 = 0x00;
	system->moscwtc	 = 0;
	system->hocowtc	 = 0;
	system->rstsr2	 = 0;
	system->swrr	 = 0;
	system->lvd1cr1	 = 0;
	system->lvd1sr	 = 0;
	system->lvd2cr1	 = 0;
	system->lvd2sr	 = 0;
	system->prcr	 = 0;
}

static void rsystem_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RSYSTEMState *system = RSYSTEM(obj);

    memory_region_init_io(&system->memory, OBJECT(system), &system_ops,
                          system, "renesas-system", 0x400);
    sysbus_init_mmio(d, &system->memory);
    
	gpRSYSTEMState = system;
}

RSYSTEMState * Get_RSYSTEMState(void)
{
	return gpRSYSTEMState;
}

static const VMStateDescription vmstate_rsystem = {
    .name = "rx-system",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rsystem_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RSYSTEMState, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rsystem_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rsystem;
    dc->reset = rsystem_reset;
    device_class_set_props(dc, rsystem_properties);
}

static const TypeInfo rsystem_info = {
    .name       = TYPE_RENESAS_SYSTEM,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RSYSTEMState),
    .instance_init = rsystem_init,
    .class_init = rsystem_class_init,
};

static void rsystem_register_types(void)
{
    type_register_static(&rsystem_info);
}

type_init(rsystem_register_types)

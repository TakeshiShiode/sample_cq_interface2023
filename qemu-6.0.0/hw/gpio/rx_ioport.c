/*
 * Renesas ioport
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
#include "hw/gpio/rx_ioport.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"


static RIOPORTState *gpRIOPORTState = NULL;

static uint64_t ioport_read(void *opaque, hwaddr addr, unsigned size)
{
    RIOPORTState *ioport = opaque;
    uint64_t ret;
    switch(addr){
        case R_PORT1_DDR:		ret = ioport->port1.ddr;		break;
        case R_PORT1_DR:		ret = ioport->port1.dr;			break;
        case R_PORT1_PORT:		ret = ioport->port1.port;		break;
        case R_PORT1_ICR:		ret = ioport->port1.icr;		break;
        case R_PORT2_DDR:		ret = ioport->port2.ddr;		break;
        case R_PORT2_DR:		ret = ioport->port2.dr;			break;
        case R_PORT2_PORT:		ret = ioport->port2.port;		break;
        case R_PORT2_ICR:		ret = ioport->port2.icr;		break;
        case R_PORT3_DDR:		ret = ioport->port3.ddr;		break;
        case R_PORT3_DR:		ret = ioport->port3.dr;			break;
        case R_PORT3_PORT:		ret = ioport->port3.port;		break;
        case R_PORT3_ICR:		ret = ioport->port3.icr;		break;
        case R_PORT4_PORT:		ret = ioport->port4.port;		break;
        case R_PORT4_ICR:		ret = ioport->port4.icr;		break;
        case R_PORT5_PORT:		ret = ioport->port5.port;		break;
        case R_PORT5_ICR:		ret = ioport->port5.icr;		break;
        case R_PORT6_PORT:		ret = ioport->port6.port;		break;
        case R_PORT6_ICR:		ret = ioport->port6.icr;		break;
        case R_PORT7_DDR:		ret = ioport->port7.ddr;		break;
        case R_PORT7_DR:		ret = ioport->port7.dr;			break;
        case R_PORT7_PORT:		ret = ioport->port7.port;		break;
        case R_PORT7_ICR:		ret = ioport->port7.icr;		break;
        case R_PORT8_DDR:		ret = ioport->port8.ddr;		break;
        case R_PORT8_DR:		ret = ioport->port8.dr;			break;
        case R_PORT8_PORT:		ret = ioport->port8.port;		break;
        case R_PORT8_ICR:		ret = ioport->port8.icr;		break;
        case R_PORT9_DDR:		ret = ioport->port9.ddr;		break;
        case R_PORT9_DR:		ret = ioport->port9.dr;			break;
        case R_PORT9_PORT:		ret = ioport->port9.port;		break;
        case R_PORT9_ICR:  	ret = ioport->port9.icr;		break;
        case R_PORTA_DDR:  	ret = ioport->porta.ddr;		break;
        case R_PORTA_DR:   	ret = ioport->porta.dr;			break;
        case R_PORTA_PORT: 	ret = ioport->porta.port;		break;
        case R_PORTA_ICR:  	ret = ioport->porta.icr;		break;
        case R_PORTB_DDR:  	ret = ioport->portb.ddr;		break;
        case R_PORTB_DR:   	ret = ioport->portb.dr;			break;
        case R_PORTB_PORT: 	ret = ioport->portb.port;		break;
        case R_PORTB_ICR:  	ret = ioport->portb.icr;		break;
        case R_PORTD_DDR:  	ret = ioport->portd.ddr;		break;
        case R_PORTD_DR:   	ret = ioport->portd.dr;			break;
        case R_PORTD_PORT: 	ret = ioport->portd.port;		break;
        case R_PORTD_ICR:  	ret = ioport->portd.icr;		break;
        case R_PORTE_DDR: 		ret = ioport->porte.ddr;		break;
        case R_PORTE_DR:  		ret = ioport->porte.dr;			break;
        case R_PORTE_PORT:		ret = ioport->porte.port;		break;
        case R_PORTE_ICR: 		ret = ioport->porte.icr;		break;
        case R_PORTG_DDR: 		ret = ioport->portg.ddr;		break;
        case R_PORTG_DR:  		ret = ioport->portg.dr;			break;
        case R_PORTG_PORT:		ret = ioport->portg.port;		break;
        case R_PORTG_ICR: 		ret = ioport->portg.icr;		break;
        case R_IOPORT_PF8IRQ:	ret = ioport->ioport.pf8irq;	break;
        case R_IOPORT_PF9IRQ:	ret = ioport->ioport.p9irq;		break;
        case R_IOPORT_PFAADC:	ret = ioport->ioport.pfaadc;		break;
        case R_IOPORT_PFCMTU:	ret = ioport->ioport.pfcmtu;		break;
        case R_IOPORT_PFDGPT:	ret = ioport->ioport.pfdgpt;	break;
        case R_IOPORT_PFFSCI:	ret = ioport->ioport.pffsci;	break;
        case R_IOPORT_PFGSPI:	ret = ioport->ioport.pfgspi;	break;
        case R_IOPORT_PFHSPI:	ret = ioport->ioport.pfhspi;	break;
        case R_IOPORT_PFJCAN:	ret = ioport->ioport.pfjcan;	break;
        case R_IOPORT_PFKLIN:	ret = ioport->ioport.pfklin;	break;
        case R_IOPORT_PFMPOE:	ret = ioport->ioport.pfmpoe;	break;
        case R_IOPORT_PFNPOE:	ret = ioport->ioport.pfnpoe;	break;
        
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
//    printf("ret = %lld\n",ret);
    return ret;
}


static void ioport_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)
{
    RIOPORTState *ioport = opaque;
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
        case R_PORT1_DDR:		ioport->port1.ddr		 = latval;  	break;
        case R_PORT1_DR:		ioport->port1.dr		 = latval;  	break;
        case R_PORT1_PORT:		ioport->port1.port		 = latval;  	break;
        case R_PORT1_ICR:		ioport->port1.icr		 = latval;  	break;
        case R_PORT2_DDR:		ioport->port2.ddr		 = latval;  	break;
        case R_PORT2_DR:		ioport->port2.dr		 = latval;  	break;
        case R_PORT2_PORT:		ioport->port2.port		 = latval;  	break;
        case R_PORT2_ICR:		ioport->port2.icr		 = latval;  	break;
        case R_PORT3_DDR:		ioport->port3.ddr		 = latval;	break;
        case R_PORT3_DR:		ioport->port3.dr		 = latval;	break;
        case R_PORT3_PORT:		ioport->port3.port		 = latval;	break;
        case R_PORT3_ICR:		ioport->port3.icr		 = latval;	break;
        case R_PORT4_PORT:		ioport->port4.port		 = latval;	break;
        case R_PORT4_ICR:		ioport->port4.icr		 = latval;	break;
        case R_PORT5_PORT:		ioport->port5.port		 = latval;	break;
        case R_PORT5_ICR:		ioport->port5.icr		 = latval;	break;
        case R_PORT6_PORT:		ioport->port6.port		 = latval;	break;
        case R_PORT6_ICR:		ioport->port6.icr		 = latval;	break;
        case R_PORT7_DDR:		ioport->port7.ddr		 = latval;	break;
        case R_PORT7_DR:		ioport->port7.dr		 = latval;	break;
        case R_PORT7_PORT:		ioport->port7.port		 = latval;	break;
        case R_PORT7_ICR:		ioport->port7.icr		 = latval;	break;
        case R_PORT8_DDR:		ioport->port8.ddr		 = latval;	break;
        case R_PORT8_DR:		ioport->port8.dr		 = latval;	break;
        case R_PORT8_PORT:		ioport->port8.port		 = latval;	break;
        case R_PORT8_ICR:		ioport->port8.icr		 = latval;	break;
        case R_PORT9_DDR:		ioport->port9.ddr		 = latval;	break;
        case R_PORT9_DR:		ioport->port9.dr		 = latval;	break;
        case R_PORT9_PORT:		ioport->port9.port		 = latval;	break;
        case R_PORT9_ICR:  	ioport->port9.icr		 = latval;	break;
        case R_PORTA_DDR:  	ioport->porta.ddr		 = latval;	break;
        case R_PORTA_DR:   	ioport->porta.dr		 = latval;	break;
        case R_PORTA_PORT: 	ioport->porta.port		 = latval;	break;
        case R_PORTA_ICR:  	ioport->porta.icr		 = latval;	break;
        case R_PORTB_DDR:  	ioport->portb.ddr		 = latval;	break;
        case R_PORTB_DR:   	ioport->portb.dr		 = latval;	break;
        case R_PORTB_PORT: 	ioport->portb.port		 = latval;	break;
        case R_PORTB_ICR:  	ioport->portb.icr		 = latval;	break;
        case R_PORTD_DDR:  	ioport->portd.ddr		 = latval;	break;
        case R_PORTD_DR:   	ioport->portd.dr		 = latval;	break;
        case R_PORTD_PORT: 	ioport->portd.port		 = latval;	break;
        case R_PORTD_ICR:  	ioport->portd.icr		 = latval;	break;
        case R_PORTE_DDR: 		ioport->porte.ddr		 = latval;	break;
        case R_PORTE_DR:  		ioport->porte.dr		 = latval;	break;
        case R_PORTE_PORT:		ioport->porte.port		 = latval;	break;
        case R_PORTE_ICR: 		ioport->porte.icr		 = latval;	break;
        case R_PORTG_DDR: 		ioport->portg.ddr		 = latval;	break;
        case R_PORTG_DR:  		ioport->portg.dr		 = latval;	break;
        case R_PORTG_PORT:		ioport->portg.port		 = latval;	break;
        case R_PORTG_ICR: 		ioport->portg.icr		 = latval;	break;
        case R_IOPORT_PF8IRQ:	ioport->ioport.pf8irq	 = latval;	break;
        case R_IOPORT_PF9IRQ:	ioport->ioport.p9irq	 = latval;	break;
        case R_IOPORT_PFAADC:	ioport->ioport.pfaadc	 = latval;	break;
        case R_IOPORT_PFCMTU:	ioport->ioport.pfcmtu	 = latval;	break;
        case R_IOPORT_PFDGPT:	ioport->ioport.pfdgpt	 = latval;	break;
        case R_IOPORT_PFFSCI:	ioport->ioport.pffsci	 = latval;	break;
        case R_IOPORT_PFGSPI:	ioport->ioport.pfgspi	 = latval;	break;
        case R_IOPORT_PFHSPI:	ioport->ioport.pfhspi	 = latval;	break;
        case R_IOPORT_PFJCAN:	ioport->ioport.pfjcan	 = latval;	break;
        case R_IOPORT_PFKLIN:	ioport->ioport.pfklin	 = latval;	break;
        case R_IOPORT_PFMPOE:	ioport->ioport.pfmpoe	 = latval;	break;
        case R_IOPORT_PFNPOE:	ioport->ioport.pfnpoe	 = latval;	break;
  
        default:
            break;
        }
}

static const MemoryRegionOps ioport_ops = {
    .write = ioport_write,
    .read  = ioport_read,
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



static void rioport_reset(DeviceState *dev)
{
    RIOPORTState *ioport = RIOPORT(dev);

	ioport->port1.ddr		 = 0x00;
	ioport->port1.dr		 = 0x00;
	ioport->port1.port		 = 0x00;
	ioport->port1.icr		 = 0x00;
	ioport->port2.ddr		 = 0x00;
	ioport->port2.dr		 = 0x00;
	ioport->port2.port		 = 0x00;
	ioport->port2.icr		 = 0x00;
	ioport->port3.ddr		 = 0x00;
	ioport->port3.dr		 = 0x00;
	ioport->port3.port		 = 0x00;
	ioport->port3.icr		 = 0x00;
	ioport->port4.port		 = 0x00;
	ioport->port4.icr		 = 0x00;
	ioport->port5.port		 = 0x00;
	ioport->port5.icr		 = 0x00;
	ioport->port6.port		 = 0x00;
	ioport->port6.icr		 = 0x00;
	ioport->port7.ddr		 = 0x00;
	ioport->port7.dr		 = 0x00;
	ioport->port7.port		 = 0x00;
	ioport->port7.icr		 = 0x00;
	ioport->port8.ddr		 = 0x00;
	ioport->port8.dr		 = 0x00;
	ioport->port8.port		 = 0x00;
	ioport->port8.icr		 = 0x00;
	ioport->port9.ddr		 = 0x00;
	ioport->port9.dr		 = 0x00;
	ioport->port9.port		 = 0xFF;
	ioport->port9.icr		 = 0x00;
	ioport->porta.ddr		 = 0x00;
	ioport->porta.dr		 = 0x00;
	ioport->porta.port		 = 0x00;
	ioport->porta.icr		 = 0x00;
	ioport->portb.ddr		 = 0x00;
	ioport->portb.dr		 = 0x00;
	ioport->portb.port		 = 0x00;
	ioport->portb.icr		 = 0x00;
	ioport->portd.ddr		 = 0x00;
	ioport->portd.dr		 = 0x00;
	ioport->portd.port		 = 0x00;
	ioport->portd.icr		 = 0x00;
	ioport->porte.ddr		 = 0x00;
	ioport->porte.dr		 = 0x00;
	ioport->porte.port		 = 0x00;
	ioport->porte.icr		 = 0x00;
	ioport->portg.ddr		 = 0x00;
	ioport->portg.dr		 = 0x00;
	ioport->portg.port		 = 0x00;
	ioport->portg.icr		 = 0x00;
	ioport->ioport.pf8irq	 = 0x00;
	ioport->ioport.p9irq	 = 0x00;
	ioport->ioport.pfaadc	 = 0x00;
	ioport->ioport.pfcmtu	 = 0x00;
	ioport->ioport.pfdgpt	 = 0x00;
	ioport->ioport.pffsci	 = 0x00;
	ioport->ioport.pfgspi	 = 0x00;
	ioport->ioport.pfhspi	 = 0x00;
	ioport->ioport.pfjcan	 = 0x00;
	ioport->ioport.pfklin	 = 0x00;
	ioport->ioport.pfmpoe	 = 0x00;
	ioport->ioport.pfnpoe	 = 0x00;
	
}

static void rioport_init(Object *obj)
{
    SysBusDevice *d = SYS_BUS_DEVICE(obj);
    RIOPORTState *ioport = RIOPORT(obj);

    memory_region_init_io(&ioport->memory, OBJECT(ioport), &ioport_ops,
                          ioport, "renesas-ioport", 0x118);
    sysbus_init_mmio(d, &ioport->memory);
    
	gpRIOPORTState = ioport;
}

RIOPORTState * Get_RIOPORTState(void)
{
	return gpRIOPORTState;
}

static const VMStateDescription vmstate_rioport = {
    .name = "rx-ioport",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};

static Property rioport_properties[] = {
    DEFINE_PROP_UINT64("input-freq", RIOPORTState, input_freq, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rioport_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_rioport;
    dc->reset = rioport_reset;
    device_class_set_props(dc, rioport_properties);
}

static const TypeInfo rioport_info = {
    .name       = TYPE_RENESAS_IOPORT,
    .parent     = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RIOPORTState),
    .instance_init = rioport_init,
    .class_init = rioport_class_init,
};

static void rioport_register_types(void)
{
    type_register_static(&rioport_info);
}

type_init(rioport_register_types)

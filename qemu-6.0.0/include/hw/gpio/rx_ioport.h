/*
 * Renesas ioport Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_IOPORT_H
#define HW_RENESAS_IOPORT_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_IOPORT "renesas-ioport"
//#define RIOPORT(obj) OBJECT_CHECK(RIOPORTState, (obj), TYPE_RENESAS_IOPORT)
typedef struct RIOPORTState RIOPORTState;
DECLARE_INSTANCE_CHECKER(RIOPORTState, RIOPORT,
                         TYPE_RENESAS_IOPORT)

/* register offset address */
/* IOPORT*/
#define R_PORT1_DDR		(0x0001)
#define R_PORT1_DR		(0x0021)
#define R_PORT1_PORT	(0x0041)
#define R_PORT1_ICR		(0x0061)
#define R_PORT2_DDR		(0x0002)
#define R_PORT2_DR		(0x0022)
#define R_PORT2_PORT	(0x0042)
#define R_PORT2_ICR		(0x0062)
#define R_PORT3_DDR		(0x0003)
#define R_PORT3_DR		(0x0023)
#define R_PORT3_PORT	(0x0043)
#define R_PORT3_ICR		(0x0063)
#define R_PORT4_PORT	(0x0044)
#define R_PORT4_ICR		(0x0064)
#define R_PORT5_PORT	(0x0045)
#define R_PORT5_ICR		(0x0065)
#define R_PORT6_PORT	(0x0046)
#define R_PORT6_ICR		(0x0066)
#define R_PORT7_DDR		(0x0007)
#define R_PORT7_DR		(0x0027)
#define R_PORT7_PORT	(0x0047)
#define R_PORT7_ICR		(0x0067)
#define R_PORT8_DDR		(0x0008)
#define R_PORT8_DR		(0x0028)
#define R_PORT8_PORT	(0x0048)
#define R_PORT8_ICR		(0x0068)
#define R_PORT9_DDR		(0x0009)
#define R_PORT9_DR		(0x0029)
#define R_PORT9_PORT	(0x0049)
#define R_PORT9_ICR   	(0x0069)
#define R_PORTA_DDR   	(0x000A)
#define R_PORTA_DR    	(0x002A)
#define R_PORTA_PORT  	(0x004A)
#define R_PORTA_ICR   	(0x006A)
#define R_PORTB_DDR   	(0x000B)
#define R_PORTB_DR    	(0x002B)
#define R_PORTB_PORT  	(0x004B)
#define R_PORTB_ICR   	(0x006B)
#define R_PORTD_DDR   	(0x000D)
#define R_PORTD_DR    	(0x002D)
#define R_PORTD_PORT  	(0x004D)
#define R_PORTD_ICR   	(0x006D)
#define R_PORTE_DDR 	(0x000E)
#define R_PORTE_DR  	(0x002E)
#define R_PORTE_PORT	(0x004E)
#define R_PORTE_ICR 	(0x006E)
#define R_PORTG_DDR 	(0x0010)
#define R_PORTG_DR  	(0x0030)
#define R_PORTG_PORT	(0x0050)
#define R_PORTG_ICR 	(0x0070)
#define R_IOPORT_PF8IRQ	(0x0108)
#define R_IOPORT_PF9IRQ	(0x0109)
#define R_IOPORT_PFAADC	(0x010A)
#define R_IOPORT_PFCMTU	(0x010C)
#define R_IOPORT_PFDGPT	(0x010D)
#define R_IOPORT_PFFSCI	(0x010F)
#define R_IOPORT_PFGSPI	(0x0110)
#define R_IOPORT_PFHSPI	(0x0111)
#define R_IOPORT_PFJCAN	(0x0113)
#define R_IOPORT_PFKLIN	(0x0114)
#define R_IOPORT_PFMPOE	(0x0116)
#define R_IOPORT_PFNPOE	(0x0117)

typedef struct Port_ST{
	uint8_t ddr;
	uint8_t dr;
	uint8_t port;
	uint8_t icr;
}Port_ST;

typedef struct IOPort_ST{
	uint8_t pf8irq;
	uint8_t p9irq;
	uint8_t pfaadc;
	uint8_t pfcmtu;
	uint8_t pfdgpt;
	uint8_t pffsci;
	uint8_t pfgspi;
	uint8_t pfhspi;
	uint8_t pfjcan;
	uint8_t pfklin;
	uint8_t pfmpoe;
	uint8_t pfnpoe;
}IOPort_ST;

typedef struct RIOPORTState {
    SysBusDevice parent_obj;

    uint64_t input_freq;
    MemoryRegion memory;

   	Port_ST port1;
	Port_ST port2;
	Port_ST port3;
	Port_ST port4;
	Port_ST port5;
	Port_ST port6;
	Port_ST port7;
	Port_ST port8;
	Port_ST port9;
	Port_ST porta;
	Port_ST portb;
	Port_ST portd;
	Port_ST porte;
	Port_ST portg;
	IOPort_ST ioport;
	
} RIOPORTState;


RIOPORTState * Get_RIOPORTState(void);
#endif

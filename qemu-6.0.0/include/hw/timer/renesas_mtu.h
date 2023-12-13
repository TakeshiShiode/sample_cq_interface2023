/*
 * Renesas mtu timer Object
 *
 * Copyright (c) 2022 Takeshi Shiode
 *
 * This code is licensed under the GPL version 2 or later.
 *
 */

#ifndef HW_RENESAS_MTU_H
#define HW_RENESAS_MTU_H

#include "hw/sysbus.h"

#define TYPE_RENESAS_MTU "renesas-mtu"
//#define RMTU(obj) OBJECT_CHECK(RMTUState, (obj), TYPE_RENESAS_MTU)
typedef struct RMTUState RMTUState;
DECLARE_INSTANCE_CHECKER(RMTUState, RMTU,
                         TYPE_RENESAS_MTU)
/* register offset address */
#define R_TCR   (0x00)
#define R_TMDR1 (0x01)
#define R_TIORH (0x02)
#define R_TIORL (0x03)
#define R_TIER  (0x04)
#define R_TSR   (0x05)
#define R_TCNT  (0x06)
#define R_TGRA  (0x08)
#define R_TGRB  (0x0A)
#define R_TGRC  (0x0C)
#define R_TGRD  (0x0E)
#define R_TGRE  (0x20)
#define R_TGRF  (0x22)
#define R_TIER2 (0x24)
#define R_TSR2  (0x25)
#define R_TBTM  (0x26)


typedef struct RMTUState {
    SysBusDevice parent_obj;

	uint64_t input_freq;
    MemoryRegion memory;

   	uint8_t		toera;      
   	uint8_t		toerb;      
   	uint8_t		tgcra;      
   	uint8_t		tocr1a;     
   	uint8_t		tocr1b;     
   	uint8_t		tocr2a;     
   	uint8_t		tocr2b;     
   	uint16_t 	tcdra;      
   	uint16_t 	tcdrb;      
   	uint16_t 	tddra;      
   	uint8_t 	tdera;      
   	uint16_t 	tddrb;      
   	uint8_t 	tderb;      
   	uint16_t 	tcntsa;     
   	uint16_t 	tcntsb;     
   	uint16_t 	tcbra;      
   	uint16_t 	tcbrb;      
   	uint8_t 	titcr1a;    
   	uint8_t 	titcr1b;    
   	uint8_t 	titcr2a;    
   	uint8_t 	titcr2b;    
   	uint8_t 	titcnt1a;   
   	uint8_t 	titcnt1b;   
   	uint8_t 	titcnt2a;   
   	uint8_t 	titcnt2b;   
   	uint8_t 	tbtera;     
   	uint8_t 	tbterb;     
   	uint8_t 	tolbra;     
   	uint8_t 	tolbrb;     
   	uint8_t 	tmdr2a;     
   	uint8_t 	tmdr2b;     
   	uint8_t 	titmra;             
   	uint8_t 	titmrb;     
   	uint8_t 	twcra;         
   	uint8_t 	twcrb;      
   	uint8_t 	tstra;      
   	uint8_t 	tstrb;      
   	uint8_t 	tsyra;      
   	uint8_t 	tsyrb;      
   	uint8_t 	tcsystr;    
   	uint8_t 	trwera;     
   	uint8_t 	trwerb;     
} RMTUState;

#endif

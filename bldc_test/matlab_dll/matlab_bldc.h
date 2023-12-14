#pragma once
#define MTU1_TCNT (0x02)
#define MTU2_TCNT (0x04)
#define MTU3_TGRD (0x06)
#define MTU4_TGRC (0x08)
#define MTU4_TGRD (0x0A)
#define S12AD0_ADDR0A_OFFSET (0x0C)
#define S12AD0_ADDR1_OFFSET (0x0E)
#define S12AD0_ADDR2_OFFSET (0x10)
#define S12AD0_ADDR3_OFFSET (0x12)
#define S12AD1_ADDR0A_OFFSET (0x04)
#define S12AD1_ADDR1_OFFSET (0x16)
#define S12AD1_ADDR2_OFFSET (0x18)
#define S12AD1_ADDR3_OFFSET (0x1A)
#define MTU_TOERA_OFFSET (0x1C)
#define MAT_COM_OFFSET (0x1D)
#define TCNT1_CLEAR_OFFSET (0x1E)

#define PWM_PERIOD (1296)
void SetBLDCMemoryAddress(BYTE* Adr);
void MatlabReleaseInt(void);
void MatlabInitBLDCData(void);


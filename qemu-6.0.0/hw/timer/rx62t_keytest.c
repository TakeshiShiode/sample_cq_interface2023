#include <stdio.h>
#include <ctype.h>
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
#include "hw/timer/rx62t_keytest.h"
#include "migration/vmstate.h"
#include "qemu/error-report.h"

#include "hw/gpio/rx_ioport.h"
#include "hw/timer/renesas_mtu34.h"

//----------------------------------------------------------------------------------
// Function templates.
    static uint64_t KeyTestRead(void *opaque, hwaddr offset, unsigned size);
    static void KeyTestWrite(void *opaque, hwaddr offset, uint64_t value, unsigned size);
    static void KeyTest_tick(void *opaque);
//***********************************************************************************

//***********************************************************************************
//! @name IO operations
//@{
//-----------------------------------------------------------------------------
//! IO read operation called by the QEMU. This function fowards to QSimCom.
static uint64_t KeyTestRead(void *opaque, hwaddr offset, unsigned size){
    RKEYTESTState *s = RKEYTEST(opaque);
    uint64_t ret_regster_value = 0;
    //---- Calulate the address where the guest CPU accessed.
    switch(offset){
        //-----------基本設定------------- 
        case TESTSTATE: ret_regster_value = s->teststate; break;
        case TESTKEY: ret_regster_value = s->testkey; break;
        default: ret_regster_value = 0; break;
    }

    return ret_regster_value;
}

//-----------------------------------------------------------------------------
//! IO write operation called by the QEMU This function fowards to QSimCom.
static void KeyTestWrite(void *opaque, hwaddr offset, uint64_t value, unsigned size){
    RKEYTESTState *s = RKEYTEST(opaque);
    //---- 32bit化-----.
    uint32_t    value32 = (uint32_t)value;
    char* fmt = NULL;
    FILE* fp = NULL;
     switch(offset){
        //-----------基本設定-------------  
        case TESTSTATE: s->teststate = (uint8_t)value32; break;
        case TESTKEY:  s->testkey = (uint8_t)value32; break;
        case SENTENCE_BUF0:   s->sentence[0] = (char)value32; break;
        case SENTENCE_BUF1:   s->sentence[1] = (char)value32; break;
        case SENTENCE_BUF2:   s->sentence[2] = (char)value32; break;
        case SENTENCE_BUF3:   s->sentence[3] = (char)value32; break;
        case SENTENCE_BUF4:   s->sentence[4] = (char)value32; break;
        case SENTENCE_BUF5:   s->sentence[5] = (char)value32; break;
        case SENTENCE_BUF6:   s->sentence[6] = (char)value32; break;
        case SENTENCE_BUF7:   s->sentence[7] = (char)value32; break;
        case SENTENCE_BUF8:   s->sentence[8] = (char)value32; break;
        case SENTENCE_BUF9:   s->sentence[9] = (char)value32; break;
        case SENTENCE_BUF10:  s->sentence[10] = (char)value32; break;
        case SENTENCE_BUF11:  s->sentence[11] = (char)value32; break;
        case SENTENCE_BUF12:  s->sentence[12] = (char)value32; break;
        case SENTENCE_BUF13:  s->sentence[13] = (char)value32; break;
        case SENTENCE_BUF14:  s->sentence[14] = (char)value32; break;
        case SENTENCE_BUF15:  s->sentence[15] = (char)value32; break;
        case SENTENCE_BUF16:  s->sentence[16] = (char)value32; break;
        case SENTENCE_BUF17:  s->sentence[17] = (char)value32; break;
        case SENTENCE_BUF18:  s->sentence[18] = (char)value32; break;
        case SENTENCE_BUF19:  s->sentence[19] = (char)value32; break;
        case SENTENCE_BUF20:  s->sentence[20] = (char)value32; break;
        case SENTENCE_BUF21:  s->sentence[21] = (char)value32; break;
        case SENTENCE_BUF22:  s->sentence[22] = (char)value32; break;
        case SENTENCE_BUF23:  s->sentence[23] = (char)value32; break;
        case SENTENCE_BUF24:  s->sentence[24] = (char)value32; break;
        case SENTENCE_BUF25:  s->sentence[25] = (char)value32; break;
        case SENTENCE_BUF26:  s->sentence[26] = (char)value32; break;
        case SENTENCE_BUF27:  s->sentence[27] = (char)value32; break;
        case SENTENCE_BUF28:  s->sentence[28] = (char)value32; break;
        case SENTENCE_BUF29:  s->sentence[29] = (char)value32; break;
        case SENTENCE_BUF30:  s->sentence[30] = (char)value32; break;
        case SENTENCE_BUF31:  s->sentence[31] = (char)value32; break;
        case SENTENCE_BUF32:  s->sentence[32] = (char)value32; break;
        case SENTENCE_BUF33:  s->sentence[33] = (char)value32; break;
        case SENTENCE_BUF34:  s->sentence[34] = (char)value32; break;
        case SENTENCE_BUF35:  s->sentence[35] = (char)value32; break;
        case SENTENCE_BUF36:  s->sentence[36] = (char)value32; break;
        case SENTENCE_BUF37:  s->sentence[37] = (char)value32; break;
        case SENTENCE_BUF38:  s->sentence[38] = (char)value32; break;
        case SENTENCE_BUF39:  s->sentence[39] = (char)value32; break;
        case SENTENCE_BUF40:  s->sentence[40] = (char)value32; break;
        case SENTENCE_BUF41:  s->sentence[41] = (char)value32; break;
        case SENTENCE_BUF42:  s->sentence[42] = (char)value32; break;
        case SENTENCE_BUF43:  s->sentence[43] = (char)value32; break;
        case SENTENCE_BUF44:  s->sentence[44] = (char)value32; break;
        case SENTENCE_BUF45:  s->sentence[45] = (char)value32; break;
        case SENTENCE_BUF46:  s->sentence[46] = (char)value32; break;
        case SENTENCE_BUF47:  s->sentence[47] = (char)value32; break;
        case SENTENCE_BUF48:  s->sentence[48] = (char)value32; break;
        case SENTENCE_BUF49:  s->sentence[49] = (char)value32; break;
        case SENTENCE_BUF50:  s->sentence[50] = (char)value32; break;
        case SENTENCE_BUF51:  s->sentence[51] = (char)value32; break;
        case SENTENCE_BUF52:  s->sentence[52] = (char)value32; break;
        case SENTENCE_BUF53:  s->sentence[53] = (char)value32; break;
        case SENTENCE_BUF54:  s->sentence[54] = (char)value32; break;
        case SENTENCE_BUF55:  s->sentence[55] = (char)value32; break;
        case SENTENCE_BUF56:  s->sentence[56] = (char)value32; break;
        case SENTENCE_BUF57:  s->sentence[57] = (char)value32; break;
        case SENTENCE_BUF58:  s->sentence[58] = (char)value32; break;
        case SENTENCE_BUF59:  s->sentence[59] = (char)value32; break;
        case SENTENCE_BUF60:  s->sentence[60] = (char)value32; break;
        case SENTENCE_BUF61:  s->sentence[61] = (char)value32; break;
        case SENTENCE_BUF62:  s->sentence[62] = (char)value32; break;
        case SENTENCE_BUF63:  s->sentence[63] = (char)value32; break;
        case SENTENCE_BUF64:  s->sentence[64] = (char)value32; break;
        case SENTENCE_BUF65:  s->sentence[65] = (char)value32; break;
        case SENTENCE_BUF66:  s->sentence[66] = (char)value32; break;
        case SENTENCE_BUF67:  s->sentence[67] = (char)value32; break;
        case SENTENCE_BUF68:  s->sentence[68] = (char)value32; break;
        case SENTENCE_BUF69:  s->sentence[69] = (char)value32; break;
        case SENTENCE_BUF70:  s->sentence[70] = (char)value32; break;
        case SENTENCE_BUF71:  s->sentence[71] = (char)value32; break;
        case SENTENCE_BUF72:  s->sentence[72] = (char)value32; break;
        case SENTENCE_BUF73:  s->sentence[73] = (char)value32; break;
        case SENTENCE_BUF74:  s->sentence[74] = (char)value32; break;
        case SENTENCE_BUF75:  s->sentence[75] = (char)value32; break;
        case SENTENCE_BUF76:  s->sentence[76] = (char)value32; break;
        case SENTENCE_BUF77:  s->sentence[77] = (char)value32; break;
        case SENTENCE_BUF78:  s->sentence[78] = (char)value32; break;
        case SENTENCE_BUF79:  s->sentence[79] = (char)value32; break;
        case SENTENCE_BUF80:  s->sentence[80] = (char)value32; break;
        case SENTENCE_BUF81:  s->sentence[81] = (char)value32; break;
        case SENTENCE_BUF82:  s->sentence[82] = (char)value32; break;
        case SENTENCE_BUF83:  s->sentence[83] = (char)value32; break;
        case SENTENCE_BUF84:  s->sentence[84] = (char)value32; break;
        case SENTENCE_BUF85:  s->sentence[85] = (char)value32; break;
        case SENTENCE_BUF86:  s->sentence[86] = (char)value32; break;
        case SENTENCE_BUF87:  s->sentence[87] = (char)value32; break;
        case SENTENCE_BUF88:  s->sentence[88] = (char)value32; break;
        case SENTENCE_BUF89:  s->sentence[89] = (char)value32; break;
        case SENTENCE_BUF90:  s->sentence[90] = (char)value32; break;
        case SENTENCE_BUF91:  s->sentence[91] = (char)value32; break;
        case SENTENCE_BUF92:  s->sentence[92] = (char)value32; break;
        case SENTENCE_BUF93:  s->sentence[93] = (char)value32; break;
        case SENTENCE_BUF94:  s->sentence[94] = (char)value32; break;
        case SENTENCE_BUF95:  s->sentence[95] = (char)value32; break;
        case SENTENCE_BUF96:  s->sentence[96] = (char)value32; break;
        case SENTENCE_BUF97:  s->sentence[97] = (char)value32; break;
        case SENTENCE_BUF98:  s->sentence[98] = (char)value32; break;
        case SENTENCE_BUF99:  s->sentence[99] = (char)value32; break;
        case SENTENCE_BUF100: s->sentence[100] = (char)value32; break;
        case SENTENCE_BUF101: s->sentence[101] = (char)value32; break;
        case SENTENCE_BUF102: s->sentence[102] = (char)value32; break;
        case SENTENCE_BUF103: s->sentence[103] = (char)value32; break;
        case SENTENCE_BUF104: s->sentence[104] = (char)value32; break;
        case SENTENCE_BUF105: s->sentence[105] = (char)value32; break;
        case SENTENCE_BUF106: s->sentence[106] = (char)value32; break;
        case SENTENCE_BUF107: s->sentence[107] = (char)value32; break;
        case SENTENCE_BUF108: s->sentence[108] = (char)value32; break;
        case SENTENCE_BUF109: s->sentence[109] = (char)value32; break;
        case SENTENCE_BUF110: s->sentence[110] = (char)value32; break;
        case SENTENCE_BUF111: s->sentence[111] = (char)value32; break; 
        case SENTENCE_BUF112: s->sentence[112] = (char)value32; break; 
        case SENTENCE_BUF113: s->sentence[113] = (char)value32; break; 
        case SENTENCE_BUF114: s->sentence[114] = (char)value32; break; 
        case SENTENCE_BUF115: s->sentence[115] = (char)value32; break; 
        case SENTENCE_BUF116: s->sentence[116] = (char)value32; break; 
        case SENTENCE_BUF117: s->sentence[117] = (char)value32; break; 
        case SENTENCE_BUF118: s->sentence[118] = (char)value32; break; 
        case SENTENCE_BUF119: s->sentence[119] = (char)value32; break; 
        case SENTENCE_BUF120: s->sentence[120] = (char)value32; break; 
        case SENTENCE_BUF121: s->sentence[121] = (char)value32; break; 
        case SENTENCE_BUF122: s->sentence[122] = (char)value32; break; 
        case SENTENCE_BUF123: s->sentence[123] = (char)value32; break; 
        case SENTENCE_BUF124: s->sentence[124] = (char)value32; break; 
        case SENTENCE_BUF125: s->sentence[125] = (char)value32; break; 
        case SENTENCE_BUF126: s->sentence[126] = (char)value32; break;
        case SENTENCE_BUF127: s->sentence[127] = (char)value32; break;
        case SENTENCE_BUF128: s->sentence[128] = (char)value32; break;
        case SENTENCE_BUF129: s->sentence[129] = (char)value32; break;
        case SENTENCE_BUF130: s->sentence[130] = (char)value32; break;
        case SENTENCE_BUF131: s->sentence[131] = (char)value32; break;
        case SENTENCE_BUF132: s->sentence[132] = (char)value32; break;
        case SENTENCE_BUF133: s->sentence[133] = (char)value32; break;
        case SENTENCE_BUF134: s->sentence[134] = (char)value32; break;
        case SENTENCE_BUF135: s->sentence[135] = (char)value32; break;
        case SENTENCE_BUF136: s->sentence[136] = (char)value32; break;
        case SENTENCE_BUF137: s->sentence[137] = (char)value32; break;
        case SENTENCE_BUF138: s->sentence[138] = (char)value32; break;
        case SENTENCE_BUF139: s->sentence[139] = (char)value32; break;
        case SENTENCE_BUF140: s->sentence[140] = (char)value32; break;
        case SENTENCE_BUF141: s->sentence[141] = (char)value32; break;
        case SENTENCE_BUF142: s->sentence[142] = (char)value32; break;
        case SENTENCE_BUF143: s->sentence[143] = (char)value32; break;
        case SENTENCE_BUF144: s->sentence[144] = (char)value32; break;
        case SENTENCE_BUF145: s->sentence[145] = (char)value32; break;
        case SENTENCE_BUF146: s->sentence[146] = (char)value32; break;
        case SENTENCE_BUF147: s->sentence[147] = (char)value32; break;
        case SENTENCE_BUF148: s->sentence[148] = (char)value32; break;
        case SENTENCE_BUF149: s->sentence[149] = (char)value32; break;
        case SENTENCE_BUF150: s->sentence[150] = (char)value32; break;
        case SENTENCE_BUF151: s->sentence[151] = (char)value32; break;
        case SENTENCE_BUF152: s->sentence[152] = (char)value32; break;
        case SENTENCE_BUF153: s->sentence[153] = (char)value32; break;
        case SENTENCE_BUF154: s->sentence[154] = (char)value32; break;
        case SENTENCE_BUF155: s->sentence[155] = (char)value32; break;
        case SENTENCE_BUF156: s->sentence[156] = (char)value32; break;
        case SENTENCE_BUF157: s->sentence[157] = (char)value32; break;
        case SENTENCE_BUF158: s->sentence[158] = (char)value32; break;
        case SENTENCE_BUF159: s->sentence[159] = (char)value32; break;
        case SENTENCE_BUF160: s->sentence[160] = (char)value32; break;
        case SENTENCE_BUF161: s->sentence[161] = (char)value32; break;
        case SENTENCE_BUF162: s->sentence[162] = (char)value32; break;
        case SENTENCE_BUF163: s->sentence[163] = (char)value32; break;
        case SENTENCE_BUF164: s->sentence[164] = (char)value32; break;
        case SENTENCE_BUF165: s->sentence[165] = (char)value32; break;
        case SENTENCE_BUF166: s->sentence[166] = (char)value32; break;
        case SENTENCE_BUF167: s->sentence[167] = (char)value32; break;
        case SENTENCE_BUF168: s->sentence[168] = (char)value32; break;
        case SENTENCE_BUF169: s->sentence[169] = (char)value32; break;
        case SENTENCE_BUF170: s->sentence[170] = (char)value32; break;
        case SENTENCE_BUF171: s->sentence[171] = (char)value32; break;
        case SENTENCE_BUF172: s->sentence[172] = (char)value32; break;
        case SENTENCE_BUF173: s->sentence[173] = (char)value32; break;
        case SENTENCE_BUF174: s->sentence[174] = (char)value32; break;
        case SENTENCE_BUF175: s->sentence[175] = (char)value32; break;
        case SENTENCE_BUF176: s->sentence[176] = (char)value32; break;
        case SENTENCE_BUF177: s->sentence[177] = (char)value32; break;
        case SENTENCE_BUF178: s->sentence[178] = (char)value32; break;
        case SENTENCE_BUF179: s->sentence[179] = (char)value32; break;
        case SENTENCE_BUF180: s->sentence[180] = (char)value32; break;
        case SENTENCE_BUF181: s->sentence[181] = (char)value32; break;
        case SENTENCE_BUF182: s->sentence[182] = (char)value32; break;
        case SENTENCE_BUF183: s->sentence[183] = (char)value32; break;
        case SENTENCE_BUF184: s->sentence[184] = (char)value32; break;
        case SENTENCE_BUF185: s->sentence[185] = (char)value32; break;
        case SENTENCE_BUF186: s->sentence[186] = (char)value32; break;
        case SENTENCE_BUF187: s->sentence[187] = (char)value32; break;
        case SENTENCE_BUF188: s->sentence[188] = (char)value32; break;
        case SENTENCE_BUF189: s->sentence[189] = (char)value32; break;
        case SENTENCE_BUF190: s->sentence[190] = (char)value32; break;
        case SENTENCE_BUF191: s->sentence[191] = (char)value32; break;
        case SENTENCE_BUF192: s->sentence[192] = (char)value32; break;
        case SENTENCE_BUF193: s->sentence[193] = (char)value32; break;
        case SENTENCE_BUF194: s->sentence[194] = (char)value32; break;
        case SENTENCE_BUF195: s->sentence[195] = (char)value32; break;
        case SENTENCE_BUF196: s->sentence[196] = (char)value32; break;
        case SENTENCE_BUF197: s->sentence[197] = (char)value32; break;
        case SENTENCE_BUF198: s->sentence[198] = (char)value32; break;
        case SENTENCE_BUF199: s->sentence[199] = (char)value32; break;
        case SENTENCE_BUF200: s->sentence[200] = (char)value32; break;
        case SENTENCE_BUF201: s->sentence[201] = (char)value32; break;
        case SENTENCE_BUF202: s->sentence[202] = (char)value32; break;
        case SENTENCE_BUF203: s->sentence[203] = (char)value32; break;
        case SENTENCE_BUF204: s->sentence[204] = (char)value32; break;
        case SENTENCE_BUF205: s->sentence[205] = (char)value32; break;
        case SENTENCE_BUF206: s->sentence[206] = (char)value32; break;
        case SENTENCE_BUF207: s->sentence[207] = (char)value32; break;
        case SENTENCE_BUF208: s->sentence[208] = (char)value32; break;
        case SENTENCE_BUF209: s->sentence[209] = (char)value32; break;
        case SENTENCE_BUF210: s->sentence[210] = (char)value32; break;
        case SENTENCE_BUF211: s->sentence[211] = (char)value32; break;
        case SENTENCE_BUF212: s->sentence[212] = (char)value32; break;
        case SENTENCE_BUF213: s->sentence[213] = (char)value32; break;
        case SENTENCE_BUF214: s->sentence[214] = (char)value32; break;
        case SENTENCE_BUF215: s->sentence[215] = (char)value32; break;
        case SENTENCE_BUF216: s->sentence[216] = (char)value32; break;
        case SENTENCE_BUF217: s->sentence[217] = (char)value32; break;
        case SENTENCE_BUF218: s->sentence[218] = (char)value32; break;
        case SENTENCE_BUF219: s->sentence[219] = (char)value32; break;
        case SENTENCE_BUF220: s->sentence[220] = (char)value32; break;
        case SENTENCE_BUF221: s->sentence[221] = (char)value32; break;
        case SENTENCE_BUF222: s->sentence[222] = (char)value32; break;
        case SENTENCE_BUF223: s->sentence[223] = (char)value32; break;
        case SENTENCE_BUF224: s->sentence[224] = (char)value32; break;
        case SENTENCE_BUF225: s->sentence[225] = (char)value32; break;
        case SENTENCE_BUF226: s->sentence[226] = (char)value32; break;
        case SENTENCE_BUF227: s->sentence[227] = (char)value32; break;
        case SENTENCE_BUF228: s->sentence[228] = (char)value32; break;
        case SENTENCE_BUF229: s->sentence[229] = (char)value32; break;
        case SENTENCE_BUF230: s->sentence[230] = (char)value32; break;
        case SENTENCE_BUF231: s->sentence[231] = (char)value32; break;
        case SENTENCE_BUF232: s->sentence[232] = (char)value32; break;
        case SENTENCE_BUF233: s->sentence[233] = (char)value32; break;
        case SENTENCE_BUF234: s->sentence[234] = (char)value32; break;
        case SENTENCE_BUF235: s->sentence[235] = (char)value32; break;
        case SENTENCE_BUF236: s->sentence[236] = (char)value32; break;
        case SENTENCE_BUF237: s->sentence[237] = (char)value32; break;
        case SENTENCE_BUF238: s->sentence[238] = (char)value32; break;
        case SENTENCE_BUF239: s->sentence[239] = (char)value32; break;
        case SENTENCE_BUF240: s->sentence[240] = (char)value32; break;
        case SENTENCE_BUF241: s->sentence[241] = (char)value32; break;
        case SENTENCE_BUF242: s->sentence[242] = (char)value32; break;
        case SENTENCE_BUF243: s->sentence[243] = (char)value32; break;
        case SENTENCE_BUF244: s->sentence[244] = (char)value32; break;
        case SENTENCE_BUF245: s->sentence[245] = (char)value32; break;
        case SENTENCE_BUF246: s->sentence[246] = (char)value32; break;
        case SENTENCE_BUF247: s->sentence[247] = (char)value32; break;
        case SENTENCE_BUF248: s->sentence[248] = (char)value32; break;
        case SENTENCE_BUF249: s->sentence[249] = (char)value32; break;
        case SENTENCE_BUF250: s->sentence[250] = (char)value32; break;
        case SENTENCE_BUF251: s->sentence[251] = (char)value32; break;
        case SENTENCE_BUF252: s->sentence[252] = (char)value32; break;
        case SENTENCE_BUF253: s->sentence[253] = (char)value32; break;
        case SENTENCE_BUF254: s->sentence[254] = (char)value32; break;
        case SENTENCE_BUF255: s->sentence[255] = (char)value32; break;
        case FLUSH_ON: 
        	fp = fopen("tprint_log.txt","a");
        	fmt = (char *)(s->sentence);
        	printf("%s",fmt);
        	if(fp == NULL){
				return;
			}
			fprintf(fp,"%s", fmt);
			fclose(fp);
        	break;
		default:
			break;
    }
}

//@}

//***********************************************************************************
//! @name TIMCU_tick
//@{
//-----------------------------------------------------------------------------
//
static void KeyTest_tick(void *opaque)
{
    RKEYTESTState *s = RKEYTEST(opaque);
	uint8_t * pMatComMemory = GetMatBLDCComMemory();
	signed short* pKEYTEST_AD1_ADDR2 = (signed short*)(pMatComMemory + KEYTEST_AD1_ADDR2_OFFSET);
//	if(s->teststate == 0){
	    if(GetKeyState(VK_SHIFT) & 0x8000){
			if(GetKeyState('A') & 0x8000) s->teststate = 1, s->testkey = 'A', printf("Key = %d\n",s->testkey);
			if(GetKeyState('B') & 0x8000) s->teststate = 1, s->testkey = 'B', printf("Key = %d\n",s->testkey);
			if(GetKeyState('C') & 0x8000) s->teststate = 1, s->testkey = 'C', printf("Key = %d\n",s->testkey);
			if(GetKeyState('D') & 0x8000) s->teststate = 1, s->testkey = 'D', printf("Key = %d\n",s->testkey);
			if(GetKeyState('E') & 0x8000) s->teststate = 1, s->testkey = 'E', printf("Key = %d\n",s->testkey);
			if(GetKeyState('F') & 0x8000) s->teststate = 1, s->testkey = 'F', printf("Key = %d\n",s->testkey);
			if(GetKeyState('G') & 0x8000) s->teststate = 1, s->testkey = 'G', printf("Key = %d\n",s->testkey);
			if(GetKeyState('H') & 0x8000) s->teststate = 1, s->testkey = 'H', printf("Key = %d\n",s->testkey);
			if(GetKeyState('I') & 0x8000) s->teststate = 1, s->testkey = 'I', printf("Key = %d\n",s->testkey);
			if(GetKeyState('J') & 0x8000) s->teststate = 1, s->testkey = 'J', printf("Key = %d\n",s->testkey);
			if(GetKeyState('K') & 0x8000) s->teststate = 1, s->testkey = 'K', printf("Key = %d\n",s->testkey);
			if(GetKeyState('L') & 0x8000) s->teststate = 1, s->testkey = 'L', printf("Key = %d\n",s->testkey);
			if(GetKeyState('M') & 0x8000) s->teststate = 1, s->testkey = 'M', printf("Key = %d\n",s->testkey);
			if(GetKeyState('N') & 0x8000) s->teststate = 1, s->testkey = 'N', printf("Key = %d\n",s->testkey);
			if(GetKeyState('O') & 0x8000) s->teststate = 1, s->testkey = 'O', printf("Key = %d\n",s->testkey);
			if(GetKeyState('P') & 0x8000) s->teststate = 1, s->testkey = 'P', printf("Key = %d\n",s->testkey);
			if(GetKeyState('Q') & 0x8000) s->teststate = 1, s->testkey = 'Q', printf("Key = %d\n",s->testkey);
			if(GetKeyState('R') & 0x8000) s->teststate = 1, s->testkey = 'R', printf("Key = %d\n",s->testkey);
			if(GetKeyState('S') & 0x8000) s->teststate = 1, s->testkey = 'S', printf("Key = %d\n",s->testkey);
			if(GetKeyState('T') & 0x8000) s->teststate = 1, s->testkey = 'T', printf("Key = %d\n",s->testkey);
			if(GetKeyState('U') & 0x8000) s->teststate = 1, s->testkey = 'U', printf("Key = %d\n",s->testkey);
			if(GetKeyState('V') & 0x8000) s->teststate = 1, s->testkey = 'V', printf("Key = %d\n",s->testkey);
			if(GetKeyState('W') & 0x8000) s->teststate = 1, s->testkey = 'W', printf("Key = %d\n",s->testkey);
			if(GetKeyState('X') & 0x8000) s->teststate = 1, s->testkey = 'X', printf("Key = %d\n",s->testkey);
			if(GetKeyState('Y') & 0x8000) s->teststate = 1, s->testkey = 'Y', printf("Key = %d\n",s->testkey);
			if(GetKeyState('Z') & 0x8000) s->teststate = 1, s->testkey = 'Z', printf("Key = %d\n",s->testkey);
		}
		else{
			if(GetKeyState('A') & 0x8000) s->teststate = 1, s->testkey = 'A' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('B') & 0x8000) s->teststate = 1, s->testkey = 'B' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('C') & 0x8000) s->teststate = 1, s->testkey = 'C' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('D') & 0x8000) s->teststate = 1, s->testkey = 'D' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('E') & 0x8000) s->teststate = 1, s->testkey = 'E' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('F') & 0x8000) s->teststate = 1, s->testkey = 'F' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('G') & 0x8000) s->teststate = 1, s->testkey = 'G' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('H') & 0x8000) s->teststate = 1, s->testkey = 'H' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('I') & 0x8000) s->teststate = 1, s->testkey = 'I' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('J') & 0x8000) s->teststate = 1, s->testkey = 'J' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('K') & 0x8000) s->teststate = 1, s->testkey = 'K' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('L') & 0x8000) s->teststate = 1, s->testkey = 'L' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('M') & 0x8000) s->teststate = 1, s->testkey = 'M' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('N') & 0x8000) s->teststate = 1, s->testkey = 'N' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('O') & 0x8000) s->teststate = 1, s->testkey = 'O' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('P') & 0x8000) s->teststate = 1, s->testkey = 'P' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('Q') & 0x8000) s->teststate = 1, s->testkey = 'Q' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('R') & 0x8000){
				RIOPORTState *ioport;
				s->teststate = 1, s->testkey = 'R' + 32, printf("Key = %d\n",s->testkey);
				ioport = Get_RIOPORTState();
				if( (ioport->port9.port & 0x04) != 0 ){
					ioport->port9.port &= 0xFB;
					printf("bldc_reset_on\n");
				}
				else{
					ioport->port9.port |= 0x04;
					printf("bldc_reset_off\n");
				}
				 printf("port9=%d\n",ioport->port9.port);
			}
			if(GetKeyState('S') & 0x8000){
				RIOPORTState *ioport;
				s->teststate = 1, s->testkey = 'S' + 32, printf("Key = %d\n",s->testkey);
				ioport = Get_RIOPORTState();
				if( (ioport->port9.port & 0x02) != 0 ){
					ioport->port9.port &= 0xFD;
					printf("bldc_sw1_on\n");
				}
				else{
					ioport->port9.port |= 0x02;
					printf("bldc_sw1_off\n");
				}
			}
			if(GetKeyState('T') & 0x8000) s->teststate = 1, s->testkey = 'T' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('U') & 0x8000) s->teststate = 1, s->testkey = 'U' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('V') & 0x8000) s->teststate = 1, s->testkey = 'V' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('W') & 0x8000) s->teststate = 1, s->testkey = 'W' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('X') & 0x8000) s->teststate = 1, s->testkey = 'X' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('Y') & 0x8000) s->teststate = 1, s->testkey = 'Y' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('Z') & 0x8000) s->teststate = 1, s->testkey = 'Z' + 32, printf("Key = %d\n",s->testkey);
			if(GetKeyState('1') & 0x8000) s->teststate = 1, s->testkey = '1', printf("Key = %d\n",s->testkey);
			if(GetKeyState('2') & 0x8000) s->teststate = 1, s->testkey = '2', printf("Key = %d\n",s->testkey);
			if(GetKeyState('3') & 0x8000) s->teststate = 1, s->testkey = '3', printf("Key = %d\n",s->testkey);
			if(GetKeyState('4') & 0x8000) s->teststate = 1, s->testkey = '4', printf("Key = %d\n",s->testkey);
			if(GetKeyState('5') & 0x8000) s->teststate = 1, s->testkey = '5', printf("Key = %d\n",s->testkey);
			if(GetKeyState('6') & 0x8000) s->teststate = 1, s->testkey = '6', printf("Key = %d\n",s->testkey);
			if(GetKeyState('7') & 0x8000) s->teststate = 1, s->testkey = '7', printf("Key = %d\n",s->testkey);
			if(GetKeyState('8') & 0x8000) s->teststate = 1, s->testkey = '8', printf("Key = %d\n",s->testkey);
			if(GetKeyState('9') & 0x8000) s->teststate = 1, s->testkey = '9', printf("Key = %d\n",s->testkey);
			if(GetKeyState('0') & 0x8000) s->teststate = 1, s->testkey = '0', printf("Key = %d\n",s->testkey);
			if(GetKeyState(VK_UP) & 0x8000){
				*pKEYTEST_AD1_ADDR2 = *pKEYTEST_AD1_ADDR2 + 40;
				if(*pKEYTEST_AD1_ADDR2 > 0x0FFF){
					*pKEYTEST_AD1_ADDR2 = 0x0FFF;
				}
				//printf("spd = %d\n", *pKEYTEST_AD1_ADDR2);
			}
			if(GetKeyState(VK_DOWN) & 0x8000){
				*pKEYTEST_AD1_ADDR2 = *pKEYTEST_AD1_ADDR2 - 40;
				if(*pKEYTEST_AD1_ADDR2 < 0x0000){
					*pKEYTEST_AD1_ADDR2 = 0x0000;
				}
				//printf("spd = %d\n", *pKEYTEST_AD1_ADDR2);
				
			}
		}
//	}
	
    timer_mod(&s->timer, qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 200);
}




//***********************************************************************************
//! @name QEMU region
//@{
static const MemoryRegionOps rx62t_keytest_mem_ops = {
    .read = KeyTestRead,
    .write = KeyTestWrite,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

//-----------------------------------------------------------------------------

//! Instance initializer.
static void Rx62t_KeyTest_init(Object *obj) {
    DeviceState *dev = DEVICE(obj);
    SysBusDevice *sd = SYS_BUS_DEVICE(obj);
    RKEYTESTState *s = RKEYTEST(obj);
	uint32_t i;
	
	uint8_t * pMatComMemory = GetMatBLDCComMemory();
	signed short* pKEYTEST_AD1_ADDR2 = (signed short*)(pMatComMemory + KEYTEST_AD1_ADDR2_OFFSET);
	*pKEYTEST_AD1_ADDR2 = 0;
	
    memory_region_init_io(&s->mmio, OBJECT(dev), &rx62t_keytest_mem_ops, s,
                          "rx62t_keytest", (uint64_t)TESTKEY_SIZE);
    sysbus_init_mmio(sd, &s->mmio);
	
	for(i=0; i<256; i++){
		s->sentence[i] = (uint8_t)'\0';
	}
    
   	timer_init_ms(&s->timer, QEMU_CLOCK_VIRTUAL, KeyTest_tick, s);
   	timer_mod(&s->timer, qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 100);
    return;
}

static Property Rx62t_KeyTest_propaties[] =  {
    DEFINE_PROP_END_OF_LIST(),
};


//-----------------------------------------------------------------------------
//! Class initializer.
static void Rx62t_KeyTest_class_init(ObjectClass * klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);
    
    device_class_set_props(dc, Rx62t_KeyTest_propaties);
}


static TypeInfo Rx62t_KeyTest_info = {
    .name = TYPE_RENESAS_KEYTEST,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RKEYTESTState),
    .instance_init = Rx62t_KeyTest_init,
    .class_init = Rx62t_KeyTest_class_init,
};

static void Rx62t_KeyTest_register_device(void) {
    type_register_static(&Rx62t_KeyTest_info);
}

type_init(Rx62t_KeyTest_register_device)
//@}

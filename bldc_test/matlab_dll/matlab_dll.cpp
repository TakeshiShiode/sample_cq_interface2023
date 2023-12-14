// matlab_dll.cpp : DLL 用にエクスポートされる関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "matlab_dll.h"
#include "matlab_bldc.h"

//tshiode
static const LPCWSTR MatlabSendEventName = TEXT("MatBLDCTimerInt");
static const LPCTSTR MatlabSharedMemoryName = TEXT("MatBLDCSharedMemory");		//!< Name of shared memory for register and main memory.
static HANDLE	EvMatSend = NULL;
static HANDLE	MatMemoryHandle = NULL;	//!< Handle shared memory for register values.
static BYTE* pMatMemory = NULL;		//!< Pointer to the shared memory for commands.
static bool		gMatComReleaseFlag = false;
#if 0
// これは、エクスポートされた変数の例です
MATLABDLL_API int nmatlabdll=0;

// これは、エクスポートされた関数の例です。
MATLABDLL_API int fnmatlabdll(void)
{
    return 0;
}

// これは、エクスポートされたクラスのコンストラクターです。
Cmatlabdll::Cmatlabdll()
{
    return;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/*MATLAB共有関数類*/
	double MatlabHandleInit(void)
	{
		double ret = 1.0;

		EvMatSend = OpenEvent(EVENT_ALL_ACCESS, FALSE, MatlabSendEventName);
		if (EvMatSend == NULL) {
			ret = 0;
		}

		MatMemoryHandle = OpenFileMapping(FILE_MAP_WRITE, FALSE, MatlabSharedMemoryName);
		if (MatMemoryHandle == NULL) {
			ret = 0;
		}
		pMatMemory = (BYTE*)MapViewOfFile(MatMemoryHandle, FILE_MAP_WRITE, 0, 0, 0);
		if (pMatMemory == NULL) {
			ret = 0;
		}

		//共有レジスタアドレスを各モータへ通知
		SetBLDCMemoryAddress(pMatMemory);

		gMatComReleaseFlag = false;
		*(char*)(pMatMemory + MAT_COM_OFFSET) = 1;
		MatlabInitBLDCData();

		return ret;
	}

	double MatlabReqInt_BLDC(void)
	{
		double ret;
		unsigned char* debug_simulink = (unsigned char*)pMatMemory;
		unsigned char* mat_com_int = (unsigned char*)(pMatMemory + MAT_COM_OFFSET);

		if (gMatComReleaseFlag == false) {
			debug_simulink = (unsigned char*)pMatMemory;
			while ((*debug_simulink) == 1) {
				if (*mat_com_int == 0) {
					gMatComReleaseFlag = true;
					break;
				}
			}
			*debug_simulink = 1;
			SetEvent(EvMatSend);
			ret = 1;
		}
		else {
			ret = 0;
		}
		return ret;
	}

#ifdef __cplusplus
}
#endif

void MatlabReleaseInt(void)
{
	unsigned char* debug_simulink = (unsigned char*)pMatMemory;
	*debug_simulink = 0;
	UnmapViewOfFile(pMatMemory);
	CloseHandle(MatMemoryHandle);
}

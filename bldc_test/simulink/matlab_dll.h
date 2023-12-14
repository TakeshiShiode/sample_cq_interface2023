// 以下の ifdef ブロックは、DLL からのエクスポートを容易にするマクロを作成するための
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された MATLABDLL_EXPORTS
// シンボルを使用してコンパイルされます。このシンボルは、この DLL を使用するプロジェクトでは定義できません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、
// MATLABDLL_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef MATLABDLL_EXPORTS
#define MATLABDLL_API __declspec(dllexport)
#else
#define MATLABDLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	double MatlabHandleInit(void);
	double MatlabReqInt_BLDC(void);

	double MatlabGetBLDC_PWM_U(void);
	double MatlabGetBLDC_PWM_V(void);
	double MatlabGetBLDC_PWM_W(void);
	double MatlabSetBLDCData(double pos, double vdc, double iu, double iw, double omega);
#ifdef __cplusplus
}
#endif

#if 0
// このクラスは dll からエクスポートされました
class MATLABDLL_API Cmatlabdll {
public:
	Cmatlabdll(void);
	// TODO: メソッドをここに追加します。
};

extern MATLABDLL_API int nmatlabdll;

MATLABDLL_API int fnmatlabdll(void);
#endif

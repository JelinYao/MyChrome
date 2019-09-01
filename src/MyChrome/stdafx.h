#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <atlfile.h>
//#include <atlstr.h>
#include <string>
using std::wstring;
using std::string;

#define _CRT_SECURE_NO_WARNINGS

#	pragma comment(lib, "libcef")
#if (_MSC_VER == 1600)
#	pragma comment(lib, "libcef_dll_wrapper100")
#elif (_MSC_VER == 1500)
#	pragma comment(lib, "libcef_dll_wrapper90")
#elif (_MSC_VER == 1800)
#	pragma comment(lib, "libcef_dll_wrapper120")
#elif (_MSC_VER == 1916)
#	pragma comment(lib, "libcef_dll_wrapper150")
#endif

// #ifndef UILIB_API
// #define UILIB_API
// #endif
#include "../DuiLib/UIlib.h"
using namespace DuiLib;
#include "UICefWebkit.h"
#include "UI/Tabbar/UITabBar.h"
#include "ThirdPart/Common/Common.h"
#include "UI/SkinMgr/SkinManager.h"
#include "MsgDefine.h"
#include "CGlobalMgr.h"

struct ShareData
{
	HWND	hWnd;
};
extern CAtlFileMapping<ShareData>	g_bShareInit;
static const wchar_t g_szShareGuid[] = L"{FDEE917E-7284-436e-83CF-823342302E57}";
extern CSkinManager	g_skinManager;



#define SOFT_NAME			L"¼«ËÙä¯ÀÀÆ÷"
#define SOFT_VERSION		L"1.3"
#define SOFT_WEBSITE		L"https://github.com/JelinYao"


extern wstring	g_strExePath;


inline wstring GetExePath()
{
	wchar_t szPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	wstring strPath(szPath);
	int nPos = strPath.find_last_of('\\');
	if (nPos != wstring::npos)
		return strPath.substr(0, nPos+1);
	return L"";
}

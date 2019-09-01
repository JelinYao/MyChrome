// WndShadow.h : header file
//
// Version 0.3
//
// Copyright (c) 2006-2007 Mingliang Zhu, All Rights Reserved.
//
// mailto:perry@live.com
//
//
// This source file may be redistributed unmodified by any means PROVIDING 
// it is NOT sold for profit without the authors expressed written 
// consent, and providing that this notice and the author's name and all 
// copyright notices remain intact. This software is by no means to be 
// included as part of any third party components library, or as part any
// development solution that offers MFC extensions that are sold for profit. 
// 
// If the source code is used in any commercial applications then a statement 
// along the lines of:
// "Portions Copyright (c) 2006-2007 Mingliang Zhu"
// must be included in the "Startup Banner", "About Box" or "Printed
// Documentation". This software is provided "as is" without express or
// implied warranty. Use it at your own risk! The author accepts no 
// liability for any damage/loss of business that this product may cause.
//
/////////////////////////////////////////////////////////////////////////////
//****************************************************************************

//****************************************************************************
// Update history--
//
// Version 0.3, 2007-06-14
//    -The shadow is made Windows Vista Aero awareness.
//    -Fixed a bug that causes the shadow to appear abnormally on Windows Vista.
//    -Fixed a bug that causes the shadow to appear abnormally if parent window
//     is initially minimized or maximized
//
// Version 0.2, 2006-11-23
//    -Fix a critical issue that may make the shadow fail to work under certain
//     conditions, e.g., on Win2000, on WinXP or Win2003 without the visual
//     theme enabled, or when the frame window does not have a caption bar.
//
// Version 0.1, 2006-11-10
//    -First release
//****************************************************************************

#include "StdAfx.h"
#include "WndShadow.h"
#include "math.h"
#include "crtdbg.h"

// Some extra work to make this work in VC++ 6.0

// walk around the for iterator scope bug of VC++6.0
#ifdef _MSC_VER
#if _MSC_VER == 1200
#define for if(false);else for
#endif
#endif

// Some definitions for VC++ 6.0 without newest SDK
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#ifndef ULW_ALPHA
#define ULW_ALPHA 0x00000002
#endif

namespace DuiLib
{
// Vista aero related message
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif


pfnUpdateLayeredWindow		CWndShadow::s_UpdateLayeredWindow = NULL;
HINSTANCE					CWndShadow::s_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
const TCHAR *strWndClassName = _T("PerryShadowWnd");
const TCHAR *g_pPropName	 = _T("CWndShadow-This");


CWndShadow::CWndShadow(void)
: m_hWnd(NULL)
, m_OriParentProc(NULL)
, m_nDarkness(150)
, m_nSharpness(5)
, m_nSize(0)
, m_nxOffset(5)
, m_nyOffset(5)
, m_Color(RGB(0, 0, 0))
, m_WndSize(0)
, m_pManager(NULL) 
, m_pMemImage(NULL)
, m_bPrepared(false)
{
	memset(m_szBkImage, 0, sizeof(TCHAR)*(MAX_PATH+1));
}

CWndShadow::~CWndShadow(void)
{
	if( m_pMemImage )
	{
		if( !m_pMemImage->IsNull() ) 
			m_pMemImage->Destroy();
		delete m_pMemImage;
		m_pMemImage = NULL;
	}
}

bool CWndShadow::Initialize(HINSTANCE hInstance)
{
	// Should not initiate more than once
	if (NULL != s_UpdateLayeredWindow)
		return false;

	HMODULE hSysDll = LoadLibrary(_T("USER32.DLL"));
	s_UpdateLayeredWindow = (pfnUpdateLayeredWindow)GetProcAddress(hSysDll, "UpdateLayeredWindow");
	FreeLibrary(hSysDll);
	// If the import did not succeed, probably layered window is not supported by current OS
	if (NULL == s_UpdateLayeredWindow)
		return false;
	s_hInstance = hInstance;

	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= DefWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= strWndClassName;
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);
	return true;
}

void CWndShadow::SetPrepared()
{
	if ( IsZoomed(m_hParentWnd) )
		ShowWindow(m_hWnd, FALSE);
	if ( m_bPrepared )
		return ;
	m_bPrepared = true;
	RECT WndRect;
	GetWindowRect(m_hParentWnd, &WndRect);//得到正常窗口的位置
	//计算阴影窗口的位置
	int nx = WndRect.left - m_rcBorder.left;
	int ny = WndRect.top - m_rcBorder.top;
	int nwidth = WndRect.right + m_rcBorder.right - nx;
	int nheight = WndRect.bottom + m_rcBorder.bottom - ny;
	::MoveWindow(m_hWnd,nx,ny,nwidth,nheight,TRUE);
	ShowWindow(m_hWnd, SW_SHOWNA);
}

void CWndShadow::SetBkImage(const TCHAR* pBkImage)
{
	_tcscpy(m_szBkImage, pBkImage);
}

void CWndShadow::SetBorder(RECT rcBorder)
{
	m_rcBorder.left = rcBorder.left;
	m_rcBorder.right = rcBorder.right;
	m_rcBorder.top = rcBorder.top;
	m_rcBorder.bottom = rcBorder.bottom;
}

void CWndShadow::Create(HWND hParentWnd, CPaintManagerUI* pManager)
{
	m_pManager		= pManager;
	m_hParentWnd	= hParentWnd;
	if(NULL == s_UpdateLayeredWindow)
		return;
	_ASSERT(s_hInstance != INVALID_HANDLE_VALUE);
	//创建一个阴影窗口
	m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, strWndClassName, NULL, WS_POPUP, \
		CW_USEDEFAULT, 0, 0, 0, hParentWnd, NULL, s_hInstance, NULL);

	m_Status = SS_ENABLED;	//设置属性为不可用状态
	BOOL bAero = FALSE;//禁用aero特效
	Show(hParentWnd);	//
	SetProp(hParentWnd, g_pPropName, this);
	// Replace the original WndProc of parent window to steal messages
	m_OriParentProc = GetWindowLong(hParentWnd, GWL_WNDPROC);//得到父窗口的 消息循环函数地址

#pragma warning(disable: 4311)	// temporrarily disable the type_cast warning in Win32
	SetWindowLong(hParentWnd, GWL_WNDPROC, (LONG)ParentProc);//设置 父窗口消息循环新地址先走阴影窗口的消息循环
#pragma warning(default: 4311)

}

LRESULT CALLBACK CWndShadow::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndShadow* pThis = (CWndShadow*)GetProp(hwnd, g_pPropName);
	ASSERT(pThis);
	WNDPROC pDefProc = (WNDPROC)pThis->m_OriParentProc;
	switch(uMsg)
	{
	case WM_MOVE://此流程应该没问题
		{
			if ( IsZoomed(hwnd) || !IsWindowVisible(pThis->m_hWnd) )
				break;
			RECT WndRect;
			GetWindowRect(hwnd, &WndRect);//得到正常窗口的位置
			int nx = WndRect.left - pThis->m_rcBorder.left;
			int ny = WndRect.top - pThis->m_rcBorder.top;
			int nwidth = WndRect.right + pThis->m_rcBorder.right - nx;
			int nheight = WndRect.bottom + pThis->m_rcBorder.bottom - ny;
			if(pThis->m_bPrepared)
				::MoveWindow(pThis->m_hWnd, nx, ny, nwidth, nheight, TRUE);
			else
				::MoveWindow(pThis->m_hWnd, nx, ny, nwidth, nheight, FALSE);	
			ShowWindow(pThis->m_hWnd, SW_NORMAL);
		}
		break;
	case WM_SIZE://正常窗口尺寸发生改变的时候
		{
			switch( wParam )
			{
			case SIZE_RESTORED:
				//ShowWindow(pThis->m_hWnd, SW_SHOWNOACTIVATE);
				pThis->m_bPrepared = false;
				pThis->SetPrepared();
				break;
			case SIZE_MAXIMIZED:
			case SIZE_MINIMIZED:
				ShowWindow(pThis->m_hWnd, SW_HIDE);
				break;
			}
			pThis->m_WndSize = lParam;
		}
		break;
	case WM_SIZING:
		pThis->Update(hwnd);
		break;
	case WM_EXITSIZEMOVE:
		if( pThis->m_bPrepared && IsWindowVisible(pThis->m_hWnd) )
			pThis->Update(hwnd);
		break;
	case WM_DESTROY:
		DestroyWindow(pThis->m_hWnd);	// Destroy the shadow
		break;
	case WM_SHOWWINDOW://主窗口调用ShowWindow函数会有这个消息
		if ( wParam == TRUE )
			pThis->Update(hwnd);
		else
			ShowWindow(pThis->m_hWnd, SW_HIDE);
		break;
	}
	return pDefProc(hwnd, uMsg, wParam, lParam);
}

void CWndShadow::Update(HWND hParent)
{
	if ( IsZoomed(m_hParentWnd) )
		return ;
	if( _tcslen(m_szBkImage) == 0 )
	{
		RECT WndRect;
		GetWindowRect(hParent, &WndRect);
		int nShadWndWid = WndRect.right - WndRect.left + m_nSize * 2;
		int nShadWndHei = WndRect.bottom - WndRect.top + m_nSize * 2;

		// Create the alpha blending bitmap
		BITMAPINFO bmi;        // bitmap header

		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = nShadWndWid;
		bmi.bmiHeader.biHeight = nShadWndHei;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = nShadWndWid * nShadWndHei * 4;

		BYTE *pvBits;          // pointer to DIB section
		HBITMAP hbitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);

		ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
		MakeShadow((UINT32 *)pvBits, hParent, &WndRect);

		HDC hMemDC = CreateCompatibleDC(NULL);
		HBITMAP hOriBmp = (HBITMAP)SelectObject(hMemDC, hbitmap);

		POINT ptDst = {WndRect.left + m_nxOffset - m_nSize, WndRect.top + m_nyOffset - m_nSize};
		POINT ptSrc = {0, 0};
		SIZE WndSize = {nShadWndWid, nShadWndHei};
		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

		MoveWindow(m_hWnd, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

		BOOL bRet= s_UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &WndSize, hMemDC,
			&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

		_ASSERT(bRet); // something was wrong....

		// Delete used resources
		SelectObject(hMemDC, hOriBmp);
		DeleteObject(hbitmap);
		DeleteDC(hMemDC);
		return ;
	}
	//m_bPrepared = false;
	//ShowWindow(m_hWnd, SW_HIDE);
	ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	RECT WndRect;
	GetWindowRect(hParent, &WndRect);
	int nShadWndWid = WndRect.right - WndRect.left + m_rcBorder.left + m_rcBorder.right;
	int nShadWndHei = WndRect.bottom - WndRect.top + m_rcBorder.top + m_rcBorder.bottom;
	POINT ptDst = {WndRect.left - m_rcBorder.left, WndRect.top - m_rcBorder.top};
	POINT ptSrc = {0, 0};
	SIZE WndSize = {nShadWndWid, nShadWndHei};
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	m_pMemImage = new CDuiImage();
	m_pMemImage->CreateEmptyImage(nShadWndWid, nShadWndHei, 32);

	RECT rcPaint;
	rcPaint.left = 0;
	rcPaint.right = nShadWndWid;
	rcPaint.top = 0;
	rcPaint.bottom = nShadWndHei;

	CRenderEngine::DrawImageString(m_pMemImage->GetDC(), m_pManager, rcPaint, rcPaint, m_szBkImage, NULL);
	m_pMemImage->ReleaseDC();

	MoveWindow(m_hWnd, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

	BOOL bRet = s_UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &WndSize, m_pMemImage->GetDC(),
		&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

	m_pMemImage->ReleaseDC();
	if( !m_pMemImage->IsNull() ) 
		m_pMemImage->Destroy();
	delete m_pMemImage;
	m_pMemImage = NULL;
}

void CWndShadow::MakeShadow(UINT32 *pShadBits, HWND hParent, RECT *rcParent)
{
	// The shadow algorithm:
	// Get the region of parent window,
	// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
	// Apply modified (with blur effect) morphologic dilation to make the blurred border
	// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it

	// Get the region of parent window,
	// Create a full rectangle region in case of the window region is not defined
	HRGN hParentRgn = CreateRectRgn(0, 0, rcParent->right - rcParent->left, rcParent->bottom - rcParent->top);
	GetWindowRgn(hParent, hParentRgn);

	// Determine the Start and end point of each horizontal scan line
	SIZE szParent = {rcParent->right - rcParent->left, rcParent->bottom - rcParent->top};
	SIZE szShadow = {szParent.cx + 2 * m_nSize, szParent.cy + 2 * m_nSize};
	// Extra 2 lines (set to be empty) in ptAnchors are used in dilation
	int nAnchors = max(szParent.cy, szShadow.cy);	// # of anchor points pares
	int (*ptAnchors)[2] = new int[nAnchors + 2][2];
	int (*ptAnchorsOri)[2] = new int[szParent.cy][2];	// anchor points, will not modify during erosion
	ptAnchors[0][0] = szParent.cx;
	ptAnchors[0][1] = 0;
	ptAnchors[nAnchors + 1][0] = szParent.cx;
	ptAnchors[nAnchors + 1][1] = 0;
	if(m_nSize > 0)
	{
		// Put the parent window anchors at the center
		for(int i = 0; i < m_nSize; i++)
		{
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchors[i + 1][1] = 0;
			ptAnchors[szShadow.cy - i][0] = szParent.cx;
			ptAnchors[szShadow.cy - i][1] = 0;
		}
		ptAnchors += m_nSize;
	}
	for(int i = 0; i < szParent.cy; i++)
	{
		// find start point
		int j;
		for(j = 0; j < szParent.cx; j++)
		{
			if(PtInRegion(hParentRgn, j, i))
			{
				ptAnchors[i + 1][0] = j + m_nSize;
				ptAnchorsOri[i][0] = j;
				break;
			}
		}

		if(j >= szParent.cx)	// Start point not found
		{
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchorsOri[i][1] = 0;
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchorsOri[i][1] = 0;
		}
		else
		{
			// find end point
			for(j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--)
			{
				if(PtInRegion(hParentRgn, j, i))
				{
					ptAnchors[i + 1][1] = j + 1 + m_nSize;
					ptAnchorsOri[i][1] = j + 1;
					break;
				}
			}
		}
// 		if(0 != ptAnchorsOri[i][1])
// 			_RPTF2(_CRT_WARN, "%d %d\n", ptAnchorsOri[i][0], ptAnchorsOri[i][1]);
	}

	if(m_nSize > 0)
		ptAnchors -= m_nSize;	// Restore pos of ptAnchors for erosion
	int (*ptAnchorsTmp)[2] = new int[nAnchors + 2][2];	// Store the result of erosion
	// First and last line should be empty
	ptAnchorsTmp[0][0] = szParent.cx;
	ptAnchorsTmp[0][1] = 0;
	ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
	ptAnchorsTmp[nAnchors + 1][1] = 0;
	int nEroTimes = 0;
	// morphologic erosion
	for(int i = 0; i < m_nSharpness - m_nSize; i++)
	{
		nEroTimes++;
		//ptAnchorsTmp[1][0] = szParent.cx;
		//ptAnchorsTmp[1][1] = 0;
		//ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
		//ptAnchorsTmp[szParent.cy + 1][1] = 0;
		for(int j = 1; j < nAnchors + 1; j++)
		{
			ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
			ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
		}
		// Exchange ptAnchors and ptAnchorsTmp;
		int (*ptAnchorsXange)[2] = ptAnchorsTmp;
		ptAnchorsTmp = ptAnchors;
		ptAnchors = ptAnchorsXange;
	}

	// morphologic dilation
	ptAnchors += (m_nSize < 0 ? -m_nSize : 0) + 1;	// now coordinates in ptAnchors are same as in shadow window
	// Generate the kernel
	int nKernelSize = m_nSize > m_nSharpness ? m_nSize : m_nSharpness;
	int nCenterSize = m_nSize > m_nSharpness ? (m_nSize - m_nSharpness) : 0;
	UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
	UINT32 *pKernelIter = pKernel;
	for(int i = 0; i <= 2 * nKernelSize; i++)
	{
		for(int j = 0; j <= 2 * nKernelSize; j++)
		{
			double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));
			if(dLength < nCenterSize)
				*pKernelIter = m_nDarkness << 24 | PreMultiply(m_Color, m_nDarkness);
			else if(dLength <= nKernelSize)
			{
				UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (m_nSharpness + 1)) * m_nDarkness));
				*pKernelIter = nFactor << 24 | PreMultiply(m_Color, nFactor);
			}
			else
				*pKernelIter = 0;
			//TRACE("%d ", *pKernelIter >> 24);
			pKernelIter ++;
		}
		//TRACE("\n");
	}
	// Generate blurred border
	for(int i = nKernelSize; i < szShadow.cy - nKernelSize; i++)
	{
		int j;
		if(ptAnchors[i][0] < ptAnchors[i][1])
		{

			// Start of line
			for(j = ptAnchors[i][0];
				j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
				j++)
			{
				for(int k = 0; k <= 2 * nKernelSize; k++)
				{
					UINT32 *pPixel = pShadBits +
						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
					for(int l = 0; l <= 2 * nKernelSize; l++)
					{
						if(*pPixel < *pKernelPixel)
							*pPixel = *pKernelPixel;
						pPixel++;
						pKernelPixel++;
					}
				}
			}	// for() start of line

			// End of line
			for(j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1);
				j < ptAnchors[i][1];
				j++)
			{
				for(int k = 0; k <= 2 * nKernelSize; k++)
				{
					UINT32 *pPixel = pShadBits +
						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
					for(int l = 0; l <= 2 * nKernelSize; l++)
					{
						if(*pPixel < *pKernelPixel)
							*pPixel = *pKernelPixel;
						pPixel++;
						pKernelPixel++;
					}
				}
			}	// for() end of line

		}
	}	// for() Generate blurred border

	// Erase unwanted parts and complement missing
	UINT32 clCenter = m_nDarkness << 24 | PreMultiply(m_Color, m_nDarkness);
	for(int i = min(nKernelSize, max(m_nSize - m_nyOffset, 0));
		i < max(szShadow.cy - nKernelSize, min(szParent.cy + m_nSize - m_nyOffset, szParent.cy + 2 * m_nSize));
		i++)
	{
		UINT32 *pLine = pShadBits + (szShadow.cy - i - 1) * szShadow.cx;
		if(i - m_nSize + m_nyOffset < 0 || i - m_nSize + m_nyOffset >= szParent.cy)	// Line is not covered by parent window
		{
			for(int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++)
			{
				*(pLine + j) = clCenter;
			}
		}
		else
		{
			for(int j = ptAnchors[i][0];
				j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, ptAnchors[i][1]);
				j++)
				*(pLine + j) = clCenter;
			for(int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, 0);
				j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, szShadow.cx);
				j++)
				*(pLine + j) = 0;
			for(int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, ptAnchors[i][0]);
				j < ptAnchors[i][1];
				j++)
				*(pLine + j) = clCenter;
		}
	}

	// Delete used resources
	delete[] (ptAnchors - (m_nSize < 0 ? -m_nSize : 0) - 1);
	delete[] ptAnchorsTmp;
	delete[] ptAnchorsOri;
	delete[] pKernel;
	DeleteObject(hParentRgn);
}

bool CWndShadow::SetSize(int NewSize)
{
	if(NewSize > 20 || NewSize < -20)
		return false;
	m_nSize = (signed char)NewSize;
	if(SS_VISABLE & m_Status)
		Update(GetParent(m_hWnd));
	return true;
}

bool CWndShadow::SetSharpness(unsigned int NewSharpness)
{
	if(NewSharpness > 20)
		return false;

	m_nSharpness = (unsigned char)NewSharpness;
	if(SS_VISABLE & m_Status)
		Update(m_hParentWnd);
	return true;
}

bool CWndShadow::SetDarkness(unsigned int NewDarkness)
{
	if(NewDarkness > 255)
		return false;

	m_nDarkness = (unsigned char)NewDarkness;
	if(SS_VISABLE & m_Status)
		Update(m_hParentWnd);
	return true;
}

bool CWndShadow::SetPosition(int NewXOffset, int NewYOffset)
{
	if(NewXOffset > 20 || NewXOffset < -20 ||
		NewYOffset > 20 || NewYOffset < -20)
		return false;
	
	m_nxOffset = (signed char)NewXOffset;
	m_nyOffset = (signed char)NewYOffset;
	if(SS_VISABLE & m_Status)
		Update(m_hParentWnd);
	return true;
}

bool CWndShadow::SetColor(COLORREF NewColor)
{
	m_Color = NewColor;
	if(SS_VISABLE & m_Status)
		Update(m_hParentWnd);
	return true;
}

void CWndShadow::Show(HWND hParentWnd)
{
	// 清空aero特效属性
	m_Status &= SS_ENABLED | SS_DISABLEDBYAERO;

	if((m_Status & SS_ENABLED) && !(m_Status & SS_DISABLEDBYAERO))	//只有不可用状态的时候才去获取窗口样式
	{
		// 得到窗口的样式
		LONG lParentStyle = GetWindowLong(hParentWnd, GWL_STYLE);

		if(WS_VISIBLE & lParentStyle)	// 如果父窗口是显示的 那么继续判断 窗口是否最小化和最大化 设置阴影窗口是否显示
		{
			m_Status |= SS_PARENTVISIBLE;

			if(!((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle))	//最大化和最小化的时候 不需要现实阴影窗口
				m_Status |= SS_VISABLE;
		}
	}

	if(m_Status & SS_VISABLE)
	{
		if(m_bPrepared)
			ShowWindow(m_hWnd, SW_SHOWNA);
		Update(hParentWnd);
	}
	else
		ShowWindow(m_hWnd, SW_HIDE);
	
}
}
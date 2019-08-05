#pragma once
#include <string>
#include <map>
using std::wstring;
using std::map;
#include "stdafx.h"


//extern CData		g_data;
struct WND_DATA
{
	HWND	hWnd;
	CControlUI* pSkinCtrl;
};

class CSkinManager
{
public:
	CSkinManager()
		:m_dwSelColor(0xFF0194FE)
		,m_nSelLine(-1)
		,m_nSelCol(-1)
	{
		
	}
	virtual ~CSkinManager()
	{
		m_wndMap.clear();
	}
	DWORD	GetSelectColor()	{ return m_dwSelColor; }
	int		GetSelectLine()		{ return m_nSelLine; }
	int		GetSelectColumn()	{ return m_nSelCol; }
	void AddToMap(WindowImplBase* pWnd, CControlUI* pSkinCtrl)
	{
		wstring strWndClass(pWnd->GetWindowClassName());
		map<wstring, WND_DATA>::iterator itor = m_wndMap.find(strWndClass);
		if ( itor != m_wndMap.end() )
			m_wndMap.erase(itor);
		WND_DATA data;
		data.hWnd		= pWnd->GetHWND();
		data.pSkinCtrl	= pSkinCtrl;
		m_wndMap.insert(std::pair<wstring, WND_DATA>(strWndClass, data));
		pSkinCtrl->SetBkColor(m_dwSelColor);
	}
	void ChangeSkinColor(DWORD dwColor, bool bWrite=true)
	{//改变背景颜色
		SetSkinColor(dwColor);
		m_dwSelColor = dwColor;
// 		if ( bWrite )
// 			g_data.SetBkColor(m_dwSelColor);
	}
	void SetSkinColor(DWORD dwColor)
	{//设置背景颜色，并不改变默认值
		map<wstring, WND_DATA>::iterator itor = m_wndMap.begin();
		while( itor != m_wndMap.end() )
		{
			WND_DATA& data = itor->second;
			if ( !IsWindow(data.hWnd) )
			{
				itor = m_wndMap.erase(itor);
				continue;
			}
			data.pSkinCtrl->SetBkColor(dwColor);
			itor++;
		}
	}
	void ResetSkinColor()
	{//还原背景颜色为默认值
		SetSkinColor(m_dwSelColor);
	}
	void SetSelPoint(const int nLine, const int nColumn)
	{
		m_nSelCol	= nColumn;
		m_nSelLine	= nLine;
	}
private:
	int		m_nSelLine;
	int		m_nSelCol;
	DWORD	m_dwSelColor;
	map<wstring, WND_DATA>	m_wndMap;
};
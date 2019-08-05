#pragma once
#include "MsgDefine.h"

class CGlobalMgr
{
public:
	static CGlobalMgr* Instance()
	{
		static CGlobalMgr mgr;
		return &mgr;
	}
	void SetMainWnd(HWND hWnd)	{ m_hMainWnd = hWnd; }
	HWND GetMainWnd()const	{ return m_hMainWnd; }
	void SetStateText(LPCWSTR lpText)
	{
		if (m_hMainWnd && IsWindow(m_hMainWnd))
			::SendMessage(m_hMainWnd, WM_SET_STATE_TEXT_MSG, 0, (LPARAM)lpText);
	}

protected:
	CGlobalMgr()
		: m_hMainWnd(NULL)
	{

	}

private:
	HWND	m_hMainWnd;
};


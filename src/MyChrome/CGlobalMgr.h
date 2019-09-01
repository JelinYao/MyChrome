#pragma once
#include "MsgDefine.h"
#include <string>
using std::wstring;


#define HOMEPAGE_URL		L"https://github.com/JelinYao"
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
	const wstring& GetHomePage()const { return m_strHomePage; }
	void SetStateText(LPCWSTR lpText)
	{
		if (m_hMainWnd && IsWindow(m_hMainWnd))
			::SendMessage(m_hMainWnd, WM_SET_STATE_TEXT_MSG, 0, (LPARAM)lpText);
	}

protected:
	CGlobalMgr()
		: m_hMainWnd(NULL)
		, m_strHomePage(HOMEPAGE_URL)
	{

	}

private:
	wstring	m_strHomePage;
	HWND	m_hMainWnd;
};


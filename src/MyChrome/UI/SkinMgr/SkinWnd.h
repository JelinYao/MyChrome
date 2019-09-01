#pragma once
#include "UIColorSkin.h"

class CSkinWnd
	: public WindowImplBase
	, public CSelectColorCallback
{
public:
	CSkinWnd(void);
	~CSkinWnd(void);
	virtual LPCTSTR		GetWindowClassName()const	{ return _T("»»·ô");		}
	virtual CDuiString	GetSkinFolder()				{ return _T("Skin");		}
	virtual CDuiString	GetSkinFile()				{ return _T("SkinWnd.xml"); }
	virtual void		InitWindow();
	virtual void		OnFinalMessage( HWND hWnd );
	virtual void		OnClick(TNotifyUI& msg);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
	virtual void	ColorTest(DWORD color);
private:
	CButtonUI*	m_pBtnOk;
	CColorSkinUI*	m_pColorSkin;
	CHorizontalLayoutUI* m_pTitleBar;
};

#pragma once

class CAboutWnd
	: public WindowImplBase
{
public:
	CAboutWnd(void);
	~CAboutWnd(void);
	virtual LPCTSTR		GetWindowClassName()const	{ return _T("¹ØÓÚ"); }
	virtual CDuiString	GetSkinFolder()				{ return _T("Skin"); }
	virtual CDuiString	GetSkinFile()				{ return _T("About.xml"); }
	virtual void		InitWindow();
	virtual void		OnFinalMessage( HWND hWnd );
	virtual void		OnClick(TNotifyUI& msg);

private:
	CLabelUI*	m_pLabelVersion;
	CButtonUI*	m_pBtnOk;
	CButtonUI*	m_pBtnWeb;
};

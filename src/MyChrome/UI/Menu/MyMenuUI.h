#pragma once

#define DUI_MSGTYPE_CLICK_MENU_ITEM 		   (_T("clickmenuitem"))


class CMyMenuUI
	: public WindowImplBase
{
public:
	CMyMenuUI(CControlUI* pSender);
	~CMyMenuUI(void);
	virtual LPCTSTR		GetWindowClassName()const	{ return _T("MenuWndClass"); }
	virtual CDuiString	GetSkinFolder()				{ return _T("Skin"); }
	virtual CDuiString	GetSkinFile()				{ return _T("Menu\\menu.xml"); }
	void	SetWndSize(int nWidth, int nHeight);
	void	AddMenuItem(LPCTSTR lpText, int xPadding, bool bEnable=true);

protected:
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool	OnNotifyMenuItem(void* lpParam);

private:
	int		m_nWidth;
	int		m_nHeight;
	CListUI	*m_pList;
	CControlUI *m_pCtrlSender;
};


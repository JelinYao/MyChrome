#pragma once

class CEditMenuWnd
	: public CMenuImplBase
{
public:
	CEditMenuWnd(CControlUI* pEditCtrl);
	~CEditMenuWnd(void);
	virtual CDuiString	GetSkinFolder()		{ return L"Skin"; }
	virtual CDuiString	GetSkinFile()		{ return L"EditMenu.xml"; }
protected:
	virtual void		OnFinalMessage(HWND hWnd);
	virtual void		OnClick(TNotifyUI& msg);
	virtual void		InitWindow();
private:
	CControlUI*		m_pEditCtl;	//保存控件指针，可以是Edit或者RichEdit
	CControlUI*		m_pUndo;
	CControlUI*		m_pCut;
	CControlUI*		m_pCopy;
	CControlUI*		m_pPaste;
};

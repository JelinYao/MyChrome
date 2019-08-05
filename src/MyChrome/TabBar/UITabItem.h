#pragma once

#define TAB_ITEM_CLOSE		L"TabItemClose"
#define TAB_ITEM_CLICK		L"TabItemClick"
#define TAB_ITEM_DBCLICK	L"TabItemDbClick"


class CTabItemUI :
	public CContainerUI
{
public:
	CTabItemUI(void);
	~CTabItemUI(void);
	void	SetParent(CControlUI* pTab)		{ m_pParent = pTab; }
	void	ShowCloseBtn(bool bShow);

protected:
	virtual void Init();
	virtual void PaintText(HDC hDC);
	virtual void SetPos(RECT rc);
	virtual void DoEvent(TEventUI& event);
	virtual LPCTSTR GetClass() const	{ return L"UITabItem"; }
	bool	OnBtnClose(void* pParam);

private:
	CButtonUI*		m_pBtnExit;
	CControlUI*		m_pParent;
};


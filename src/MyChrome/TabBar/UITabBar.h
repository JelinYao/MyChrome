#pragma once


class CTabBarCallback
{
public:
	virtual void	OnItemClose(void* pTabBar, const int nDelIndex, const int nSelIndex) = 0;
	virtual void	OnItemSelectChange(void* pTabBar, const int nUnSelIndex, const int nSelIndex) = 0;
	virtual void	OnItemAdd(void* pTabBar) = 0;
	virtual void	OnItemDbClick(void* pTabBar, const int nIndex) = 0;
};


class CTabBarUI
	: public CContainerUI
{
public:
	CTabBarUI(void);
	~CTabBarUI(void);
	CControlUI*	AddTabItem(LPCTSTR lpText, bool bReset=false, bool bShowClose=true);
	void	SelectItem(CControlUI* pItem);
	void	DeleteItem(CControlUI* pItem);
	void	DeleteItem(const size_t nIndex);
	void	SetItemText(const size_t nIndex, LPCTSTR lpText);
	int		GetItemCount()const								{ return m_pTabItems.size(); }
	void	SetCallback(CTabBarCallback* pCallback)			{ m_pCallback = pCallback; }
	void	ShowAddBtn(bool bShow)							{ m_bShowAdd = bShow; }

protected:
	virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void	Init();
	virtual void	SetPos(RECT rc);
	virtual LPCTSTR GetClass() const	{ return L"UITabBar"; }
	bool	OnNotifySelf(void* param);
	void	ResetTabPos();
	int		GetTabItemIndex(CControlUI* pItem);
	bool	OnBtnClickAdd(void* pParam);

private:
	bool	m_bShowAdd;
	int		m_nNorWidth;
	int		m_nMinWidth;
	int		m_nCurWidth;
	size_t		m_nSelIndex;
	DWORD	m_dwTabSelColor;
	DWORD	m_dwTabNorColor;
	CButtonUI* m_pBtnAdd;
	vector<CControlUI*> m_pTabItems;
	CTabBarCallback*	m_pCallback;
};


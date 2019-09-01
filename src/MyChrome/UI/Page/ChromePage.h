#pragma once


enum WebLoadState
{
	Wls_Begin = 0,
	Wls_Finish,
	Wls_Error,
};
class CChromePage
	: public CContainerUI
{
public:
	CChromePage(CPaintManagerUI* pManager, bool bNewBank);
	~CChromePage(void);
	CCefWebkitUI*	GetWebkit()			{ return m_pWebkit; }
	CEditUI*		GetEdit()			{ return m_pEdit; }
	bool	IsNewBank()const				{ return m_bIsNewBank; }
	void	SetNewBank(bool bValue)		{ m_bIsNewBank = bValue; }
	void	SetEditText(LPCWSTR lpText);
	void	OnSize(int nWidth, int nHeight);
	void	LoadUrl(LPCTSTR lpUrl);
	void	UpdateLoadState(WebLoadState state);
	void	Close();

protected:
	virtual void Init();
	bool	OnNotifyBtnBack(void* lpParam);
	bool	OnNotifyBtnRefresh(void* lpParam);
	bool	OnNotifyBtnHome(void* lpParam);

private:
	bool	m_bIsNewBank;
	CCefWebkitUI	*m_pWebkit;
	CEditUI			*m_pEdit;
	CControlUI		*m_pBtnBack;
	CControlUI		*m_pBtnRefresh;
	CControlUI		*m_pBtnHome;
};

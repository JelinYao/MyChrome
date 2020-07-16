#pragma once
#include "UI/Page/ChromePage.h"



class CMainWnd :
	public WindowImplBase,
	public CCefWebCallback,
	public CTabBarCallback
{
public:
	CMainWnd(void);
	~CMainWnd(void);
	virtual LPCTSTR		GetWindowClassName(void) const	{ return L"MainWndClass"; }
	virtual CDuiString	GetSkinFolder()					{ return L"Skin"; }
	virtual CDuiString	GetSkinFile()					{ return L"MainWnd.xml"; }
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;

protected:
	virtual void	OnFinalMessage( HWND hWnd );
	virtual void	OnCloseWnd();
	virtual CControlUI*	CreateControl(LPCTSTR pstrClass);
	virtual LRESULT	HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void	InitWindow();
	virtual bool	HitTestControl(CControlUI* pCtrl);
	//网页加载状态回调
	virtual void OnLoadStart(CCefWebkitUI* pWeb, void* lpParam);
	virtual void OnLoadEnd(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser);
	virtual void OnLoadError(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefFrame> pFrame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl);
	virtual void OnSetAddress(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url);
	virtual void OnSetTitle(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& title);
	virtual bool OnOpenNewUrl(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url);
	virtual void OnStatusMessage(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& msg);
	virtual bool OnBeforeBrowse(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, const std::wstring& url);
	//TAB控件回调
	virtual void	OnItemClose(void* pTabBar, const int nDelIndex, const int nSelIndex);
	virtual void	OnItemSelectChange(void* pTabBar, const int nUnSelIndex, const int nSelIndex);
	virtual void	OnItemAdd(void* pTabBar);
	virtual void	OnItemDbClick(void* pTabBar, const int nIndex);
	//控件消息处理
	bool	OnNotifyUrlEdit( void* lpParam );
	bool	OnNotifyBtnSkin(void* lpParam);
	bool	OnNotifyBtnSet(void* lpParam);
	void	OnTimer(WPARAM wParam, LPARAM lParam);
	//其他
	bool	AddNewBrowser(LPCTSTR lpUrl, bool bNewBank=false);
	CChromePage* GetCurrentSelPage();
	void	CloseAllWeb();
	inline void SetStatus(LPCWSTR lpText)
	{
		KillTimer(m_hWnd, TIMER_ID_STATUS_TEXT);
		if (lpText && m_pLblState)
			m_pLblState->SetText(lpText);
		SetTimer(m_hWnd, TIMER_ID_STATUS_TEXT, 2000, NULL);
	}

private:
	bool		m_bClose;
	CLabelUI	*m_pLblState;
	CTabBarUI	*m_pTabBar;
	CTabLayoutUI *m_pTabWeb;
};


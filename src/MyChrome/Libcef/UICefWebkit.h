#pragma once
#include "CefBase/util.h"
#include "CefBase/CefHandler.h"
#include "CefBase/CefAppEx.h"
#include "CefBase/CEfV8Handler.h"
#include "CefBase/Delegate.h"

class CCefWebkitUI;
//��ҳ����״̬�Ļص�
class CCefWebCallback
{
public:
	virtual void OnLoadStart(CCefWebkitUI* pWeb, void* lpParam)									= 0;
	virtual void OnLoadEnd(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser)	= 0;
	virtual void OnLoadError(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefFrame> pFrame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl)		= 0;
	virtual void OnSetAddress(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url)		= 0;
	virtual void OnSetTitle(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& title)		= 0;
	virtual bool OnOpenNewUrl(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url)		= 0;
	virtual void OnStatusMessage(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& msg)	= 0;
	virtual bool OnBeforeBrowse(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, const std::wstring& url) = 0;
};



class CCefWebkitUI
	: public CControlUI
	, public Delegate
	
{
public:
	CCefWebkitUI(LPCTSTR lpUrl = L"");
	virtual ~CCefWebkitUI();
	virtual LPCTSTR	GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);
	virtual void	SetPos(RECT rc);
	virtual void	DoInit();
	virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void	SetInternVisible(bool bVisible = true);
	HWND	GetHostWnd();
	wstring	GetLoadingUrl();
	void	Close();
	void	SetWebloadCallback(CCefWebCallback* pCallback, void* lpParam);
	void	ExecuteJavascript(const wstring& strCode);
	CefRefPtr<CefBrowser>	GetBrowser();
	CefRefPtr<CefFrame>		GetMainFrame();
	CCefHandler* GetCefHandle()	{ return m_cefHandles.get(); }
	bool CanGoBack() { return (m_pWebBrowser.get())?m_pWebBrowser->CanGoBack():false; }
	void SetTitle(LPCTSTR lpTitle) { m_strTitle = lpTitle; }
	const wstring& GetTitle()const { return m_strTitle; }
	bool IsTitleEmpty()const { return m_strTitle.empty(); }
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRePaint);
	void Navigate(LPCTSTR lpUrl);
	void Refresh();
	void Reload();
	void GoBack();
	void GoForward();
	bool SetZoomRatio(int ratio);
	int GetZoomRatio() { return m_zoomRatio; }

protected:

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
	virtual void OnSetAddress(const std::wstring& url);
	virtual void OnSetTitle(const std::wstring& title);
	virtual void OnSetFullscreen(bool fullscreen);
	virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
	virtual void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions);
	virtual bool OnOpenNewUrl(const std::wstring& url);
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode);
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl);
	virtual void OnStatusMessage(const std::wstring& msg);
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, const std::wstring& url);

private:
	int m_zoomRatio;//��ҳ���ű���
	wstring	m_strHomePage;
	wstring	m_strTitle;
	void*	m_lpCallbackData;
	CCefWebCallback	*m_pCallback;
	CefRefPtr<CefBrowser>	m_pWebBrowser;
	CefRefPtr<CCefHandler>	m_cefHandles;
};
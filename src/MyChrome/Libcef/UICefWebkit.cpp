#include "StdAfx.h"
#include "UICefWebkit.h"
#include "Win32Wnd.h"



CCefWebkitUI::CCefWebkitUI(LPCTSTR lpUrl /*= L""*/)
: m_strHomePage(lpUrl)
, m_cefHandles(new client::ClientHandler(this, false, ""))
, m_lpCallbackData(NULL)
, m_pCallback(NULL)
, m_zoomRatio(100)
{

}

CCefWebkitUI::~CCefWebkitUI()
{

}

LPCTSTR CCefWebkitUI::GetClass() const
{
	return L"CefWebkitUI";
}

LPVOID CCefWebkitUI::GetInterface( LPCTSTR pstrName )
{
	if ( _tcscmp(pstrName, L"CefWebkit") == 0 )
		return static_cast<CCefWebkitUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CCefWebkitUI::SetPos( RECT rc )
{
	CControlUI::SetPos(rc);
	HWND hWnd = GetHostWnd();
	if (hWnd && IsWindow(hWnd))
		::MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
}

void CCefWebkitUI::DoInit()
{
	RECT rc = {0, 0, 0, 0};
	m_cefHandles->CreateBrowser(m_pManager->GetPaintWindow(), rc, m_strHomePage.c_str());
}

void CCefWebkitUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if ( _tcscmp(pstrName, _T("url")) == 0 )
		m_strHomePage = pstrValue;
	else
		CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefWebkitUI::SetInternVisible( bool bVisible /*= true*/ )
{
	CControlUI::SetInternVisible(bVisible);
	HWND hWnd = GetHostWnd();
	if (hWnd)
		::ShowWindow(hWnd, bVisible?SW_SHOW:SW_HIDE);
}

HWND CCefWebkitUI::GetHostWnd()
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefBrowserHost> pHost = m_pWebBrowser->GetHost();
		if (pHost.get())
			return pHost->GetWindowHandle();
	}
	return NULL;
}

wstring CCefWebkitUI::GetLoadingUrl()
{
	if (m_pWebBrowser.get())
	{
		CefString strUrl = m_pWebBrowser->GetMainFrame()->GetURL();
		if ( strUrl.empty() )
			return m_strHomePage;
		return strUrl.c_str();
	}
	return L"";
}

void CCefWebkitUI::Close()
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefBrowserHost> pHost = m_pWebBrowser->GetHost();
		if (pHost.get())
			pHost->CloseBrowser(false);
	}
}

void CCefWebkitUI::SetWebloadCallback( CCefWebCallback* pCallback, void* lpParam )
{
	m_lpCallbackData = lpParam;
	m_pCallback = pCallback;
}

CefRefPtr<CefBrowser> CCefWebkitUI::GetBrowser()
{
	return m_pWebBrowser;
}

void CCefWebkitUI::ExecuteJavascript( const wstring& strCode )
{
	if ( m_pWebBrowser.get() )
	{
		CefRefPtr<CefFrame> frame = m_pWebBrowser->GetMainFrame();
		if (frame.get())
		{
			CefString strCode(strCode.c_str()), strUrl(L"");
			frame->ExecuteJavaScript(strCode, strUrl, 0);
		}
	}
}

void CCefWebkitUI::Navigate( LPCTSTR lpUrl )
{
	if ( !m_pWebBrowser.get() || (NULL == lpUrl) )
		return ;
	CefRefPtr<CefFrame> pFrame = m_pWebBrowser->GetMainFrame();
	if ((_tcslen(lpUrl)>0) && pFrame.get())
		pFrame->LoadURL(lpUrl);
}

CefRefPtr<CefFrame> CCefWebkitUI::GetMainFrame()
{
	if (m_pWebBrowser.get())
	{
		return m_pWebBrowser->GetMainFrame();
	}
	return NULL;
}

void CCefWebkitUI::Reload()
{
	if (m_pWebBrowser.get())
	{
		m_pWebBrowser->Reload();
	}
}

void CCefWebkitUI::Refresh()
{
	if (m_pWebBrowser.get())
	{
		m_pWebBrowser->ReloadIgnoreCache();
	}
}

void CCefWebkitUI::GoBack()
{
	if (m_pWebBrowser.get() && m_pWebBrowser->CanGoBack())
		m_pWebBrowser->GoBack();
}

void CCefWebkitUI::GoForward()
{
	if (m_pWebBrowser.get() && m_pWebBrowser->CanGoForward())
		m_pWebBrowser->GoForward();
}

/*
参考 https://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11491#p20284
/*https://www.mycode.net.cn/language/cpp/2636.html
*/
bool CCefWebkitUI::SetZoomRatio(int ratio)
{
	if (ratio > 500 || ratio < 25) {
		//Chrome浏览器：最大缩放500%，最小25%
		return false;
	}
	m_zoomRatio = ratio;
	if (m_pWebBrowser.get()) {
		CefRefPtr<CefBrowserHost> host = m_pWebBrowser->GetHost();
		if (host.get()) {
			//缩放比例转换成zoom delta
			double delta = (double(m_zoomRatio - 100)) / 25.0;
			host->SetZoomLevel(delta);
			return true;
		}
	}
	return false;
}

// void CCefWebkitUI::CookieTest()
// {
// 	CefRefPtr<CefCookieManager> pManager = CefCookieManager::GetGlobalManager(NULL);
// 	if (pManager)
// 	{
// 		CefString url(L"pc.xxx.com");
// 		CefCookie cookie;
// 		LPCWSTR lpDomain = L"http://pc.xxx.com";
// 		LPCWSTR lpName = L"installList";
// 		LPCWSTR lpValue = L"{\"com.tencent.mqq\":1,\"com.tencent.mttlite\":1,\"com.tencent.xin\":1}";
// 		m_cefHandles->SetCookie(url, lpName, lpValue, lpDomain);
// 	}
// }

void CCefWebkitUI::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRePaint)
{
	HWND hWnd = GetHostWnd();
	if (hWnd && IsWindow(hWnd))
		::MoveWindow(hWnd, x, y, nWidth, nHeight, bRePaint);
}

void CCefWebkitUI::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
	m_pWebBrowser = browser;
}

void CCefWebkitUI::OnBrowserClosing(CefRefPtr<CefBrowser> browser)
{

}

void CCefWebkitUI::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{

}

void CCefWebkitUI::OnSetAddress(const std::wstring& url)
{
	if (m_pCallback)
		m_pCallback->OnSetAddress(this, m_lpCallbackData, url);
}

void CCefWebkitUI::OnSetTitle(const std::wstring& title)
{
	if (m_pCallback)
		m_pCallback->OnSetTitle(this, m_lpCallbackData, title);
}

void CCefWebkitUI::OnSetFullscreen(bool fullscreen)
{

}

void CCefWebkitUI::OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
{

}

void CCefWebkitUI::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions)
{

}

bool CCefWebkitUI::OnOpenNewUrl(const std::wstring& url)
{
	if (m_pCallback)
		return m_pCallback->OnOpenNewUrl(this, m_lpCallbackData, url);
	return false;
}

void CCefWebkitUI::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType transition_type)
{
	if (m_pCallback)
		m_pCallback->OnLoadStart(this, m_lpCallbackData);
}

void CCefWebkitUI::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode)
{
	if (m_pCallback)
		m_pCallback->OnLoadEnd(this, m_lpCallbackData, browser);
}

void CCefWebkitUI::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl)
{
	if (m_pCallback)
		m_pCallback->OnLoadError(this, m_lpCallbackData, frame);
}

void CCefWebkitUI::OnStatusMessage(const std::wstring& msg)
{
	if (m_pCallback)
		m_pCallback->OnStatusMessage(this, m_lpCallbackData, msg);
}

bool CCefWebkitUI::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, const CefString& url)
{
	if (m_pCallback)
		return m_pCallback->OnBeforeBrowse(this, m_lpCallbackData, browser, url);
	return false;
}

bool CCefWebkitUI::OnShowDevTools(CefRefPtr<CefBrowser> browser, CefWindowInfo& wndInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& setting)
{
	CWin32Wnd* pWnd = new CWin32Wnd;
	if (!pWnd->Create(CPaintManagerUI::GetInstance(), m_pManager->GetPaintWindow()))
		return false;
	pWnd->CenterWindow();
	m_hWndDev = pWnd->GetWnd();
	pWnd->Show();
	//return pWnd->CreateBrowserWnd(client, wndInfo, setting, L"");
	return pWnd->ShowBrowserHostWnd(browser, wndInfo, client, setting);
}

void CCefWebkitUI::OnCloseDevTools()
{
	if (IsWindow(m_hWndDev))
	{
		::PostMessage(m_hWndDev, WM_CLOSE, 0, 0);
		m_hWndDev = NULL;
	}
}

void CCefWebkitUI::OnAutoResize(const CefSize& new_size)
{

}

//CefRefPtr<CCefHandler> CCefWebkitUI::m_cefHandles;


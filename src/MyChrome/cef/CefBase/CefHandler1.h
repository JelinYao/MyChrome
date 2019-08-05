#pragma once
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include <list>
#include <vector>
#include <map>
#include <string>
using std::map;
using std::vector;
using std::wstring;

enum
{
	MENU_ID_COPY_LINK_URL = MENU_ID_USER_FIRST + 1,
};


class CCefHandler;
//网页加载状态的回调
class CCefWebLoadCallback
{
public:
	virtual void	OnLoadStart(void* lpParam)	= 0;
	virtual void	OnLoadEnd(void* lpParam, CefRefPtr<CefBrowser> browser)	= 0;
	virtual void	OnLoadError(void* lpParam, CefRefPtr<CefFrame> pFrame)	= 0;
	virtual void	OnTitleChange(void* lpParam, const wchar_t* lpTitle) = 0;
	virtual void	OnOpenNewUrl(void* lpParam, const wchar_t* lpUrl) = 0;
};

class CCefHandler :
	public CefClient,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler,
	public CefContextMenuHandler,
	public CefDownloadHandler
{
public:
	CCefHandler();
	virtual ~CCefHandler();
	//自定义方法
	CefRefPtr<CefBrowser>	GetBrowser() { return m_pBrowser; }
	CefRefPtr<CefFrame>		GetMainFram() { return m_pBrowser.get()?m_pBrowser->GetMainFrame():NULL; }
	HWND	GetBrowserHostWnd() { return m_pBrowser.get()?m_pBrowser->GetHost()->GetWindowHandle():NULL; }
	wstring GetLoadingUrl();
	void	Navigate(const wstring& strUrl);
	void	CreateBrowser(HWND hParentWnd, const RECT& rect, LPCTSTR lpUrl);
	void	SetWebloadCallback(CCefWebLoadCallback* pCallback, void* lpParam) { m_pWebloadCallback = pCallback; m_lpParam = lpParam; }

  //凡是继承了的处理功能都在这里返回this指针
	virtual CefRefPtr<CefDisplayHandler>		GetDisplayHandler()			{ return this; }
	virtual CefRefPtr<CefLifeSpanHandler>		GetLifeSpanHandler()		{ return this; }
	virtual CefRefPtr<CefLoadHandler>			GetLoadHandler()			{ return this; }
	virtual CefRefPtr<CefContextMenuHandler>	GetContextMenuHandler()		{ return this; }
	virtual CefRefPtr<CefDownloadHandler>		GetDownloadHandler()		{ return this; }
  // CefDisplayHandler methods:
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url);
  virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text);
  virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value);

  // CefLifeSpanHandler methods:
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
	  const CefString& target_url, const CefString& target_frame_name, const CefPopupFeatures& popupFeatures, \
	  CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access);
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);
  virtual bool DoClose(CefRefPtr<CefBrowser> browser);
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser);

  // CefLoadHandler methods:
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward);
//  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);
//  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode);
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl);

  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);
  //
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
	  CefRefPtr<CefRequest> request, bool is_redirect) 
  {
		  //return true;
		  return false;
  }

  virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
	  CefRefPtr<CefFrame> frame,
	  CefRefPtr<CefRequest> request) {
		  return false;
  }

  //菜单处理
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
	  CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model);

  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
	  CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags);

  //下载处理
  virtual void OnBeforeDownload( CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
	  const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback);

  virtual void OnDownloadUpdated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
	  CefRefPtr<CefDownloadItemCallback> callback);
private:
	void*	m_lpParam;
	CCefWebLoadCallback* m_pWebloadCallback;
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;
	CefRefPtr<CefBrowser>	m_pBrowser;
	static int	m_nBrowserCount;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefHandler);
	DISALLOW_COPY_AND_ASSIGN(CCefHandler);
};
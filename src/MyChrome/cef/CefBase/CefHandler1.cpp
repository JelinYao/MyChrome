//#include "StdAfx.h"
#include "CefHandler.h"
#include <sstream>
#include "util.h"
#include "../include/cef_app.h"



int		CCefHandler::m_nBrowserCount = 0;
CCefHandler::CCefHandler()
: m_pWebloadCallback(NULL)
, m_lpParam(NULL)
{

}

CCefHandler::~CCefHandler() 
{

}

void CCefHandler::CloseAllBrowsers(bool force_close) 
{
	if (!CefCurrentlyOn(TID_UI)) 
	{
		// Execute on the UI thread.
		//CefPostTask(TID_UI, NewCefRunnableMethod(this, &CCefHandler::CloseAllBrowsers, force_close));
		return;
	}

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}

wstring CCefHandler::GetLoadingUrl()
{
	CefRefPtr<CefFrame> pMainFram=GetMainFram();
	return pMainFram.get()?pMainFram->GetURL():L"";
}

void CCefHandler::Navigate( const wstring& strUrl )
{
	CefRefPtr<CefFrame> pMainFram=GetMainFram();
	if ( pMainFram.get() )
		pMainFram->LoadURL(strUrl.c_str());
}

void CCefHandler::CreateBrowser( HWND hParentWnd, const RECT& rect, LPCTSTR lpUrl )
{
	CefWindowInfo info;
	CefBrowserSettings settings;
//  	static wchar_t* pCharset = L"GB2312";
//  	settings.default_encoding.str = pCharset;
//  	settings.default_encoding.length = wcslen(pCharset);
	info.SetAsChild(hParentWnd, rect);
	//CefBrowserHost::CreateBrowser(info, this, m_strHomePage.c_str(), settings, NULL);
	CefBrowserHost::CreateBrowserSync(info, this, lpUrl, settings, NULL);
}

//****************************************************
//菜单加载接口
void CCefHandler::OnBeforeContextMenu( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
									  CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model )
{
	cef_context_menu_type_flags_t flag =   params->GetTypeFlags();
	if ( flag & CM_TYPEFLAG_PAGE )
	{
		model->SetLabel(MENU_ID_BACK, L"后退");
		model->SetLabel(MENU_ID_FORWARD, L"前进");
		model->SetLabel(MENU_ID_VIEW_SOURCE, L"查看源代码");
		model->SetLabel(MENU_ID_PRINT, L"打印");
		model->SetLabel(MENU_ID_RELOAD, L"刷新");
		model->SetLabel(MENU_ID_RELOAD_NOCACHE, L"强制刷新");
		model->SetLabel(MENU_ID_STOPLOAD, L"停止加载");
		model->SetLabel(MENU_ID_REDO, L"重复");
	}
	if ( (flag & CM_TYPEFLAG_EDITABLE) || (flag & CM_TYPEFLAG_SELECTION) )
	{
		model->SetLabel(MENU_ID_UNDO, L"撤销");
		//model->SetLabel(MENU_ID_REDO, L"重做");
		model->SetLabel(MENU_ID_CUT, L"剪切");
		model->SetLabel(MENU_ID_COPY, L"复制");
		model->SetLabel(MENU_ID_PASTE, L"粘贴");
		model->SetLabel(MENU_ID_DELETE, L"删除");
		model->SetLabel(MENU_ID_SELECT_ALL, L"全选");
	}
	if ( flag & CM_TYPEFLAG_LINK )
	{
		model->AddItem(MENU_ID_COPY_LINK_URL, L"复制链接地址");
	}
//	model->Remove(MENU_ID_PRINT);
//	model->Remove(MENU_ID_VIEW_SOURCE);
//	model->Remove(MENU_ID_BACK);
//	model->Remove(MENU_ID_FORWARD);
//	model->Remove(MENU_ID_REDO);
	if ( MENUITEMTYPE_SEPARATOR == model->GetTypeAt(0) )
		model->RemoveAt(0);
	model->AddItem(MENU_ID_RELOAD, L"刷新");
	//model->AddItem(MENU_ID_STOPLOAD, L"停止加载");
	if ( flag & CM_TYPEFLAG_MEDIA )
	{
		cef_context_menu_media_type_t mediaFlag = params->GetMediaType();
		if ( mediaFlag == CM_MEDIATYPE_IMAGE )
		{
			CefString strUrl = params->GetLinkUrl();
			OutputDebugString(strUrl.c_str());
			strUrl = params->GetSourceUrl();
			OutputDebugString(strUrl.c_str());
			model->AddItem(MENU_ID_USER_FIRST+1, L"复制图片");
		}
	}
}

bool CCefHandler::OnContextMenuCommand( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
									   CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags )
{
	switch ( command_id )
	{
	case MENU_ID_COPY_LINK_URL:
		{
			CefString strUrl = params->GetLinkUrl();
			if ( ::OpenClipboard(NULL) )
			{
				if ( ::EmptyClipboard() )
				{
					int nSize = strUrl.size()*2+1;
					HGLOBAL hMem = ::GlobalAlloc(GHND, nSize);	
					BYTE* pData=(BYTE*)::GlobalLock(hMem);
					memcpy_s(pData, nSize, strUrl.c_str(), nSize-1);
					pData[nSize-1] = '\0';
					::GlobalUnlock(hMem);
					::SetClipboardData(CF_UNICODETEXT, hMem);
					::CloseClipboard();
					::GlobalFree(hMem);
				}
			}
			return true;
		}
	}
	return false;
}


//****************************************************
//网页加载状态回调接口
// void CCefHandler::OnLoadStart( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame )
// {
// 	if ( m_pWebloadCallback )
// 		m_pWebloadCallback->OnLoadStart(this);
// }
// 
// void CCefHandler::OnLoadEnd( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode )
// {
// 	//CefRefPtr<CefV8Context> v8 = frame->GetV8Context();
// 	if ( m_pWebloadCallback )
// 		m_pWebloadCallback->OnLoadEnd(this);
// }

void CCefHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
							  ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) 
{
	REQUIRE_UI_THREAD();
	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;
	// Display a load error message.
// 	std::wstringstream ss;
// 	//std::wstring 
// 	ss << L"<html><body bgcolor=\"white\">"
// 		L"<h2>Failed to load URL " << std::wstring(failedUrl) <<
// 		L" with error " << std::wstring(errorText) << L" (" << errorCode <<
// 		L").</h2></body></html>"<<'\0';
// 	frame->LoadString(ss.str(), failedUrl);
// 	wstring strErrHtml = GetExePath() + L"Error\\error.htm";
// 	frame->LoadURL(strErrHtml.c_str());
	if ( m_pWebloadCallback )
		m_pWebloadCallback->OnLoadError(m_lpParam, frame);
}

//****************************************************
//状态改变回调接口
void CCefHandler::OnTitleChange( CefRefPtr<CefBrowser> browser, const CefString& title )
{
	if ( m_pWebloadCallback )
		m_pWebloadCallback->OnTitleChange(m_lpParam, title.c_str());
}

void CCefHandler::OnAddressChange( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url )
{

}

bool CCefHandler::OnTooltip( CefRefPtr<CefBrowser> browser, CefString& text )
{
	return false;
}

void CCefHandler::OnStatusMessage( CefRefPtr<CefBrowser> browser, const CefString& value )
{
	
}

//****************************************************
//网页生命周期回调接口
bool CCefHandler::OnBeforePopup( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
		const CefString& target_url, const CefString& target_frame_name, const CefPopupFeatures& popupFeatures, \
		CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, \
		bool* no_javascript_access )
{
	//ShellExecute(NULL, L"open", target_url.c_str(), NULL, NULL, SW_SHOW);
	//return false;//创建新窗口
	if ( m_pWebloadCallback )
		m_pWebloadCallback->OnOpenNewUrl(m_lpParam, target_url.c_str());
	return true; //禁止创建新的窗口
}

void CCefHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();
	// Add to the list of existing browsers.
	browser_list_.push_back(browser);
	AutoLock scopLock(this);
	m_pBrowser=browser;
	m_nBrowserCount++;
}

bool CCefHandler::DoClose(CefRefPtr<CefBrowser> browser) 
{
	REQUIRE_UI_THREAD();
	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	HWND hWnd = browser->GetHost()->GetWindowHandle();
	SetParent(hWnd, NULL);
	return false;
}

void CCefHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) 
{
	REQUIRE_UI_THREAD();
	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}
// 	if (browser_list_.empty()) {
// 		// All browser windows have closed. Quit the application message loop.
// 		CefQuitMessageLoop();
// 	}
}

void CCefHandler::OnBeforeDownload( CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
		const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback )
{
	CefString str;
	callback->Continue(str, true);
}

void CCefHandler::OnDownloadUpdated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
		CefRefPtr<CefDownloadItemCallback> callback )
{
// 	callback->Cancel();
// 	CefString strUrl = download_item->GetURL();
// 	CefRefPtr<CefFrame> pFrame = browser->GetMainFrame();
// 	if ( pFrame.get() )
// 		pFrame->LoadURL(strUrl.c_str());
// 	MessageBox(NULL, strUrl.c_str(), NULL, 0);
	//ShellExecute(NULL, L"open", strUrl.c_str(), NULL, NULL, SW_SHOW);
//	int i = 0;
	//if ( download_item->IsComplete() )
	//	MessageBox(NULL, L"下载完毕", NULL, 0);
}

void CCefHandler::OnLoadingStateChange( CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward )
{
	if ( m_pWebloadCallback )
	{
		if ( isLoading )
			m_pWebloadCallback->OnLoadStart(m_lpParam);
		else
			m_pWebloadCallback->OnLoadEnd(m_lpParam, browser);
	}
}

// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "CefHandler.h"
#include <stdio.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include "include/base/cef_bind.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_closure_task.h"
#include "main_message_loop.h"
#include "include/cef_runnable.h"
#include "PdfPrintCallbackEx.h"

#if defined(OS_WIN)
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

namespace {

	// Custom menu command Ids.
	enum client_menu_ids {
		CLIENT_ID_SHOW_DEVTOOLS = MENU_ID_USER_FIRST,
		CLIENT_ID_CLOSE_DEVTOOLS,
		CLIENT_ID_PRINT_TO_PDF,
	};

	// Musr match the value in client_renderer.cc.
	const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

	std::string GetTimeString(const CefTime& value) {
		if (value.GetTimeT() == 0)
			return "Unspecified";

		static const char* kMonths[] = {
			"January", "February", "March", "April", "May", "June", "July", "August",
			"September", "October", "November", "December"
		};
		std::string month;
		if (value.month >= 1 && value.month <= 12)
			month = kMonths[value.month - 1];
		else
			month = "Invalid";

		std::stringstream ss;
		ss << month << " " << value.day_of_month << ", " << value.year << " " <<
			std::setfill('0') << std::setw(2) << value.hour << ":" <<
			std::setfill('0') << std::setw(2) << value.minute << ":" <<
			std::setfill('0') << std::setw(2) << value.second;
		return ss.str();
	}

	std::string GetBinaryString(CefRefPtr<CefBinaryValue> value) {
		if (!value.get())
			return "&nbsp;";

		// Retrieve the value.
		const size_t size = value->GetSize();
		std::string src;
		src.resize(size);
		value->GetData(const_cast<char*>(src.data()), size, 0);

		// Encode the value.
		return CefBase64Encode(src.data(), src.size());
	}

	std::string GetCertStatusString(cef_cert_status_t status) {
#define FLAG(flag) if (status & flag) result += std::string(#flag) + "<br/>"
		std::string result;

		FLAG(CERT_STATUS_COMMON_NAME_INVALID);
		FLAG(CERT_STATUS_DATE_INVALID);
		FLAG(CERT_STATUS_AUTHORITY_INVALID);
		FLAG(CERT_STATUS_NO_REVOCATION_MECHANISM);
		FLAG(CERT_STATUS_UNABLE_TO_CHECK_REVOCATION);
		FLAG(CERT_STATUS_REVOKED);
		FLAG(CERT_STATUS_INVALID);
		FLAG(CERT_STATUS_WEAK_SIGNATURE_ALGORITHM);
		FLAG(CERT_STATUS_NON_UNIQUE_NAME);
		FLAG(CERT_STATUS_WEAK_KEY);
		FLAG(CERT_STATUS_PINNED_KEY_MISSING);
		FLAG(CERT_STATUS_NAME_CONSTRAINT_VIOLATION);
		FLAG(CERT_STATUS_VALIDITY_TOO_LONG);
		FLAG(CERT_STATUS_IS_EV);
		FLAG(CERT_STATUS_REV_CHECKING_ENABLED);
		FLAG(CERT_STATUS_SHA1_SIGNATURE_PRESENT);
		FLAG(CERT_STATUS_CT_COMPLIANCE_FAILED);

		if (result.empty())
			return "&nbsp;";
		return result;
	}
};


int CCefHandler::m_nBrowserCount = 0;
CCefHandler::CCefHandler(Delegate* delegate, const std::string& startup_url)
    : startup_url_(startup_url),
    download_favicon_images_(false),
    delegate_(delegate)
{
  resource_manager_ = new CefResourceManager();
}

CCefHandler::~CCefHandler()
{

}

void CCefHandler::DetachDelegate()
{
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::Bind(&CCefHandler::DetachDelegate, this));
    return;
  }
  DCHECK(delegate_);
  delegate_ = NULL;
}

bool CCefHandler::SetCookie(const CefString& url, const CefString& name, const CefString& value, const CefString& domain)
{
	bool bRet = false;
	CefRefPtr<CefCookieManager> pManager = CefCookieManager::GetGlobalManager(NULL);
	if (pManager)
	{
		CefCookie cookie;
		CefString(&cookie.name) = name;
		CefString(&cookie.value) = value;
		CefString(&cookie.domain) = domain;
		cef_string_set(L"/", 1, &cookie.path, 1);
		cookie.has_expires = false;
		bRet = pManager->SetCookie(url, cookie, NULL);
// 		CefRefPtr<CefSetCookieCallback> cb = NULL;
// 		bRet = CefPostTask(TID_IO, NewCefRunnableMethod(pManager.get(), &CefCookieManager::SetCookie, url, cookie, cb));
	}
	return bRet;
}

bool CCefHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) 
{
  CEF_REQUIRE_UI_THREAD();
  return false;
}

void CCefHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) {
  CEF_REQUIRE_UI_THREAD();

  if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
    // Add a separator if the menu already has items.
    if (model->GetCount() > 0)
		model->AddSeparator(); 
	int nIndex = model->GetIndexOf(MENU_ID_FORWARD);
	//model->AddItem(MENU_ID_RELOAD, L"&刷新");
	model->InsertItemAt(nIndex+1, MENU_ID_RELOAD_NOCACHE, L"&重新加载");
	model->InsertItemAt(nIndex + 2, MENU_ID_STOPLOAD, L"&停止加载");
	model->SetEnabled(MENU_ID_STOPLOAD, browser->IsLoading());
    // Add DevTools items to all context menus.
    model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, L"&审查元素");
	model->AddItem(CLIENT_ID_PRINT_TO_PDF, L"&保存为PDF");
    //model->AddItem(CLIENT_ID_CLOSE_DEVTOOLS, "Close DevTools");
    //model->AddSeparator();
  }
}

bool CCefHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) {
  CEF_REQUIRE_UI_THREAD();

  switch (command_id) 
  {
  case CLIENT_ID_SHOW_DEVTOOLS:
		//CloseDevTools(browser);
		ShowDevTools(browser, CefPoint());// CefPoint(params->GetXCoord(), params->GetYCoord()))
		return true;
    case CLIENT_ID_CLOSE_DEVTOOLS:
		CloseDevTools(browser);
		return true;
	case CLIENT_ID_PRINT_TO_PDF:
		PrintToPDF(browser);
		return true;
// 	case MENU_ID_PRINT:
// 	{
// 						  CefRefPtr<CefFrame> mainFrame = browser->GetMainFrame();
// 						  mainFrame->GetBrowser()->GetHost()->Print();
// 	}
// 		return true;
    default:  // Allow default handling, if any.
		return false;
  }
}

void CCefHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
  CEF_REQUIRE_UI_THREAD();
  // Only update the address for the main (top-level) frame.
  if (frame->IsMain())
    NotifyAddress(url);
}

void CCefHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
  CEF_REQUIRE_UI_THREAD();
  NotifyTitle(title);
}

void CCefHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) {
  CEF_REQUIRE_UI_THREAD();
  NotifyFullscreen(fullscreen);
}

void CCefHandler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls)
{

}

void CCefHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
	CEF_REQUIRE_UI_THREAD();
	if (!value.empty())
		NotifyStatusMessage(value);
}

void CCefHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  // Continue the download and show the "Save As" dialog.
  callback->Continue(suggested_name, true);
}

void CCefHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

//   if (download_item->IsComplete()) {
//     test_runner::Alert(
//         browser,
//         "File \"" + download_item->GetFullPath().ToString() +
//         "\" downloaded successfully.");
//   }
}

bool CCefHandler::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) {
  CEF_REQUIRE_UI_THREAD();

  // Forbid dragging of link URLs.
//   if (mask & DRAG_OPERATION_LINK)
//     return true;

  return false;
}

void CCefHandler::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) {
  CEF_REQUIRE_UI_THREAD();

  NotifyDraggableRegions(regions);
}

bool CCefHandler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  // Allow geolocation access from all websites.
  callback->Continue(true);
  return true;
}

bool CCefHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) {
  CEF_REQUIRE_UI_THREAD();

  if (!event.focus_on_editable_field && event.windows_key_code == 0x20) {
    // Special handling for the space character when an input element does not
    // have focus. Handling the event in OnPreKeyEvent() keeps the event from
    // being processed in the renderer. If we instead handled the event in the
    // OnKeyEvent() method the space key would cause the window to scroll in
    // addition to showing the alert box.
//     if (event.type == KEYEVENT_RAWKEYDOWN)
//       test_runner::Alert(browser, "You pressed the space bar!");
    return true;
  }

  return false;
}

bool CCefHandler::OnBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
    bool* no_javascript_access) 
{
	CEF_REQUIRE_IO_THREAD();
  // Return true to cancel the popup window.
//   return !CreatePopupWindow(browser, false, popupFeatures, windowInfo, client,
//                             settings);
	if (delegate_ && delegate_->OnOpenNewUrl(target_url))
		return true;
	return false;
}

void CCefHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	m_nBrowserCount++;
  // Disable mouse cursor change if requested via the command-line flag.
//   if (mouse_cursor_change_disabled_)
//     browser->GetHost()->SetMouseCursorChangeDisabled(true);

	NotifyBrowserCreated(browser);
}

bool CCefHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	m_nBrowserCount--;
	if (m_nBrowserCount>0)
	{
		HWND hWnd = browser->GetHost()->GetWindowHandle();
		SetParent(hWnd, NULL);
	}
	return false;
}

void CCefHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  //browser_count_--;
}

void CCefHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) 
{
	CEF_REQUIRE_UI_THREAD();
	NotifyLoadingState(isLoading, canGoBack, canGoForward);
}

void CCefHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();
	NotifyLoadStart(browser, frame);
}

void CCefHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();
	NotifyLoadEnd(browser, frame, httpStatusCode);
}

void CCefHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) 
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	//   if (errorCode == ERR_ABORTED)
	//     return;

	// Don't display an error for external protocols that we allow the OS to
	// handle. See OnProtocolExecution().
	if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
	std::string urlStr = frame->GetURL();
	if (urlStr.find("spotify:") == 0)
		return;
	}
	// Load the error page.
	NotifyLoadError(browser, frame, errorCode, errorText, failedUrl);
}

bool CCefHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) 
{
	CEF_REQUIRE_UI_THREAD();
	CefString strUrl = request->GetURL();
	if (delegate_)
		return delegate_->OnBeforeBrowse(browser, strUrl);
	return false;
}

bool CCefHandler::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, CefRequestHandler::WindowOpenDisposition target_disposition, bool user_gesture) 
{
	if (target_disposition == WOD_NEW_BACKGROUND_TAB ||
		target_disposition == WOD_NEW_FOREGROUND_TAB) 
	{
	// Handle middle-click and ctrl + left-click by opening the URL in a new
	// browser window.
	//     MainContext::Get()->GetRootWindowManager()->CreateRootWindow(
	//         true, is_osr(), CefRect(), target_url);
	return true;
	}

	// Open the URL in the current browser window.
	return false;
}

cef_return_value_t CCefHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) 
{
	CEF_REQUIRE_IO_THREAD();
// 	CefRequest::HeaderMap hm;
// 	request->GetHeaderMap(hm);
// 	auto itor = hm.find(L"User-Agent");
// 	if (itor != hm.end())
// 	{
// 		itor->second = L"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (BcYun, like Gecko) Maxthon/4.9.3.1000 Chrome/39.0.2146.0 Safari/537.36";
// 	}
// 	else
// 	{
// 		hm.insert(std::make_pair(L"User-Agent", L"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (BcYun, like Gecko) Maxthon/4.9.3.1000 Chrome/39.0.2146.0 Safari/537.36"));
// 	}
// 	itor = hm.find(L"Accept-Language");
// 	if (itor != hm.end())
// 	{
// 		itor->second = L"1.5";
// 	}
// 	else
// 	{
// 		hm.insert(std::make_pair(L"Accept-Language", L"1.5"));
// 	}
// 	request->SetHeaderMap(hm);
	return resource_manager_->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> CCefHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) 
{
	CEF_REQUIRE_IO_THREAD();
	return resource_manager_->GetResourceHandler(browser, frame, request);
}

bool CCefHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefRequestCallback> callback) 
{
	CEF_REQUIRE_IO_THREAD();

	static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

	// Grant the quota request if the size is reasonable.
	callback->Continue(new_size <= max_size);
	return true;
}

void CCefHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution) 
{
	CEF_REQUIRE_UI_THREAD();

	std::string urlStr = url;
	// Allow OS execution of Spotify URIs.
	if (urlStr.find("spotify:") == 0)
	allow_os_execution = true;
}

bool CCefHandler::OnCertificateError(CefRefPtr<CefBrowser> browser,ErrorCode cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) 
{
	CEF_REQUIRE_UI_THREAD();

	CefRefPtr<CefSSLCertPrincipal> subject = ssl_info->GetSubject();
	CefRefPtr<CefSSLCertPrincipal> issuer = ssl_info->GetIssuer();

	// Build a table showing certificate information. Various types of invalid
	// certificates can be tested using https://badssl.com/.
	std::stringstream ss;
	ss << "X.509 Certificate Information:"
		"<table border=1><tr><th>Field</th><th>Value</th></tr>" <<
		"<tr><td>Subject</td><td>" <<
			(subject.get() ? subject->GetDisplayName().ToString() : "&nbsp;") <<
			"</td></tr>"
		"<tr><td>Issuer</td><td>" <<
			(issuer.get() ? issuer->GetDisplayName().ToString() : "&nbsp;") <<
			"</td></tr>"
		"<tr><td>Serial #*</td><td>" <<
			GetBinaryString(ssl_info->GetSerialNumber()) << "</td></tr>"
		"<tr><td>Status</td><td>" <<
			GetCertStatusString(ssl_info->GetCertStatus()) << "</td></tr>"
		"<tr><td>Valid Start</td><td>" <<
			GetTimeString(ssl_info->GetValidStart()) << "</td></tr>"
		"<tr><td>Valid Expiry</td><td>" <<
			GetTimeString(ssl_info->GetValidExpiry()) << "</td></tr>";

	CefSSLInfo::IssuerChainBinaryList der_chain_list;
	CefSSLInfo::IssuerChainBinaryList pem_chain_list;
	ssl_info->GetDEREncodedIssuerChain(der_chain_list);
	ssl_info->GetPEMEncodedIssuerChain(pem_chain_list);
	DCHECK_EQ(der_chain_list.size(), pem_chain_list.size());

	der_chain_list.insert(der_chain_list.begin(), ssl_info->GetDEREncoded());
	pem_chain_list.insert(pem_chain_list.begin(), ssl_info->GetPEMEncoded());

	for (size_t i = 0U; i < der_chain_list.size(); ++i) {
	ss << "<tr><td>DER Encoded*</td>"
			"<td style=\"max-width:800px;overflow:scroll;\">" <<
				GetBinaryString(der_chain_list[i]) << "</td></tr>"
			"<tr><td>PEM Encoded*</td>"
			"<td style=\"max-width:800px;overflow:scroll;\">" <<
				GetBinaryString(pem_chain_list[i]) << "</td></tr>";
	}

	ss << "</table> * Displayed value is base64 encoded.";

	// Load the error page.
	return false;  // Cancel the request.
}

void CCefHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) 
{
	CEF_REQUIRE_UI_THREAD();

	// Don't reload if there's no start URL, or if the crash URL was specified.
	if (startup_url_.empty() || startup_url_ == "chrome://crash")
	return;

	CefRefPtr<CefFrame> frame = browser->GetMainFrame();
	std::string url = frame->GetURL();

	// Don't reload if the termination occurred before any URL had successfully
	// loaded.
	if (url.empty())
	return;

	std::string start_url = startup_url_;

	// Convert URLs to lowercase for easier comparison.
	std::transform(url.begin(), url.end(), url.begin(), tolower);
	std::transform(start_url.begin(), start_url.end(), start_url.begin(),
					tolower);

	// Don't reload the URL that just resulted in termination.
	if (url.find(start_url) == 0)
	return;

	frame->LoadURL(startup_url_);
}

void CCefHandler::ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at) 
{
	CefWindowInfo windowInfo;
	CefBrowserSettings settings;
#if defined(OS_WIN)
	windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), L"DevTool");
#endif
	browser->GetHost()->ShowDevTools(windowInfo, browser->GetHost()->GetClient(), settings, inspect_element_at);
}

void CCefHandler::CloseDevTools(CefRefPtr<CefBrowser> browser) 
{
	CefRefPtr<CefBrowserHost> pHost = browser->GetHost();
	if (pHost.get())
		pHost->CloseDevTools();
}

void CCefHandler::NotifyBrowserCreated(CefRefPtr<CefBrowser> browser) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::Bind(&CCefHandler::NotifyBrowserCreated, this, browser));
    return;
  }

  if (delegate_)
    delegate_->OnBrowserCreated(browser);
}

void CCefHandler::NotifyAddress(const CefString& url) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::Bind(&CCefHandler::NotifyAddress, this, url));
    return;
  }

  if (delegate_)
    delegate_->OnSetAddress(url);
}

void CCefHandler::NotifyTitle(const CefString& title) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyTitle, this, title));
    return;
  }

  if (delegate_)
    delegate_->OnSetTitle(title);
}

void CCefHandler::NotifyFullscreen(bool fullscreen) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyFullscreen, this, fullscreen));
    return;
  }
  if (delegate_)
    delegate_->OnSetFullscreen(fullscreen);
}

void CCefHandler::NotifyStatusMessage(const CefString& msg)
{
	if (!CURRENTLY_ON_MAIN_THREAD()) {
		// Execute this method on the main thread.
		MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyStatusMessage, this, msg));
		return;
	}
	if (delegate_)
		delegate_->OnStatusMessage(msg);
}

void CCefHandler::NotifyLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
{
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyLoadingState, this, isLoading, canGoBack, canGoForward));
    return;
  }

  if (delegate_)
    delegate_->OnSetLoadingState(isLoading, canGoBack, canGoForward);
}

void CCefHandler::NotifyLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	if (!CURRENTLY_ON_MAIN_THREAD()) {
		// Execute this method on the main thread.
		MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyLoadStart, this, browser, frame));
		return;
	}
	if (frame->IsMain() && delegate_)
		delegate_->OnLoadStart(browser, frame);
}

void CCefHandler::NotifyLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode)
{
	if (!CURRENTLY_ON_MAIN_THREAD()) {
		// Execute this method on the main thread.
		MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyLoadEnd, this, browser, frame, nHttpCode));
		return;
	}
	
	if (frame->IsMain() && delegate_)
		delegate_->OnLoadEnd(browser, frame, nHttpCode);
}

void CCefHandler::NotifyLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	if (!CURRENTLY_ON_MAIN_THREAD()) {
		// Execute this method on the main thread.
		MAIN_POST_CLOSURE(base::Bind(&CCefHandler::NotifyLoadError, this, browser, frame, errorCode, errorText, failedUrl));
		return;
	}
	if (frame->IsMain() && delegate_)
		delegate_->OnLoadError(browser, frame, errorCode, errorText, failedUrl);
}

void CCefHandler::NotifyDraggableRegions(const std::vector<CefDraggableRegion>& regions) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::Bind(&CCefHandler::NotifyDraggableRegions, this, regions));
    return;
  }

  if (delegate_)
    delegate_->OnSetDraggableRegions(regions);
}

void CCefHandler::CreateBrowser(HWND hParentWnd, const RECT& rect, LPCTSTR lpUrl)
{
	CefWindowInfo info;
	CefBrowserSettings settings;
	//settings.webgl = STATE_ENABLED;
	static wchar_t* pCharset = L"UTF-8";
 	settings.default_encoding.str = pCharset;
	settings.default_encoding.length = wcslen(pCharset);
	info.SetAsChild(hParentWnd, rect);
	//info.SetAsWindowless(hParentWnd, false);
	//bool bRet = CefBrowserHost::CreateBrowser(info, this, L"https://www.baidu.com", settings, NULL);
	CefBrowserHost::CreateBrowserSync(info, this, lpUrl, settings, NULL);
}

void CCefHandler::PrintToPDF(CefRefPtr<CefBrowser> browser) 
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CCefHandler::PrintToPDF, this, browser));
		return;
	}
	new CPdfPrintCallbackEx(browser);
}
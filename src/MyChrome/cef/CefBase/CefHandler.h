// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_BROWSER_CLIENT_HANDLER_H_
#define CEF_TESTS_CEFCLIENT_BROWSER_CLIENT_HANDLER_H_
#pragma once

#include <set>
#include <string>
#include "Delegate.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"
#include "include/cef_base.h"
#include "include/cef_context_menu_handler.h"
#include "include/cef_dialog_handler.h"
#include "include/cef_display_handler.h"
#include "include/cef_download_handler.h"
#include "include/cef_drag_handler.h"
#include "include/cef_find_handler.h"
#include "include/cef_focus_handler.h"
#include "include/cef_geolocation_handler.h"
#include "include/cef_jsdialog_handler.h"
#include "include/cef_keyboard_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_process_message.h"
#include "include/cef_render_handler.h"
#include "include/cef_request_handler.h"


class ClientDownloadImageCallback;

// Client handler abstract base class. Provides common functionality shared by
// all concrete client handler implementations.
class CCefHandler : public CefClient,
                      public CefContextMenuHandler,
                      public CefDisplayHandler,
                      public CefDownloadHandler,
                      public CefDragHandler,
                      public CefGeolocationHandler,
                      public CefKeyboardHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler,
                      public CefRequestHandler
					  //public CefRenderHandler
{
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
 public:
	// Constructor may be called on any thread.
	// |delegate| must outlive this object or DetachDelegate() must be called.
	CCefHandler(Delegate* delegate, const std::string& startup_url);
	~CCefHandler();
	// Implement this interface to receive notification of ClientHandler
	// events. The methods of this class will be called on the main thread.
	// Returns the Delegate.
	Delegate* delegate() const			{ return delegate_; }
	// Returns the startup URL.
	std::string startup_url() const		{ return startup_url_; }
	int	GetBrowserCount()				{ return m_nBrowserCount; }
	void CreateBrowser(HWND hParentWnd, const RECT& rect, LPCTSTR lpUrl);
	// This object may outlive the Delegate object so it's necessary for the
	// Delegate to detach itself before destruction.
	void DetachDelegate();
	bool SetCookie(const CefString& url, const CefString& name, const CefString& value, const CefString& domain);

protected:
	// CefClient methods
	CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE		{ return this; }
	CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE				{ return this; }
	CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE				{ return this; }
	CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE						{ return this; }
	CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE		{ return this; }
	CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE				{ return this; }
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE				{ return this; }
	CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE						{ return this; }
	CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE				{ return this; }
	//CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE					{ return this; }
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

	// CefContextMenuHandler methods
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;

	bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;

	// CefDisplayHandler methods
	void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
	void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
	void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
	void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) OVERRIDE;
	void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value);
	// CefDownloadHandler methods
	void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;

	void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

	// CefDragHandler methods
	bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) OVERRIDE;

	void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) OVERRIDE;

	// CefGeolocationHandler methods
	bool OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

	// CefKeyboardHandler methods
	bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) OVERRIDE;

	// CefLifeSpanHandler methods
	bool OnBeforePopup(
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
		bool* no_javascript_access) OVERRIDE;
	void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods
	void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) OVERRIDE;
	void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
	void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
	void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

	// CefRequestHandler methods
	bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) OVERRIDE;
	bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, CefRequestHandler::WindowOpenDisposition target_disposition, bool user_gesture) OVERRIDE;
	cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) OVERRIDE;
	CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) OVERRIDE;
	bool OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefRequestCallback> callback) OVERRIDE;
	void OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution) OVERRIDE;
	bool OnCertificateError(CefRefPtr<CefBrowser> browser, ErrorCode cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) OVERRIDE;
	void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) OVERRIDE;

	void PrintToPDF(CefRefPtr<CefBrowser> browser);

	// Returns the number of browsers currently using this handler. Can only be
	// called on the CEF UI thread.
	int GetBrowserCount() const;

	// Show a new DevTools popup window.
	void ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at);

	// Close the existing DevTools popup window, if any.
	void CloseDevTools(CefRefPtr<CefBrowser> browser);
	// Set/get whether the client should download favicon images. Only safe to
	// call immediately after client creation or on the browser process UI thread.
	bool download_favicon_images() const { return download_favicon_images_; }
	void set_download_favicon_images(bool allow) { download_favicon_images_ = allow; }

// 	virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect);
// 	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);
// 	virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY);
// 	virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info);
// 	virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height);

private:
	friend class ClientDownloadImageCallback;

	void NotifyBrowserCreated(CefRefPtr<CefBrowser> browser);
	void NotifyAddress(const CefString& url);
	void NotifyTitle(const CefString& title);
	void NotifyFullscreen(bool fullscreen);
	void NotifyStatusMessage(const CefString& msg);
	void NotifyLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
	void NotifyLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);
	void NotifyLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode);
	void NotifyLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl);
	void NotifyDraggableRegions(const std::vector<CefDraggableRegion>& regions);

	// THREAD SAFE MEMBERS
	// The following members may be accessed from any thread.

	// The startup URL.
	const std::string startup_url_;

	// True if mouse cursor change is disabled.
	bool mouse_cursor_change_disabled_;

	// True if Favicon images should be downloaded.
	bool download_favicon_images_;

	// Manages the registration and delivery of resources.
	CefRefPtr<CefResourceManager> resource_manager_;

	// MAIN THREAD MEMBERS
	// The following members will only be accessed on the main thread. This will
	// be the same as the CEF UI thread except when using multi-threaded message
	// loop mode on Windows.
	Delegate* delegate_;

	// The current number of browsers using this handler.
	static int	m_nBrowserCount;

	IMPLEMENT_REFCOUNTING(CCefHandler);
	DISALLOW_COPY_AND_ASSIGN(CCefHandler);
	//IMPLEMENT_LOCKING(CCefHandler);
};


#endif  // CEF_TESTS_CEFCLIENT_BROWSER_CLIENT_HANDLER_H_

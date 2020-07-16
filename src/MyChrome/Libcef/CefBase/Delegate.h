#pragma once
#include <string>
#include "include/cef_client.h"


class Delegate
{
public:
	// Called when the browser is created.
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) {}

	// Set the window URL address.
	virtual void OnSetAddress(const std::wstring& url) {}

	// Set the window title.
	virtual void OnSetTitle(const std::wstring& title) {}

	// Set fullscreen mode.
	virtual void OnSetFullscreen(bool fullscreen) {}

	// Set the loading state.
	virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) {}

	// Set the draggable regions.
	virtual void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) {}

	//open new url
	virtual bool OnOpenNewUrl(const std::wstring& url) { return false; }

	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, const std::wstring& url) { return false; }	//return true取消打开

	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {}

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode) {}

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl) {}

	virtual void OnStatusMessage(const std::wstring& msg) {}

protected:
	virtual ~Delegate() {}
};
// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once
#ifndef CEF_TESTS_DELEGATE_H_
#define CEF_TESTS_DELEGATE_H_
#include "include/cef_browser.h"
#include "include/cef_menu_model.h"


class Delegate {
public:
	// Called when the browser is created.
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) {}

	// Called when the browser is closing.
	virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) {}

	// Called when the browser has been closed.
	virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) {}

	// Set the window URL address.
	virtual void OnSetAddress(const std::wstring& url) {}

	// Set the window title.
	virtual void OnSetTitle(const std::wstring& title) {}

	// Set the Favicon image.
	virtual void OnSetFavicon(CefRefPtr<CefImage> image) {}

	// Set fullscreen mode.
	virtual void OnSetFullscreen(bool fullscreen) {}

	// Auto-resize contents.
	virtual void OnAutoResize(const CefSize& new_size) {}

	// Set the loading state.
	virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) {}

	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType transition_type) {}

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {}

	// Set the draggable regions.
	virtual void OnSetDraggableRegions(
		const std::vector<CefDraggableRegion>& regions) {}

	// Set focus to the next/previous control.
	virtual void OnTakeFocus(bool next) {}

	// Called on the UI thread before a context menu is displayed.
	virtual void OnBeforeContextMenu(CefRefPtr<CefMenuModel> model) {}

	virtual bool OnShowDevTools(CefRefPtr<CefBrowser> browser, CefWindowInfo& wndInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& setting) { return false; }

	virtual void OnCloseDevTools() {}

	virtual bool OnOpenNewUrl(const std::wstring& url) { return false; }

	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, const CefString& url) { return false; }

protected:
	virtual ~Delegate() {}
  };


#endif
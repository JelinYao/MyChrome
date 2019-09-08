// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_APP_RENDER_DELEGATE_H_
#define CEF_TESTS_APP_RENDER_DELEGATE_H_
#pragma once
#include "include/cef_browser.h"
#include "include/cef_menu_model.h"
#include "include/cef_load_handler.h"
#include "include/cef_v8.h"


namespace client {
	class ClientAppRenderer;
	class AppRenderDelegate : public virtual CefBaseRefCounted {
	public:
		virtual void OnRenderThreadCreated(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefListValue> extra_info) {}

		virtual void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) {}

		virtual void OnBrowserCreated(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDictionaryValue> extra_info) {}

		virtual void OnBrowserDestroyed(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser) {}

		virtual CefRefPtr<CefLoadHandler> GetLoadHandler(
			CefRefPtr<ClientAppRenderer> app) {
			return NULL;
		}

		virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}

		virtual void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}

		virtual void OnUncaughtException(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context,
			CefRefPtr<CefV8Exception> exception,
			CefRefPtr<CefV8StackTrace> stackTrace) {}

		virtual void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefDOMNode> node) {}

		// Called when a process message is received. Return true if the message was
		// handled and should not be passed on to other handlers. Delegates
		// should check for unique message names to avoid interfering with each
		// other.
		virtual bool OnProcessMessageReceived(
			CefRefPtr<ClientAppRenderer> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) {
			return false;
		}
	};
}

#endif
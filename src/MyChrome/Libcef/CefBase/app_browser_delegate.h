// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_APP_BROWSER_DELEGATE_H_
#define CEF_TESTS_APP_BROWSER_DELEGATE_H_
#pragma once
#include "include/cef_base.h"
#include "include/cef_command_line.h"
#include "include/cef_values.h"


namespace client {
	// Interface for browser delegates. All Delegates must be returned via
	// CreateDelegates. Do not perform work in the Delegate
	// constructor. See CefBrowserProcessHandler for documentation.
	class ClientAppBrowser;
	class AppBrowserDelegate : public virtual CefBaseRefCounted {
	public:
		virtual void OnBeforeCommandLineProcessing(
			CefRefPtr<ClientAppBrowser> app,
			CefRefPtr<CefCommandLine> command_line) {}

		virtual void OnContextInitialized(CefRefPtr<ClientAppBrowser> app) {}

		virtual void OnBeforeChildProcessLaunch(
			CefRefPtr<ClientAppBrowser> app,
			CefRefPtr<CefCommandLine> command_line) {}

		virtual void OnRenderProcessThreadCreated(
			CefRefPtr<ClientAppBrowser> app,
			CefRefPtr<CefListValue> extra_info) {}
	};
}

#endif
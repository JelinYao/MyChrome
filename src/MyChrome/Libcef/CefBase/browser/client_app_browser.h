// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_SHARED_BROWSER_CLIENT_APP_BROWSER_H_
#define CEF_TESTS_SHARED_BROWSER_CLIENT_APP_BROWSER_H_
#pragma once

#include <set>
#include "common/client_app.h"
#include "app_browser_delegate.h"

namespace client {

// Client app implementation for the browser process.
class ClientAppBrowser : public ClientApp, public CefBrowserProcessHandler {
 public:
 

  ClientAppBrowser(CefRefPtr<AppBrowserDelegate> callback = nullptr);

 private:

  // CefApp methods.
  void OnBeforeCommandLineProcessing(
      const CefString& process_type,
      CefRefPtr<CefCommandLine> command_line) OVERRIDE;
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
    return this;
  }

  // CefBrowserProcessHandler methods.
  void OnContextInitialized() OVERRIDE;
  void OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) OVERRIDE;
  void OnRenderProcessThreadCreated(
      CefRefPtr<CefListValue> extra_info) OVERRIDE;
//   CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE {
//     return print_handler_;
//   }
  void OnScheduleMessagePumpWork(int64 delay) OVERRIDE;

  // Set of supported Delegates.
  CefRefPtr<AppBrowserDelegate> delegates_;

  //CefRefPtr<CefPrintHandler> print_handler_;

  IMPLEMENT_REFCOUNTING(ClientAppBrowser);
  DISALLOW_COPY_AND_ASSIGN(ClientAppBrowser);
};

}  // namespace client

#endif  // CEF_TESTS_SHARED_BROWSER_CLIENT_APP_BROWSER_H_

// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_SHARED_RENDERER_CLIENT_APP_RENDERER_H_
#define CEF_TESTS_SHARED_RENDERER_CLIENT_APP_RENDERER_H_
#pragma once

#include <set>
#include "common/client_app.h"
#include "app_render_delegate.h"


namespace client {

// Client app implementation for the renderer process.
class ClientAppRenderer : public ClientApp, public CefRenderProcessHandler {
 public:
  // Interface for renderer delegates. All Delegates must be returned via
  // CreateDelegates. Do not perform work in the Delegate
  // constructor. See CefRenderProcessHandler for documentation.

  ClientAppRenderer(CefRefPtr<AppRenderDelegate> callback = nullptr);

 private:
  // CefApp methods.
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
    return this;
  }

  // CefRenderProcessHandler methods.
  void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
  void OnWebKitInitialized() OVERRIDE;
  void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefDictionaryValue> extra_info) OVERRIDE;
  void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
  CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
  void OnContextCreated(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) OVERRIDE;
  void OnContextReleased(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) OVERRIDE;
  void OnUncaughtException(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context,
                           CefRefPtr<CefV8Exception> exception,
                           CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
  void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefDOMNode> node) OVERRIDE;
  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) OVERRIDE;

 private:
  // Set of supported Delegates.
	 CefRefPtr<AppRenderDelegate> delegates_;

  IMPLEMENT_REFCOUNTING(ClientAppRenderer);
  DISALLOW_COPY_AND_ASSIGN(ClientAppRenderer);
};

}  // namespace client

#endif  // CEF_TESTS_SHARED_RENDERER_CLIENT_APP_RENDERER_H_

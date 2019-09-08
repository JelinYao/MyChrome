// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "stdafx.h"
#include "client_app_renderer.h"
#include "include/base/cef_logging.h"

namespace client {

ClientAppRenderer::ClientAppRenderer(CefRefPtr<AppRenderDelegate> callback/* = nullptr*/)
	: delegates_(callback)
{
 
}

void ClientAppRenderer::OnRenderThreadCreated(
    CefRefPtr<CefListValue> extra_info) 
{
	if (delegates_)
		delegates_->OnRenderThreadCreated(this, extra_info);
}

void ClientAppRenderer::OnWebKitInitialized() 
{
	if (delegates_)
		delegates_->OnWebKitInitialized(this);
}

void ClientAppRenderer::OnBrowserCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDictionaryValue> extra_info) 
{
	if (delegates_)
		delegates_->OnBrowserCreated(this, browser, extra_info);
}

void ClientAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) 
{
	if (delegates_)
		delegates_->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> ClientAppRenderer::GetLoadHandler() 
{
	CefRefPtr<CefLoadHandler> load_handler;
	if (delegates_)
		load_handler = delegates_->GetLoadHandler(this);
	return load_handler;
}

void ClientAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Context> context) 
{
	if (delegates_)
		delegates_->OnContextCreated(this, browser, frame, context);
}

void ClientAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefV8Context> context) 
{
	if (delegates_)
		delegates_->OnContextReleased(this, browser, frame, context);
}

void ClientAppRenderer::OnUncaughtException(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context,
    CefRefPtr<CefV8Exception> exception,
    CefRefPtr<CefV8StackTrace> stackTrace) 
{
	if (delegates_)
		delegates_->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
}

void ClientAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefDOMNode> node) 
{
	if (delegates_)
		delegates_->OnFocusedNodeChanged(this, browser, frame, node);
}

bool ClientAppRenderer::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) 
{
	DCHECK_EQ(source_process, PID_BROWSER);
	bool handled = false;
	if (delegates_)
		delegates_->OnProcessMessageReceived(this, browser, frame, source_process, message);
	return handled;
}

}  // namespace client

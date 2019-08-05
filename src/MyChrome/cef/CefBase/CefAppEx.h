// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#include "include/cef_app.h"
#include "CEfV8Handler.h"


class CJsBindCallback
{
public:
	virtual void OnInitJavascript(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
		CefRefPtr<CefV8Context> context)=0;
};

#define MSGBOX(x) MessageBox(NULL, x, L"Msg: ", MB_OK)

class CCefAppEx : 
	public CefApp,
    public CefBrowserProcessHandler,
	public CefRenderProcessHandler,
	public CefLoadHandler
{
public:
	CCefAppEx();
	//自定义
	void	SetJsCallback(CJsBindCallback* pCallback) { m_pJsCallback = pCallback; }
	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()	{ return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()	{ return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler()						{ return this; }
	//渲染进程回调
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
	  CefRefPtr<CefV8Context> context);
	virtual void OnContextInitialized();
	virtual void OnWebKitInitialized();
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);
	//加载状态的回调
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode);

private:
	CJsBindCallback*	m_pJsCallback;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefAppEx);
};

class CCefDOMVisitor: public CefDOMVisitor
{
public:
	virtual void Visit(CefRefPtr<CefDOMDocument> document);
	
private:
	IMPLEMENT_REFCOUNTING(CCefDOMVisitor);
};

// class CCefDOMEventListener: 
// 	public CefDOMEventListener
// {
// public:
// 	CCefDOMEventListener(LPCTSTR lpKey)
// 		: m_strKey(lpKey)
// 	{
// 
// 	}
// 	virtual void HandleEvent(CefRefPtr<CefDOMEvent> event);
// 
// private:
// 	IMPLEMENT_REFCOUNTING(CCefDOMEventListener);
// 	wstring m_strKey;
// 
// };

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

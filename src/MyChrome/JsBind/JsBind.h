#pragma once
#include "CefBase/CefAppEx.h"






class CMyJsBindCallback
	: public CJsBindCallback
{
	virtual void OnInitJavascript(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);
};




static bool JsNotifyLiveDataChange(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);
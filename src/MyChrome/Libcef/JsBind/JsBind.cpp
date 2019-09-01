#include "stdafx.h"
#include "JsBind.h"




bool JsNotifyLiveDataChange(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval)
{
	wstring strArg;
	for (size_t i = 0; i < arguments.size(); ++i)
	{
		if (arguments[i]->IsString())
		{
			CefString str = arguments[i]->GetStringValue();
			if (!str.empty())
				strArg.append(arguments[i]->GetStringValue().c_str());
			strArg.append(L"#");
		}
	}
#ifdef _DEBUG
	MessageBox(NULL, strArg.c_str(), L"JS call OtherStr:", MB_OK);
#endif
	return true;
}

void CMyJsBindCallback::OnInitJavascript(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> window = context->GetGlobal();
	CefRefPtr<CefV8Accessor> myV8Acc = new CCefV8Accessor;
	CefRefPtr<CefV8Value> val = CefV8Value::CreateString(L"elib");
	CefString cefException;
	myV8Acc->Set(L"name", window, val, cefException);
	CefRefPtr<CefV8Value> pObjApp = CefV8Value::CreateObject(myV8Acc);
	window->SetValue(L"elib", pObjApp, V8_PROPERTY_ATTRIBUTE_NONE);
	//°ó¶¨º¯ÊýNotifyLiveDataChange
	CefRefPtr<CefV8Handler> myV8handle3 = new CCefV8Handler();
	((CCefV8Handler*)myV8handle3.get())->AddFun(L"OtherStr", JsNotifyLiveDataChange);
	CefRefPtr<CefV8Value> myFun3 = CefV8Value::CreateFunction(L"OtherStr", myV8handle3);
	pObjApp->SetValue(L"OtherStr", myFun3, V8_PROPERTY_ATTRIBUTE_NONE);
}

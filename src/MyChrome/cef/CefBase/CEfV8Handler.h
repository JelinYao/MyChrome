#pragma once
#include "include/cef_client.h"
#include "include/cef_v8.h"
#include <string>
#include <vector>
#include <map>
using std::map;
using std::wstring;
using std::vector;
//定义JS回调函数指针
typedef bool(* JS_CALLBACK_FUN)(const CefV8ValueList& , CefRefPtr<CefV8Value>& );
typedef map<CefString, JS_CALLBACK_FUN> FunctionMap;

class CCefV8Handler : 
	public CefV8Handler
{
public:
	CCefV8Handler();
	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, \
		const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
	bool AddFun(const CefString& strName, JS_CALLBACK_FUN pFun);
private:
	FunctionMap	m_fun;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefV8Handler);
};


////////////////////////////////////////////
class CCefV8Accessor
	: public CefV8Accessor
{
public:
	CCefV8Accessor();
	virtual bool Get(const CefString& name, const CefRefPtr<CefV8Value> object, \
		CefRefPtr<CefV8Value>& retval, CefString& exception);
	virtual bool Set(const CefString& name, const CefRefPtr<CefV8Value> object, \
		const CefRefPtr<CefV8Value> value, CefString& exception);
private:
	CefString	m_strValue;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefV8Accessor);
};

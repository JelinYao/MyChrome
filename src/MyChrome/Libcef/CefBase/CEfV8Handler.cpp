//#include "StdAfx.h"
#include "CEfV8Handler.h"

CCefV8Handler::CCefV8Handler()
{

}

bool CCefV8Handler::Execute( const CefString& name, CefRefPtr<CefV8Value> object, \
							const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception )
{
	FunctionMap::iterator itor = m_fun.find(name);
	if ( itor == m_fun.end() )
		return false;
	itor->second(arguments, retval);
	return true;
}

bool CCefV8Handler::AddFun( const CefString& strName, JS_CALLBACK_FUN pFun )
{
	if ( strName.empty() )
		return false;
	m_fun.insert(std::pair<CefString, JS_CALLBACK_FUN>(strName, pFun));
	return true;
}



/////////////////////////////////////////////////////////////////////////////////
//CefV8Accessor

CCefV8Accessor::CCefV8Accessor()
{

}

bool CCefV8Accessor::Get( const CefString& name, const CefRefPtr<CefV8Value> object, \
						 CefRefPtr<CefV8Value>& retval, CefString& exception )
{
	if ( name != L"name" )
		return false;
	retval = CefV8Value::CreateString(m_strValue);
	return true;
}

bool CCefV8Accessor::Set( const CefString& name, const CefRefPtr<CefV8Value> object, \
						 const CefRefPtr<CefV8Value> value, CefString& exception )
{
	if ( name != L"name" )
		return false;
	if ( !value->IsString() )
		return false;
	m_strValue = value->GetStringValue();
	return true;
}
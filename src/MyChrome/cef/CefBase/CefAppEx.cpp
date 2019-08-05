// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//#include "StdAfx.h"
#include "CefAppEx.h"
#include <string>
#include "CefHandler.h"
#include "util.h"
#include "../include/cef_browser.h"
#include "../include/cef_command_line.h"


const char* pMsg1 =	"DOMTest.Message";


CCefAppEx::CCefAppEx() 
: m_pJsCallback(NULL)
{

}

void CCefAppEx::OnContextInitialized() 
{

}

void CCefAppEx::OnContextCreated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, \
								 CefRefPtr<CefV8Context> context )
{
	context->Enter();
	if ( m_pJsCallback )
		m_pJsCallback->OnInitJavascript(browser, frame, context);
// 	CefRefPtr<CefV8Value> window = context->GetGlobal();
// 	CefRefPtr<CefV8Accessor> myV8Acc = new CCefV8Accessor;
// 	CefRefPtr<CefV8Value> val = CefV8Value::CreateString(L"Application");
// 	CefString cefException;
// 	myV8Acc->Set(L"App", window, val, cefException);
// 	CefRefPtr<CefV8Value> pObjApp = CefV8Value::CreateObject(myV8Acc);
// 	window->SetValue(L"Application", pObjApp, V8_PROPERTY_ATTRIBUTE_NONE);
// 
// 
// 	CefRefPtr<CefV8Handler> myV8handle = new CCefV8Handler();
// 	CefRefPtr<CefV8Value> myFun = CefV8Value::CreateFunction(L"fun1", myV8handle);
// 	pObjApp->SetValue(L"fun1", myFun, V8_PROPERTY_ATTRIBUTE_NONE);

	//CefRefPtr<CefV8Value> pObjApp = CefV8Value::CreateObject(NULL);
	//pObjApp->SetValue(L"appid", CefV8Value::CreateString(L"1990"), V8_PROPERTY_ATTRIBUTE_READONLY);
// 	CefRefPtr<CefV8Handler> myV8handle = new CCefV8Handler();
// 	CefRefPtr<CefV8Value> myFun = CefV8Value::CreateFunction(L"fun1", myV8handle);
// 	window->SetValue(L"fun1", myFun, V8_PROPERTY_ATTRIBUTE_NONE);

	//pObjApp->
// 	CefString srtFun = L"test";
// 	CefRefPtr<CefV8Exception> exception;
// 	if ( !context->Eval(srtFun, window, exception) )
// 	{
// 		CefString& strMsg = exception->GetMessage();
// 		MessageBox(NULL, strMsg.c_str(), L"JS执行错误", 0);
// 	}
// 	else
// 		MessageBox(NULL, NULL, L"JS执行完成", 0);
	context->Exit();
// 	CefRefPtr<CefFrame> mf = browser->GetMainFrame();
// 	//CefRefPtr<CefDOMVisitor> visitor = new CCefDOMVisitor();
// 	mf->VisitDOM(new CCefDOMVisitor());
}

void CCefAppEx::OnWebKitInitialized()
{
	//CefRegisterExtension();
}

void CCefAppEx::OnLoadEnd( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode )
{
// 	CefRefPtr<CefProcessMessage> pMsg = CefProcessMessage::Create(pMsg1);
//  	bool bRet = browser->SendProcessMessage(PID_RENDERER, pMsg);
// 	if ( bRet )
// 	{
// 		MessageBox(NULL, L"消息发送成功", NULL, 0);
// 	}
}

bool CCefAppEx::OnProcessMessageReceived( CefRefPtr<CefBrowser> browser, CefProcessId source_process, \
	CefRefPtr<CefProcessMessage> message )
{
// 	if ( message->GetName() == pMsg1 )
// 	{
// 		browser->GetMainFrame()->VisitDOM(new CCefDOMVisitor);
// 		return true;
// 	}
	return false;
}
// static int s_nCount = 0;
// CefRefPtr<CefDOMNode> g_pEditNode = NULL;
// void EnumHtmlElems(CefRefPtr<CefDOMNode> pNode)
// {
// 	if ( pNode == NULL || pNode.get() == NULL )
// 		return ;
// 	CefString strTag = pNode->GetElementTagName();
// 	CefString strId = pNode->GetElementAttribute(L"id");
// 	if ( !strId.empty() )
// 	{
// 		if ( (wcsicmp(strTag.c_str(), L"input") == 0) && (wcsicmp(strId.c_str(), L"su") == 0) )
// 		{
// 			pNode->AddEventListener(L"click", new CCefDOMEventListener(L"su"), false);
// 			//MSGBOX(L"捆绑百度搜索按钮完成");
// 		}
// 		else if ( (wcsicmp(strTag.c_str(), L"input") == 0) && (wcsicmp(strId.c_str(), L"kw") == 0) )
// 		{
// 			//pNode->AddEventListener(L"click", new CCefDOMEventListener(L"kw"), false);
// 			//MSGBOX(L"捆绑百度搜索按钮完成");
// 			g_pEditNode = pNode;
// 		}
// 	}
// 	s_nCount++;
// 	CefRefPtr<CefDOMNode> pNext = NULL;
// 	if ( pNode->HasChildren() )
// 	{
// 		pNext = pNode->GetFirstChild();
// 		EnumHtmlElems(pNext);
// 	}
// 	pNext = pNode->GetNextSibling();
// 	EnumHtmlElems(pNext);
// }
// 
// CefRefPtr<CefDOMNode> FindElemById(CefRefPtr<CefDOMNode> pNode, LPCTSTR lpTagName, LPCTSTR lpId)
// {
// 	if ( pNode == NULL || pNode.get() == NULL )
// 		return NULL;
// 	CefString strTag = pNode->GetElementTagName();
// 	CefString strId = pNode->GetElementAttribute(L"id");
// 	if ( !strTag.empty() && !strId.empty() )
// 	{
// 		if ( (wcsicmp(strTag.c_str(), L"input") == 0) && (wcsicmp(strId.c_str(), L"kw") == 0) )
// 		{
// 			return pNode;
// 		}
// 	}
// 	CefRefPtr<CefDOMNode> pNext = NULL, pFind = NULL;
// 	if ( pNode->HasChildren() )
// 	{
// 		pNext = pNode->GetFirstChild();
// 		pFind = FindElemById(pNext, lpTagName, lpId);
// 		if ( pFind )
// 			return pFind;
// 	}
// 	pNext = pNode->GetNextSibling();
// 	pFind = FindElemById(pNext, lpTagName, lpId);
// 	if ( pFind )
// 		return pFind;
// 	return NULL;
// }
// 
// void CCefDOMVisitor::Visit( CefRefPtr<CefDOMDocument> document )
// {
// 	CefRefPtr<CefDOMNode> pBody = document->GetBody();
// 	if ( pBody )
// 	{
// 		CefRefPtr<CefDOMNode> pChild = pBody->GetFirstChild();
// 		EnumHtmlElems(pChild);
// 		wchar_t szText[100];
// 		wsprintf(szText, L"循环遍历次数=%d", s_nCount);
// 		MSGBOX(szText);
// 		while( pChild )
// 		{
// 
// 			CefString strTag = pChild->GetElementTagName();
// 			CefString strClass = pChild->GetElementAttribute(L"id");
// 			if ( wcsicmp(strTag.c_str(), L"div") == 0 && (!strClass.empty()) )
// 			{
// 				if ( wcsicmp(strClass.c_str(), L"wrapper") == 0 )
// 				{
// 					MessageBox(NULL, L"设置成功", NULL, 0);
// 					pChild->SetElementAttribute(L"style", L"display:none");
// 				}
// 			}
// 			pChild = pChild->GetNextSibling();
// 		}
// 	}
// }
// 
// void CCefDOMEventListener::HandleEvent( CefRefPtr<CefDOMEvent> event )
// {
// 	CefString strType = event->GetType();
// 	if ( wcsicmp(strType.c_str(), L"click") == 0 )
// 	{
// 		if ( m_strKey.compare(L"su") == 0 )
// 		{
// 			CefRefPtr<CefDOMDocument> doc = event->GetDocument();
// 			CefRefPtr<CefDOMNode> pBody = doc->GetBody();
// 			CefRefPtr<CefDOMNode> pChild = pBody->GetFirstChild();
// 			CefRefPtr<CefDOMNode> pFind = FindElemById(pChild, L"input", L"kw");
// 			if ( pFind )
// 			{
// 				CefString str = pFind->GetValue();
// 				if ( str.empty() )
// 					MSGBOX(L"Value为空！");
// 				else
// 					MSGBOX(str.c_str());
// 			}
// 			else
// 				MSGBOX(L"没有找到input控件");
// 			if ( g_pEditNode )
// 			{
// 				CefString str = g_pEditNode->GetValue();
// 				if ( str.empty() )
// 					MSGBOX(L"Value为空！");
// 				else
// 				//g_pEditNode->GetName()
// 					MSGBOX(str.c_str());
// 			}
// 			MSGBOX(L"点击了百度搜索按钮");
// 		}
// 	}
// }

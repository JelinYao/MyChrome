#include "stdafx.h"
#include "MainWnd.h"






void CMainWnd::OnLoadStart(CCefWebkitUI* pWeb, void* lpParam)
{
	CChromePage* pChromePage = (CChromePage*)lpParam;
	int nIndex = m_pTabWeb->GetItemIndex(pChromePage);
	if (nIndex != -1)
		m_pTabBar->SetItemText(nIndex, L"页面加载中……");
	pChromePage->UpdateLoadState(Wls_Begin);
}

void CMainWnd::OnLoadEnd(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser)
{
	CChromePage* pChromePage = (CChromePage*)lpParam;
	int nIndex = m_pTabWeb->GetItemIndex(pChromePage);
	if (nIndex == -1)
		return;
	pChromePage->UpdateLoadState(Wls_Finish);
// 	if (pWeb->GetUserValue())
// 	{
// 		m_pTabBar->SetItemText(nIndex, L"新标签页");
// 		pWeb->SetUserValue(0);
// 	}
// 	else
// 		m_pTabBar->SetItemText(nIndex, pWeb->GetTitle().c_str());
// 	CContainerUI* pPage = static_cast<CContainerUI*>(m_pTabWeb->GetItemAt(nIndex));
// 	if (NULL == pPage)
// 		return;
// 	CCefWebkitUI* pCurWeb = static_cast<CCefWebkitUI*>(pPage->FindSubControl(L"cefwebkit"));
// 	if (pWeb == pCurWeb)
// 	{
// 		CEditUI* pEdit = static_cast<CEditUI*>(pPage->FindSubControl(L"urledit"));
// 		if (pWeb->GetUserValue())
// 			pEdit->SetText(L"NewTab");
// 		else
// 			pEdit->SetText(pWeb->GetLoadingUrl().c_str());
// 	}

	// 	static const char* pMsg1 = "DOMTest.Message";
	// 	CefRefPtr<CefProcessMessage> pMsg = CefProcessMessage::Create(pMsg1);
	// 	browser->SendProcessMessage(PID_RENDERER, pMsg);
}

void CMainWnd::OnLoadError(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefFrame> pFrame)
{
	CChromePage* pChromePage = (CChromePage*)lpParam;
	int nIndex = m_pTabWeb->GetItemIndex(pChromePage);
	if (nIndex == -1)
		return;
	pChromePage->UpdateLoadState(Wls_Error);
	m_pTabBar->SetItemText(nIndex, L"页面加载失败");
	wstring strErrorHtml = g_strExePath + L"Html\\error.htm";
	if (PathFileExists(strErrorHtml.c_str()))
	{
		pWeb->Navigate(strErrorHtml.c_str());
	}
}

void CMainWnd::OnSetAddress(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url)
{
	CChromePage* pChromePage = (CChromePage*)lpParam;
	int nIndex = m_pTabWeb->GetItemIndex(pChromePage);
	if (nIndex == -1)
		return;
	CEditUI* pEdit = static_cast<CEditUI*>(pChromePage->FindSubControl(L"urledit"));
	if (pChromePage->IsNewBank())
		pEdit->SetText(L"");
	else
		pEdit->SetText(pWeb->GetLoadingUrl().c_str());
}

void CMainWnd::OnSetTitle(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& title)
{
	pWeb->SetTitle(title.c_str());
	CChromePage* pChromePage = (CChromePage*)lpParam;
	int nIndex = m_pTabWeb->GetItemIndex(pChromePage);
	if (nIndex == -1)
		return;
	if (pChromePage->IsNewBank())
	{
		m_pTabBar->SetItemText(nIndex, L"新标签页");
	}
	else
	{
		m_pTabBar->SetItemText(nIndex, pWeb->GetTitle().c_str());
	}
}

bool CMainWnd::OnOpenNewUrl(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url)
{
	int size = sizeof(wchar_t)*(url.size() + 1);
	wchar_t* pUrl = (wchar_t*)malloc(size);
	wcscpy_s(pUrl, size, url.c_str());
	pUrl[url.size()] = '\0';
	::PostMessage(m_hWnd, WM_OPENURL_MSG, 0, (LPARAM)pUrl);
	return true;
}

void CMainWnd::OnStatusMessage(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& msg)
{
	SetStatus(msg.c_str());
}

bool CMainWnd::OnBeforeBrowse(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, const std::wstring& url)
{
	//需要拦截的网址，请返回true
	CChromePage* pChromePage = (CChromePage*)lpParam;
	if (url.find(L"Html/NewTab.htm") != wstring::npos)
	{
		pChromePage->SetNewBank(true);
	}
	else
	{
		pChromePage->SetNewBank(false);
	}
	return false;
}

void CMainWnd::OnItemClose(void* pTabBar, const int nDelIndex, const int nSelIndex)
{
	CChromePage* pPage = static_cast<CChromePage*>(m_pTabWeb->GetItemAt(nDelIndex));
	if ( NULL == pPage )
		return ;
	CCefWebkitUI* pWeb = static_cast<CCefWebkitUI*>(pPage->GetWebkit());
	if (pWeb)
	{
		pWeb->Close();
	}
	m_pTabWeb->Remove(pPage);
	int nCount = m_pTabWeb->GetCount();
	if (nCount == 0)
	{
		wstring strNewHtml = g_strExePath + L"Html\\NewTab.htm";
		AddNewBrowser(strNewHtml.c_str(), true);
		return ;
	}
	m_pTabWeb->SelectItem(nSelIndex);
}

void CMainWnd::OnItemAdd(void* pTabBar)
{
	wstring strNewHtml = g_strExePath + L"Html\\NewTab.htm";
	AddNewBrowser(strNewHtml.c_str(), true);
}

void CMainWnd::OnItemSelectChange(void* pTabBar, const int nUnSelIndex, const int nSelIndex)
{
	CChromePage* pPage = dynamic_cast<CChromePage*>(m_pTabWeb->GetItemAt(nSelIndex));
	if (NULL == pPage)
		return;
	CCefWebkitUI* pWeb = static_cast<CCefWebkitUI*>(pPage->FindSubControl(L"cefwebkit"));
	if (pWeb)
	{
		bool bCanBack = pWeb->CanGoBack();
		pWeb->SetEnabled(bCanBack);
		CEditUI* pEdit = pPage->GetEdit();
		if (pPage->IsNewBank())
			pEdit->SetText(L"");
		else if (pEdit->GetText().IsEmpty())
		{
			wstring strUrl = pWeb->GetLoadingUrl();
			if (!strUrl.empty())
				pEdit->SetText(strUrl.c_str());
		}
		m_pTabWeb->SelectItem(nSelIndex);
	}
	const wstring& strTitle = pWeb->GetTitle();
	wstring strText;
	if (strTitle.empty())
	{
		strText = SOFT_NAME;
	}
	else
	{
		if (pPage->IsNewBank())
			strText = L"新标签页 - ";
		else
			strText = strTitle + L" - ";
		strText.append(SOFT_NAME);
	}
	::SetWindowText(m_hWnd, strText.c_str());
}

void CMainWnd::OnItemDbClick(void* pTabBar, const int nIndex)
{	
	CTabBarUI* pTabBarUI = (CTabBarUI*)pTabBar;
	if (pTabBarUI)
		pTabBarUI->DeleteItem(nIndex);
}

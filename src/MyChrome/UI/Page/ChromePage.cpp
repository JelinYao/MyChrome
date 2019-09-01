#include "StdAfx.h"
#include "ChromePage.h"
#include "ControlCallback.h"
#include "UI/Wnd/EditMenuWnd.h"



CChromePage::CChromePage(CPaintManagerUI* pManager, bool bNewBank)
: m_pWebkit(NULL)
, m_pEdit(NULL)
, m_pBtnBack(NULL)
, m_pBtnRefresh(NULL)
, m_pBtnHome(NULL)
, m_bIsNewBank(bNewBank)
{
	CDialogBuilder builder;
	CControlCallback cb;
	CContainerUI* pItem = dynamic_cast<CContainerUI*>(builder.Create(L"ChromePage.xml", 0, &cb, NULL));
	if (pItem)
	{
		Add(pItem);
	}
}

CChromePage::~CChromePage(void)
{
}

void CChromePage::Init()
{
	m_pWebkit = static_cast<CCefWebkitUI*>(FindSubControl(L"cefwebkit"));
	assert(m_pWebkit);
	m_pEdit = static_cast<CEditUI*>(FindSubControl(L"urledit"));
	assert(m_pEdit);
	m_pBtnBack = FindSubControl(L"btn_back");
	assert(m_pBtnBack);
	m_pBtnBack->OnNotify += MakeDelegate(this, &CChromePage::OnNotifyBtnBack);
	m_pBtnRefresh = static_cast<CButtonUI*>(FindSubControl(L"btn_refresh"));
	assert(m_pBtnRefresh);
	m_pBtnRefresh->OnNotify += MakeDelegate(this, &CChromePage::OnNotifyBtnRefresh);
	m_pBtnHome = static_cast<CButtonUI*>(FindSubControl(L"btn_home"));
	assert(m_pBtnHome);
	m_pBtnHome->OnNotify += MakeDelegate(this, &CChromePage::OnNotifyBtnHome);
}

void CChromePage::SetEditText(LPCWSTR lpText)
{
	if (m_pEdit)
	{
		m_pEdit->SetText(lpText);
	}
}

void CChromePage::OnSize(int nWidth, int nHeight)
{
	static int nOldWidth = 0, nOldHeight = 0;
	if (nWidth == nOldWidth && nHeight == nOldHeight)
		return ;
	m_pWebkit->MoveWindow(0, 66, nWidth, nHeight-66, TRUE);
}

bool CChromePage::OnNotifyBtnBack(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (m_pWebkit)
			m_pWebkit->GoBack();
	}
	return true;
}

bool CChromePage::OnNotifyBtnRefresh( void* lpParam )
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (m_pWebkit)
			m_pWebkit->Refresh();
	}
	return true;
}

bool CChromePage::OnNotifyBtnHome( void* lpParam )
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		const wstring& strUrl = CGlobalMgr::Instance()->GetHomePage();
		if (m_pWebkit)
			m_pWebkit->Navigate(strUrl.c_str());
	}
	return true;
}

void CChromePage::LoadUrl(LPCTSTR lpUrl)
{
	if (lpUrl && wcslen(lpUrl)>0)
	{
		m_pWebkit->Navigate(lpUrl);
		//m_pEdit->SetText(lpUrl);
	}
}

void CChromePage::UpdateLoadState(WebLoadState state)
{
	switch (state)
	{
	case Wls_Begin:
		m_pBtnBack->SetEnabled(false);
		break;
	case Wls_Finish:
	case Wls_Error:
		m_pBtnBack->SetEnabled(m_pWebkit->CanGoBack());
		m_pBtnRefresh->SetEnabled();
		break;
	default:
		break;
	}
}

void CChromePage::Close()
{
	if (m_pWebkit)
		m_pWebkit->Close();
}

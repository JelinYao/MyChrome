#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"


CMainWnd::CMainWnd(void)
	: m_pTabBar(NULL)
	, m_pTabWeb(NULL)
	, m_pLblState(NULL)
	, m_bClose(false)
{
}


CMainWnd::~CMainWnd(void)
{
}

DuiLib::UILIB_RESOURCETYPE CMainWnd::GetResourceType() const
{
	return UILIB_FILE;
}

LPCTSTR CMainWnd::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_DAT1);
}

void CMainWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
	CefQuitMessageLoop();
}

void CMainWnd::OnCloseWnd()
{
	WindowImplBase::OnCloseWnd();
}

CControlUI* CMainWnd::CreateControl( LPCTSTR pstrClass )
{
	if (wcscmp(pstrClass, L"TabBar") == 0)
		return new CTabBarUI;
	if ( _tcscmp(pstrClass, _T("CefWebkit")) == 0 )
		return new CCefWebkitUI;
	if ( _tcscmp(pstrClass, _T("ChromePage")) == 0 ) 
		return new CChromePage(&m_PaintManager, true);
	return NULL;
}

void CMainWnd::InitWindow()
{
	CGlobalMgr::Instance()->SetMainWnd(m_hWnd);
	m_pTabBar = static_cast<CTabBarUI*>(m_PaintManager.FindControl(L"tabbar"));
	assert(m_pTabBar);
	m_pTabBar->SetCallback(this);
	m_pLblState = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(L"lbl_state"));
	assert(m_pLblState);
	CControlUI* pControl = m_PaintManager.FindControl(L"btn_sys_skin");
	assert(pControl);
	pControl->OnNotify += MakeDelegate(this, &CMainWnd::OnNotifyBtnSkin);

	pControl = m_PaintManager.FindControl(L"btn_sys_set");
	assert(pControl);
	pControl->OnNotify += MakeDelegate(this, &CMainWnd::OnNotifyBtnSet);

	m_pTabWeb = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(L"tab_web"));
	assert(m_pTabWeb);
	SetIcon(IDI_ICON1);
	AddNewBrowser(HOMEPAGE_URL);
}

bool CMainWnd::AddNewBrowser(LPCTSTR lpUrl, bool bNewBank/* = false*/)
{
	CChromePage* pChromePage = new CChromePage(&m_PaintManager, bNewBank);
	m_pTabWeb->Add(pChromePage);
	CCefWebkitUI* pChrome = pChromePage->GetWebkit();
	if ( pChrome )
	{
		pChrome->Navigate(lpUrl);
		pChrome->SetWebloadCallback(this, pChromePage);
	}
	CControlUI* pItem = m_pTabBar->AddTabItem(L"Мгдижа...", true);
	if ( NULL == pItem )
	{
		delete pChrome;
		return false;
	}
	m_pTabBar->SelectItem(pItem);
	CEditUI* pUrlEdit = pChromePage->GetEdit();
	assert(pUrlEdit);
	pUrlEdit->OnNotify += MakeDelegate(this, &CMainWnd::OnNotifyUrlEdit);
	m_pTabWeb->SelectItem(0);
	m_pTabWeb->SelectItem(pChromePage);
	return true;
}

CChromePage* CMainWnd::GetCurrentSelPage()
{
	int nCount = m_pTabWeb->GetCount();
	if ( 0 == nCount )
	{
		return NULL;
	}
	int nIndex = m_pTabWeb->GetCurSel();
	if (nIndex < 0)
		return NULL;
	return dynamic_cast<CChromePage*>(m_pTabWeb->GetItemAt(nIndex));
}

void CMainWnd::CloseAllWeb()
{
	if (m_bClose)
		return;
	int nCount = m_pTabWeb->GetCount();
	for (int i = 0; i < nCount; ++i)
	{
		CChromePage* pChromePage = dynamic_cast<CChromePage*>(m_pTabWeb->GetItemAt(i));
		if (NULL == pChromePage)
			continue;
		pChromePage->Close();
	}
	m_bClose = true;
}

bool CMainWnd::HitTestControl(CControlUI* pCtrl)
{
	if (_tcsicmp(pCtrl->GetClass(), _T("UITabItem")) == 0)
		return true;
	return WindowImplBase::HitTestControl(pCtrl);
}











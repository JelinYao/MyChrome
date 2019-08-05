#include "StdAfx.h"
#include "MyMenuUI.h"



CMyMenuUI::CMyMenuUI( CControlUI* pSender)
	: m_pList(NULL)
	, m_pCtrlSender(pSender)
	, m_nWidth(0)
	, m_nHeight(0)
{
	ASSERT(pSender);
}


CMyMenuUI::~CMyMenuUI(void)
{
}

void CMyMenuUI::SetWndSize(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void CMyMenuUI::AddMenuItem(LPCTSTR lpText, int xPadding, bool bEnable/* = true*/)
{
	CDialogBuilder dlgBuilder;
	CListContainerElementUI* pItem = (CListContainerElementUI*)dlgBuilder.Create(L"Menu\\menu_item.xml", (UINT)0, NULL);
	if (NULL == pItem)
	{
		return;
	}
	CButtonUI* pCtrl = static_cast<CButtonUI*>(pItem->FindSubControl(L"btn_menu"));
	if (NULL == pCtrl)
	{
		return;
	}
	pCtrl->OnNotify += MakeDelegate(this, &CMyMenuUI::OnNotifyMenuItem);
	pCtrl->SetText(lpText);
	RECT rc = { xPadding, 0, 0, 0 };
	pCtrl->SetTextPadding(rc);
	int nIndex = m_pList->GetCount();
	pCtrl->SetTag(nIndex);
	pCtrl->SetEnabled(bEnable);
	pItem->SetEnabled(bEnable);
	m_pList->Add(pItem);
}

void CMyMenuUI::InitWindow()
{
	m_pList = static_cast<CListUI*>(m_PaintManager.FindControl(L"menulist"));
	ASSERT(m_pList);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE)&~WS_EX_APPWINDOW | WS_EX_TOOLWINDOW); //去掉任务栏窗口
	if ( m_nWidth>0 && m_nHeight>0 )
		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE);
}

LRESULT CMyMenuUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KILLFOCUS)
	{
		Close();
		return 0;
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

bool CMyMenuUI::OnNotifyMenuItem(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType != DUI_MSGTYPE_CLICK) 
		return true;
	ShowWindow(false, false);
	int nIndex = pNotify->pSender->GetTag();
	m_pCtrlSender->GetManager()->SendNotify(m_pCtrlSender, DUI_MSGTYPE_CLICK_MENU_ITEM, nIndex, 0, true);
	return true;
}

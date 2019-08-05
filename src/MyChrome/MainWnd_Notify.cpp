#include "stdafx.h"
#include "MainWnd.h"
#include "EditMenuWnd.h"
#include "SkinWnd/SkinWnd.h"
#include "Menu/MyMenuUI.h"


bool CMainWnd::OnNotifyUrlEdit( void* lpParam )
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if ( pNotify->sType == DUI_MSGTYPE_RETURN )
	{
		CEditUI* pEdit = static_cast<CEditUI*>(pNotify->pSender);
		CDuiString strText = pEdit->GetText();
		if (strText.IsEmpty())
		{//弹窗提示
			return true;
		}
// 		bool bIsUrl = IsUrl(wstring(strText.GetBuffer(0)));
// 		if ( !bIsUrl )
// 		{
// 			strText = L"www.baidu.com/s?wd=" + strText;
// 		}
		int nIndex = m_pTabWeb->GetCurSel();
		CChromePage* pPage = static_cast<CChromePage*>(m_pTabWeb->GetItemAt(nIndex));
		if (pPage)
		{
			pPage->LoadUrl(strText);
		}
	}
	else if ( pNotify->sType == DUI_MSGTYPE_EDIT_RBTNUP )
	{
		CEditUI* pEdit = static_cast<CEditUI*>(pNotify->pSender);
		pEdit->SetRbtnupMenu(true);
		CEditMenuWnd* pMenu = new CEditMenuWnd(pEdit);
		pMenu->Create(NULL, L"", UI_WNDSTYLE_FRAME, WS_EX_TOPMOST);
		POINT pt;
		GetCursorPos(&pt);
		::MoveWindow(pMenu->GetHWND(), pt.x, pt.y, 84, 102, FALSE);
		pMenu->ShowWindow();
	}
	return true;
}


bool CMainWnd::OnNotifyBtnSkin( void* lpParam )
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if ( pNotify->sType == DUI_MSGTYPE_CLICK )
	{
		CSkinWnd* pWnd = new CSkinWnd;
		pWnd->Create(m_hWnd, L"换肤", UI_WNDSTYLE_DIALOG, 0);
		pWnd->CenterWindow();
		pWnd->ShowModal();
	}
	return true;
}

bool CMainWnd::OnNotifyBtnSet(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		CMyMenuUI* pMenu = new CMyMenuUI(pNotify->pSender);
		POINT pt;
		GetCursorPos(&pt);
		pMenu->SetWndSize(96, 134);
		pMenu->Create(m_hWnd, L"Menu", UI_WNDSTYLE_DIALOG, 0, pt.x, pt.y, 0, 0, 0);
		pMenu->AddMenuItem(L"预览", 22);
		pMenu->AddMenuItem(L"导入", 22);
		pMenu->AddMenuItem(L"导出", 22);
		pMenu->AddMenuItem(L"删除", 22);
		pMenu->ShowWindow();
	}
	return true;
}

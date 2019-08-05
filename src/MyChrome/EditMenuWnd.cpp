#include "StdAfx.h"
#include "EditMenuWnd.h"

CEditMenuWnd::CEditMenuWnd(CControlUI* pEditCtrl)
	: m_pEditCtl(pEditCtrl)
	, m_pUndo(NULL)
	, m_pCut(NULL)
	, m_pCopy(NULL)
	, m_pPaste(NULL)
{
	
}

CEditMenuWnd::~CEditMenuWnd(void)
{
}

void CEditMenuWnd::OnFinalMessage( HWND hWnd )
{
	CEditUI* pEdit = static_cast<CEditUI*>(m_pEditCtl);
	if ( pEdit )
	{
		HWND hEditWnd = pEdit->GetEditHwnd();
		if ( hEditWnd )
		{
			::SendMessage(hEditWnd, WM_CLOSE, 0, 0);
			pEdit->SetRbtnupMenu(false);
		}
	}
	CMenuImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CEditMenuWnd::OnClick( TNotifyUI& msg )
{
	CEditUI* pEdit			= static_cast<CEditUI*>(m_pEditCtl);
	CRichEditUI* pRichEdit	= static_cast<CRichEditUI*>(m_pEditCtl);
	HWND hEditWnd = NULL;
	if ( pEdit )
	{
		hEditWnd = pEdit->GetEditHwnd();
		if ( NULL == hEditWnd )
		{
			Close();
			return;
		}
	}
	if ( _tcscmp(msg.pSender->GetName(), _T("undo")) == 0 )
	{
		hEditWnd ? ::SendMessage(hEditWnd, WM_UNDO, 0, 0):pRichEdit->Undo();
	}

	else if ( _tcscmp(msg.pSender->GetName(), _T("cut")) == 0 )
	{
		hEditWnd ? ::SendMessage(hEditWnd, WM_CUT, 0, 0):pRichEdit->Cut();
	}

	else if ( _tcscmp(msg.pSender->GetName(), _T("copy")) == 0 )
	{
		hEditWnd ? ::SendMessage(hEditWnd, WM_COPY, 0, 0):pRichEdit->Copy();
	}

	if ( _tcscmp(msg.pSender->GetName(), _T("paste")) == 0 )
	{
		hEditWnd ? ::SendMessage(hEditWnd, WM_PASTE, 0, 0):pRichEdit->Paste();
	}
	Close();
}

void CEditMenuWnd::InitWindow()
{
	m_pUndo	= m_PaintManager.FindControl(L"undo");
	m_pCut	= m_PaintManager.FindControl(L"cut");
	m_pCopy	= m_PaintManager.FindControl(L"copy");
	m_pPaste= m_PaintManager.FindControl(L"paste");
	CRichEditUI* pRichEdit = dynamic_cast<CRichEditUI*>(m_pEditCtl);
	if ( pRichEdit )
	{

	}
	else
	{
		CEditUI* pEdit = dynamic_cast<CEditUI*>(m_pEditCtl);
		HWND hWnd = pEdit->GetEditHwnd();
		if ( hWnd )
		{
			LRESULT lRet = ::SendMessage(hWnd, EM_CANUNDO, 0, 0);
			if ( lRet )
				m_pUndo->SetEnabled();
// 			lRet = ::SendMessage(hWnd, EM_CANPASTE, 0, 0);
// 			if ( lRet )
// 				m_pPaste->SetEnabled();
			DWORD dwBegin, dwEnd;  
			::SendMessage(hWnd, EM_GETSEL, (WPARAM)&dwBegin, (LPARAM)&dwEnd);
			if ( dwBegin != dwEnd )
			{
				m_pCut->SetEnabled();
				m_pCopy->SetEnabled();
			}
		}
	}
}

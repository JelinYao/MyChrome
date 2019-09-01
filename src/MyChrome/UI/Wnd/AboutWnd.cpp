#include "StdAfx.h"
#include "AboutWnd.h"



CAboutWnd::CAboutWnd(void)
:m_pLabelVersion(NULL)
,m_pBtnOk(NULL)
,m_pBtnWeb(NULL)
{
}

CAboutWnd::~CAboutWnd(void)
{
}

void CAboutWnd::InitWindow()
{
	ModifyStyle(WS_MAXIMIZEBOX, NULL);//去掉最大化功能
	m_pLabelVersion	=static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_version")));
	m_pBtnOk		=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_ok")));
	m_pBtnWeb		=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_web")));
	m_pLabelVersion->SetText(SOFT_VERSION);
	CDuiString strUrl;
	strUrl.Format(L"<u>%s</u>", SOFT_WEBSITE);
	m_pBtnWeb->SetText(strUrl);
}

void CAboutWnd::OnClick( TNotifyUI& msg )
{
	if ( msg.pSender == m_pBtnOk )
		Close();
	else if ( msg.pSender == m_pBtnWeb )
		::ShellExecute(NULL, L"open", SOFT_WEBSITE, NULL, NULL, SW_SHOW);
	WindowImplBase::OnClick(msg);
}

void CAboutWnd::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}


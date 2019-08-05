#include "StdAfx.h"
#include "SkinWnd.h"



CSkinWnd::CSkinWnd(void)
:m_pBtnOk(NULL)
,m_pColorSkin(NULL)
,m_pTitleBar(NULL)
{
}

CSkinWnd::~CSkinWnd(void)
{
}

void CSkinWnd::InitWindow()
{
	m_pBtnOk		= static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_ok"));
	m_pColorSkin	= static_cast<CColorSkinUI*>(m_PaintManager.FindControl(L"color"));
	m_pTitleBar		= static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(L"sys_titlebar"));
	m_pColorSkin->SetSelectColorCallback(this);
	m_pColorSkin->SetCurSelPos(g_skinManager.GetSelectLine(), g_skinManager.GetSelectColumn());
	g_skinManager.AddToMap(this, m_pTitleBar);
}

void CSkinWnd::OnClick( TNotifyUI& msg )
{
	if ( m_pBtnOk == msg.pSender )
	{
		g_skinManager.ChangeSkinColor(m_pColorSkin->GetSelectColor());
		g_skinManager.SetSelPoint(m_pColorSkin->GetCurSelLine(), m_pColorSkin->GetCurSelCol());
		Close();
	}
	else
	{
		g_skinManager.ResetSkinColor();
		WindowImplBase::OnClick(msg);
	}
}

void CSkinWnd::OnFinalMessage( HWND hWnd )
{
	try
	{
		WindowImplBase::OnFinalMessage(hWnd);
		delete this;
	}
	catch(...)
	{
	}
}

CControlUI* CSkinWnd::CreateControl( LPCTSTR pstrClass )
{
	if ( _tcscmp(pstrClass, _T("ColorSkin")) == 0 )
	{	
		return new CColorSkinUI;
	}
	return NULL;
}

void CSkinWnd::ColorTest( DWORD color )
{
	g_skinManager.SetSkinColor(color);
}



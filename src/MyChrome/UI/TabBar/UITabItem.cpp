#include "StdAfx.h"
#include "UITabItem.h"


#define BTN_CLOSE_WIDTH		14
#define BTN_CLOSE_HEIGHT	14

CTabItemUI::CTabItemUI(void)
	: m_pBtnExit(NULL)
	, m_pParent(NULL)
{
}


CTabItemUI::~CTabItemUI(void)
{
}

void CTabItemUI::Init()
{
	m_pBtnExit = new CButtonUI;
	CDuiString strAttr;
	strAttr.Format(L"float=\"true\" pos=\"%d,%d,%d,%d\" normalimage=\"close_nor.png\" hotimage=\"close_hot.png\" pushedimage=\"close_push.png\"",
		m_cxyFixed.cx-BTN_CLOSE_WIDTH, 0, m_cxyFixed.cx, BTN_CLOSE_HEIGHT);
	m_pBtnExit->ApplyAttributeList(strAttr);
	Add(m_pBtnExit);
	m_pBtnExit->OnNotify += MakeDelegate(this, &CTabItemUI::OnBtnClose);
}

void CTabItemUI::PaintText( HDC hDC )
{
	RECT rc = m_rcItem;
	rc.left += 4;
	rc.right-= 10;
	CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, 0xFF666666, 0,  DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
}

void CTabItemUI::SetPos( RECT rc )
{
	CContainerUI::SetPos(rc);
	RECT rcExit = {rc.right-BTN_CLOSE_WIDTH-1, rc.top+1, rc.right-1, rc.top+BTN_CLOSE_HEIGHT+1};
	m_pBtnExit->SetPos(rcExit);
}

void CTabItemUI::DoEvent( TEventUI& event )
{
	if(event.Type == UIEVENT_BUTTONDOWN && m_pParent)
	{
		m_pManager->SendNotify(m_pParent, TAB_ITEM_CLICK, 0, (LPARAM)this, true);
	}
	else if (event.Type == UIEVENT_DBLCLICK && m_pParent)
	{
		m_pManager->SendNotify(m_pParent, TAB_ITEM_DBCLICK, 0, (LPARAM)this, true);
	}
	CContainerUI::DoEvent(event);
}

bool CTabItemUI::OnBtnClose( void* pParam )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if(pNotifyUI->sType == DUI_MSGTYPE_CLICK && m_pParent) 
	{
		m_pManager->SendNotify(m_pParent, TAB_ITEM_CLOSE, 0, (LPARAM)this, true);
	}
	return true;
}

void CTabItemUI::ShowCloseBtn(bool bShow)
{
	if (m_pBtnExit)
		m_pBtnExit->SetVisible(bShow);
}

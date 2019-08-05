#include "StdAfx.h"
#include "UITabBar.h"
#include "UITabItem.h"


CTabBarUI::CTabBarUI(void)
	: m_nNorWidth(100)
	, m_nCurWidth(0)
	, m_nMinWidth(30)
	, m_dwTabSelColor(0xffffffff)
	, m_dwTabNorColor(0xfff0f0f0)
	, m_nSelIndex(-1)
	, m_pBtnAdd(NULL)
	, m_pCallback(NULL)
	, m_bShowAdd(true)
{
}


CTabBarUI::~CTabBarUI(void)
{
}

void CTabBarUI::Init()
{
	m_pBtnAdd = new CButtonUI;
	CDuiString strAttr = L"float=\"true\" pos=\"0,0,0,0\" normalimage=\"file='tab_add.png' source='0,0,25,34' dest='0,0,25,30' \" \
					hotimage=\"file='tab_add.png' source='25,0,50,34' dest='0,0,25,30' \" pushedimage=\"file='tab_add.png' source='50,0,75,34' dest='0,0,25,30' \" \
					disabledimage=\"file='tab_add.png' source='75,0,100,34' dest='0,0,25,30' \" tooltip=\"ÐÂ½¨\" ";
	m_pBtnAdd->ApplyAttributeList(strAttr);
	Add(m_pBtnAdd);
	m_pBtnAdd->SetVisible(m_bShowAdd);
	m_pBtnAdd->OnNotify += MakeDelegate(this, &CTabBarUI::OnBtnClickAdd);
	OnNotify += MakeDelegate(this, &CTabBarUI::OnNotifySelf);
}

void CTabBarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (wcscmp(pstrName, L"TabWidth") == 0)
	{
		m_nNorWidth = _ttoi(pstrValue);
		return;
	}
	if (wcscmp(pstrName, L"MinTabWidth") == 0)
	{
		m_nMinWidth = _ttoi(pstrValue);
		return;
	}
	if (wcscmp(pstrName, L"TabSelColor") == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		m_dwTabSelColor = _tcstoul(pstrValue, &pstr, 16);
		return;
	}
	if (wcscmp(pstrName, L"TabNorColor") == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		m_dwTabNorColor = _tcstoul(pstrValue, &pstr, 16);
		return;
	}
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

CControlUI* CTabBarUI::AddTabItem(LPCTSTR lpText, bool bReset/*=false*/, bool bShowClose/*=true*/)
{
	CTabItemUI* pTabItem = new CTabItemUI;
	if (NULL == pTabItem)
		return NULL;
	CDuiString strAttr;
	strAttr.Format(L"float=\"true\" bordercolor=\"#FF999999\" bordersize=\"1\" borderround=\"2,2\" text=\"%s\" tooltip=\"%s\"",
		lpText, lpText);
	pTabItem->ApplyAttributeList(strAttr);
	pTabItem->SetParent(this);
	if (!Add(pTabItem))
	{
		delete pTabItem;
		return NULL;
	}
	pTabItem->ShowCloseBtn(bShowClose);
	if (m_pTabItems.empty())
		m_nSelIndex = 0;
	m_pTabItems.push_back(pTabItem);
	if (bReset)
		ResetTabPos();
	return pTabItem;
}

void CTabBarUI::SelectItem(CControlUI* pItem)
{
	if (m_nSelIndex >= 0 && m_nSelIndex < m_pTabItems.size())
	{
		CControlUI* pCurItem = m_pTabItems[m_nSelIndex];
		if (pCurItem)
			pCurItem->SetBkColor(m_dwTabNorColor);
	}
	pItem->SetBkColor(m_dwTabSelColor);
	int nUnSelIndex = m_nSelIndex;
	m_nSelIndex = GetTabItemIndex(pItem);
	if (m_pCallback)
		m_pCallback->OnItemSelectChange(this, nUnSelIndex, m_nSelIndex);
}

void CTabBarUI::DeleteItem( CControlUI* pItem )
{
	int nDelIndex = GetTabItemIndex(pItem);
	if ( nDelIndex == -1 )
		return ;
	DeleteItem(nDelIndex);
}

void CTabBarUI::DeleteItem( const size_t nIndex )
{
	if ( nIndex<0 || nIndex>=m_pTabItems.size() )
		return ;
	Remove(m_pTabItems[nIndex]);
	m_pTabItems.erase(m_pTabItems.begin()+nIndex);
	int nCount = m_pTabItems.size();
	if ( nCount>0 )
	{
		if ( nIndex<m_nSelIndex )
			m_nSelIndex--;
		else if ( nIndex == m_nSelIndex )
		{
			m_nSelIndex = nCount-1;
			CControlUI* pItem = m_pTabItems[m_nSelIndex];
			if ( pItem )
				pItem->SetBkColor(m_dwTabSelColor);
		}
		ResetTabPos();
	}
	else
		m_nSelIndex = -1;
	if ( m_pCallback )
		m_pCallback->OnItemClose(this, nIndex, m_nSelIndex);
}

void CTabBarUI::ResetTabPos()
{
	int nCount = m_pTabItems.size();
	if ( nCount == 0 )
	{
		RECT rcItem = {m_rcItem.left, m_rcItem.top, 25, m_rcItem.bottom};
		if (m_bShowAdd)
			m_pBtnAdd->SetPos(rcItem);
		return ;
	}
	int nWidth = m_rcItem.right - m_rcItem.left-25;
	int nHeight= m_rcItem.bottom - m_rcItem.top;
	if ( nWidth/nCount < m_nMinWidth )
		return ;
	if ( nCount*m_nNorWidth <= nWidth )
		m_nCurWidth = m_nNorWidth;
	else
		m_nCurWidth = nWidth/nCount;
	RECT rcItem;
	for( size_t i=0; i<m_pTabItems.size(); ++i )
	{
		rcItem.left	= m_nCurWidth*i + m_rcItem.left;
		rcItem.top	= 0 + m_rcItem.top;
		rcItem.right= rcItem.left + m_nCurWidth;
		rcItem.bottom = rcItem.top + nHeight;
		CControlUI* pItem = m_pTabItems[i];
		pItem->SetPos(rcItem);
		if ( m_nSelIndex == i )
			pItem->SetBkColor(m_dwTabSelColor);
		else
			pItem->SetBkColor(m_dwTabNorColor);
	}
	rcItem.left = rcItem.right;
	rcItem.right = rcItem.left + 25;
	if (m_bShowAdd)
		m_pBtnAdd->SetPos(rcItem);
}

void CTabBarUI::SetItemText(const size_t nIndex, LPCTSTR lpText)
{
	size_t nCount = m_pTabItems.size();
	if ( 0 == nCount || nCount <= nIndex )
		return ;
	CControlUI* pItem = m_pTabItems[nIndex];
	if ( pItem )
	{
		pItem->SetText(lpText);
		pItem->SetToolTip(lpText);
	}
}

int CTabBarUI::GetTabItemIndex( CControlUI* pItem )
{
	int nIndex = -1;
	for ( size_t i=0; i<m_pTabItems.size(); ++i )
	{
		if ( m_pTabItems[i] == pItem )
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

bool CTabBarUI::OnBtnClickAdd( void* pParam )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if(pNotifyUI->sType != DUI_MSGTYPE_CLICK) 
		return true;
	if (m_pCallback)
		m_pCallback->OnItemAdd(this);
	return true;
}

void CTabBarUI::SetPos( RECT rc )
{
	CContainerUI::SetPos(rc);
	ResetTabPos();
}

bool CTabBarUI::OnNotifySelf(void* param)
{
	TNotifyUI* pNotify = (TNotifyUI*)param;
	if (pNotify->sType == TAB_ITEM_CLOSE)
	{
		CControlUI* pItem = (CControlUI*)pNotify->lParam;
		if (pItem)
			DeleteItem(pItem);
	}
	else if (pNotify->sType == TAB_ITEM_CLICK)
	{
		CControlUI* pItem = (CControlUI*)pNotify->lParam;
		if (pItem)
			SelectItem(pItem);
	}
	else if (pNotify->sType == TAB_ITEM_DBCLICK)
	{
		if (m_pCallback)
			m_pCallback->OnItemDbClick(this, m_nSelIndex);
	}
	return true;
}



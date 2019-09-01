#include "StdAfx.h"
#include "UIOptionLayout.h"


namespace DuiLib
{
	COptionLayoutUI::COptionLayoutUI(void)
		:m_nSelectIndex(-1)
	{
	}

	COptionLayoutUI::~COptionLayoutUI(void)
	{
	}

	void COptionLayoutUI::ItemSelect( CControlUI* pItem )
	{
		int nIndex=GetOptionIndex(pItem);
		SelectItem(nIndex);
	}

	int COptionLayoutUI::GetOptionIndex( CControlUI* pItem )
	{
		for ( size_t i=0; i<m_pOptionItems.size(); ++i )
		{
			if ( m_pOptionItems[i] == pItem )
				return (int)i;
		}
		return -1;
	}

	void COptionLayoutUI::SelectItem( const int nIndex )
	{
		int nCount=(int)m_pOptionItems.size();
		if ( nIndex<0 || nIndex>nCount )
			return ;
		if ( nIndex == m_nSelectIndex )
			return ;
		int nExpandHeight=m_pHorizItems[nIndex]->GetFixedHeight();
		int nTopPos=0, nHeight=0, nWidth=0;
		for ( int i=0; i<nCount; ++i )
		{
			SIZE szPos=m_pOptionItems[i]->GetFixedXY();
			nHeight=m_pOptionItems[i]->GetFixedHeight();
			szPos.cy=nTopPos;
			m_pOptionItems[i]->SetFixedXY(szPos);
			nTopPos+=nHeight;
			if ( i == nIndex )
				nTopPos+=nExpandHeight;
		}
		if ( m_nSelectIndex != -1 )
			m_pHorizItems[m_nSelectIndex]->SetVisible(false);
		m_pHorizItems[nIndex]->SetVisible();
		COptioneExUI* pSelectOoption=(COptioneExUI*)(m_pOptionItems[nIndex]->GetInterface(DUI_CTR_OPTIONEX));
		if ( pSelectOoption )
			pSelectOoption->Selected(true);
		m_nSelectIndex=nIndex;
		NeedUpdate();
	}

	bool COptionLayoutUI::Add( CControlUI* pControl )
	{
		if ( pControl->GetInterface(DUI_CTR_OPTION) )
		{
			((COptioneExUI*)pControl)->SetOptionLayoutCallback(this);
			m_pOptionItems.push_back(pControl);
		}
		else if ( pControl->GetInterface(DUI_CTR_HORIZONTALLAYOUT) )
		{
			pControl->SetVisible(false);
			m_pHorizItems.push_back(pControl);
		}
		return CVerticalLayoutUI::Add(pControl);
	}

	void COptionLayoutUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if ( _tcscmp(pstrName, L"selectindex")==0 )
		{
			int nIndex=_ttoi(pstrValue);
			SelectItem(nIndex);
		}
		else
			CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
	}	

	LPVOID COptionLayoutUI::GetInterface( LPCTSTR pstrName )
	{
		if ( _tcscmp(pstrName, DUI_CTR_OPTIONLAYOUT)==0 )
			return static_cast<COptionLayoutUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

}

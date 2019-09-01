#include "StdAfx.h"
#include "UIOptioneEx.h"

namespace DuiLib
{
	COptioneExUI::COptioneExUI(void)
		:m_pCallback(NULL)
	{
	}

	COptioneExUI::~COptioneExUI(void)
	{
	}

	void COptioneExUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() && m_pCallback )
				m_pCallback->ItemSelect(this);
		}
		COptionUI::DoEvent(event);
	}

	void COptioneExUI::SetOptionLayoutCallback( COptionLayoutCallback* pCallback )
	{
		m_pCallback=pCallback;
	}

	LPVOID COptioneExUI::GetInterface( LPCTSTR pstrName )
	{
		if ( _tcscmp(pstrName, DUI_CTR_OPTIONEX)==0 )
			return static_cast<COptioneExUI*>(this);
		return COptionUI::GetInterface(pstrName);
	}

}

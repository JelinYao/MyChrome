#include "StdAfx.h"
#include "UIFadeButton.h"

namespace DuiLib {

	CFadeButtonUI::CFadeButtonUI() :  
		m_bMouseHove(false), 
		m_bMouseLeave(false),
		m_hasFadeEffect(true)
	{
	}

	CFadeButtonUI::~CFadeButtonUI()
	{
		StopAnimation();
		CControlUI::~CControlUI();
	}

	LPCTSTR CFadeButtonUI::GetClass() const
	{
		return _T("FadeButtonUI");
	}

	LPVOID CFadeButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("FadeButtonUI")) == 0 ) 
			return static_cast<CFadeButtonUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void CFadeButtonUI::DoEvent(TEventUI& event)
	{
		if (!m_hasFadeEffect)
		{
			CButtonUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_MOUSEENTER && !IsAnimationRunning( FADE_IN_ID ) )
		{
			m_bFadeAlpha = 0;
			m_bMouseHove = TRUE;
			StopAnimation( FADE_OUT_ID );
			StartAnimation( FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_IN_ID );
			Invalidate();
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE && !IsAnimationRunning( FADE_OUT_ID ) )
		{
			m_bFadeAlpha = 0;
			m_bMouseLeave = TRUE;
			StopAnimation(FADE_IN_ID);
			StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_OUT_ID);
			Invalidate();
			return;
		}
		if( event.Type == UIEVENT_TIMER ) 
		{
			OnTimer(  event.wParam );
		}
		CButtonUI::DoEvent( event );
	}

	void CFadeButtonUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("fadeeffect")) == 0 )
		{
			SetFadeEffect( _tcscmp(pstrName, _T("true")) == 0);
		}

		CButtonUI::SetAttribute(pstrName,pstrValue);
	}

	void CFadeButtonUI::SetFadeEffect(bool hasFadeEffect)
	{
		m_hasFadeEffect = hasFadeEffect;
	}

	void CFadeButtonUI::OnTimer( int nTimerID )
	{
		OnAnimationElapse( nTimerID );
	}

	void CFadeButtonUI::PaintStatusImage(HDC hDC)
	{
		if (!m_hasFadeEffect)
		{
			CButtonUI::PaintStatusImage(hDC);
			return;
		}

		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if( !m_sPushedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( IsAnimationRunning(FADE_IN_ID) || IsAnimationRunning(FADE_OUT_ID))
			{
				if( m_bMouseHove )
				{
					m_bMouseHove = FALSE;
					m_sLastImage = m_sHotImage;
					if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) 
						m_sNormalImage.Empty();
					return;
				}

				if( m_bMouseLeave )
				{
					m_bMouseLeave = FALSE;
					m_sLastImage = m_sNormalImage;
					if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) 
						m_sHotImage.Empty();
					return;
				}

				m_sOldImage = m_sNormalImage;
				m_sNewImage = m_sHotImage;
				if( IsAnimationRunning(FADE_OUT_ID) )
				{
					m_sOldImage = m_sHotImage;
					m_sNewImage = m_sNormalImage;
				}

				m_sOldImage = UpdateFadeAttribute(m_sOldImage, 255 - m_bFadeAlpha);
				m_sNewImage = UpdateFadeAttribute(m_sNewImage, m_bFadeAlpha);

				if( !DrawImage(hDC, (LPCTSTR)m_sOldImage, NULL) ) 
					m_sOldImage.Empty();
				if( !DrawImage(hDC, (LPCTSTR)m_sNewImage, NULL) ) 
					m_sNewImage.Empty();
				return;
			}
			else
			{
				if (m_sLastImage.IsEmpty())
				{
					m_sLastImage = m_sNormalImage;
				}

				if( !DrawImage(hDC, (LPCTSTR)m_sLastImage) ) 
					m_sLastImage.Empty();
				return;
			}
		}
	}

	CControlUI* CFadeButtonUI::OwnerControlUI()
	{
		return static_cast<CControlUI*>(this);
	}

	void CFadeButtonUI::OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID)
	{
		m_bFadeAlpha = (byte)(nCurFrame* 255.0 / nTotalFrame);
		if(m_bFadeAlpha == 0) 
			m_bFadeAlpha = 10;
		Invalidate();
	}

	CDuiString CFadeButtonUI::UpdateFadeAttribute(const CDuiString& imageAttribute, int fadeAlpha)
	{
		CDuiString updateImageAttribute = imageAttribute;
		if (updateImageAttribute.Find(L"file='") == -1)
		{
			updateImageAttribute = L"file='";
			updateImageAttribute += imageAttribute;
			updateImageAttribute += L"' ";
		}

		if (int startFadePos = imageAttribute.Find(L"fade='") != -1)
		{
			int endFadePos = imageAttribute.Find(L"'", startFadePos + 6);
			ASSERT(endFadePos == -1);
			updateImageAttribute = imageAttribute.Mid(startFadePos, endFadePos - startFadePos);
		}

		updateImageAttribute += L" fade='%d' ";
		updateImageAttribute.Format(updateImageAttribute.GetData(), fadeAlpha);

		return updateImageAttribute;
	}

} // namespace DuiLib
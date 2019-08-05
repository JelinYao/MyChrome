#ifndef __UIFADEBUTTON_H__
#define __UIFADEBUTTON_H__

#pragma once

#include "UIAnimation.h"

namespace DuiLib {

	class CButtonUI;

	class UILIB_API CFadeButtonUI : public CButtonUI, public CUIAnimation
	{
	public:
		CFadeButtonUI();
		virtual ~CFadeButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetFadeEffect(bool hasFadeEffect);

		void OnTimer(int nTimerID);
		void PaintStatusImage(HDC hDC);

		virtual CControlUI* OwnerControlUI();
		virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) {}
		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID);
		virtual void OnAnimationStop(int nAnimationID) {}
		
	protected:
		CDuiString m_sOldImage;
		CDuiString m_sNewImage;
		CDuiString m_sLastImage;
		byte          m_bFadeAlpha;
		bool        m_bMouseHove;
		bool        m_bMouseLeave;
		enum{
			FADE_IN_ID    = 8,
			FADE_OUT_ID = 9,

			FADE_ELLAPSE = 10,
			FADE_FRAME_COUNT = 50,
		};

	private:
		static CDuiString UpdateFadeAttribute(const CDuiString& imageAttribute, int fadeAlpha);

	private:
		bool m_hasFadeEffect;
	};

} // namespace DuiLib

#endif // __UIFADEBUTTON_H__
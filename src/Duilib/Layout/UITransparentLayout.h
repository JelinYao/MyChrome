#ifndef __UITRANSPARENTLAYOUT_H__
#define __UITRANSPARENTLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTransparentLayoutUI : public CContainerUI
	{
	public:
		CTransparentLayoutUI();
		~CTransparentLayoutUI();
		LPCTSTR GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		UINT	GetControlFlags() const;
		void	SetAlphaValue(const BYTE bAlpha);
		BYTE	GetAlphaValue()const;
		void SetSepWidth(int iWidth);
		int GetSepWidth() const;
		void SetSepImmMode(bool bImmediately);
		bool IsSepImmMode() const;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void DoEvent(TEventUI& event);

		void SetPos(RECT rc);
		void DoPostPaint(HDC hDC, const RECT& rcPaint);

		RECT GetThumbRect(bool bUseNew = false) const;
		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	protected:
		int		m_iSepWidth;
		UINT	m_uButtonState;
		POINT	ptLastMouse;
		RECT	m_rcNewPos;
		bool	m_bImmMode;
		BYTE	m_bAlpha;
		HDC		m_hMemDC;
		HBITMAP	m_hMemBitmap;
	};
}
#endif // __UITRANSPARENTLAYOUT_H__

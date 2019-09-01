#ifndef __UICUSTOMEDIT_H__
#define __UICUSTOMEDIT_H__

#pragma once

namespace DuiLib
{
	//光标窗口
	interface UILIB_API ICaret
	{
		virtual ~ICaret() = 0{};
		virtual BOOL  Create(CControlUI* pControl, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight) = 0;
		virtual void  Destroy(bool bRedraw) = 0;
		virtual void  Show(bool bRedraw) = 0;
		virtual void  Hide(bool bRedraw) = 0;
		virtual void  SetPos(int x, int y, bool bRedraw) = 0;
		virtual void  OnControlPaint(HDC hDC) = 0;
		virtual CControlUI*  GetControlUI() = 0;
	};

	class UILIB_API ControlDrawCaret : public ICaret
	{
	public:
		ControlDrawCaret();
		~ControlDrawCaret();

		virtual BOOL  Create(CControlUI* pControl, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
		virtual void  Destroy(bool bRedraw);
		virtual void  SetPos(int x, int y, bool bRedraw);
		virtual void  Show(bool bRedraw);
		virtual void  Hide(bool bRedraw);
		virtual void  OnControlPaint(HDC hDC);
		virtual CControlUI*  GetControlUI() { return m_pControl; }

		void  OnTimer();
	protected:
		void  CreateTimer();
		void  DestroyTimer();

	private:
		HWND   m_hWnd;
		HBITMAP  m_hBitmap;
		CDuiImage*  m_pRenderBitmap;

		CControlUI*  m_pControl;
		POINT  m_ptLast;
		SIZE   m_sizeCaret;

		CDuiRect  m_rcDraw;
		UINT_PTR  m_nTimerId;
		bool   m_bOn;  // 当前是否调用了ShowCaret
		bool   m_bVisible; // 记录光标的闪烁状态
	};

	class UILIB_API CCaret
	{
	public:
		CCaret();
		~CCaret();

		BOOL  CreateCaret(CControlUI* pControl, HBITMAP hbmp, int nWidth, int nHeight);
		void  DestroyCaret(CControlUI* pControl, bool bRedraw);
		void  SetCaretPos(CControlUI* pControl, int x, int y, bool bRedraw=true);
		void  ShowCaret(CControlUI* pControl, bool bRedraw=true);
		void  HideCaret(CControlUI* pControl, bool bRedraw=true);
		void  OnControlPaint(CControlUI* pControl, HDC hDC);

	public:
		static ICaret*  m_pCaret;  // 全局唯一  //因为光标是有焦点的时候才有 那么只会有一个窗口才有焦点
		static POINT  m_ptLast;
	};

	class UILIB_API CCustomEditUI : public CLabelUI
	{
	public:
		CCustomEditUI();
		~CCustomEditUI();//析构函数

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		void SetNativeEditBkColor(DWORD dwBkColor);
		DWORD GetNativeEditBkColor() const;

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		void SetPos(RECT rc);
		RECT CalEditPos();
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		void SetBackText(LPCTSTR pstrValue);
		void SetBacTextkColor(DWORD dwColor);

		
	protected:

		UINT m_uMaxChar;
		bool m_bReadOnly;
		bool m_bPasswordMode;
		TCHAR m_cPasswordChar;
		UINT m_uButtonState;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;
		DWORD m_dwEditbkColor;
		int m_iWindowStyls;

		CDuiString m_szBackText;
		DWORD m_dwBackTextColor;
	};
}
#endif // __UICUSTOMEDIT_H__
#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__

#pragma once

namespace DuiLib
{
	class IHotKeyCallbackUI
	{
	public:
		virtual void RepeatHotKey(HWND hWnd)
		{
			::MessageBox(hWnd,_T("热键控件重复！请您重新输入！"),_T("提示"),0);
		};
	};

	class CHotKeyWnd;

	class UILIB_API CHotKeyUI : public CLabelUI
	{
		friend class CHotKeyWnd;
	public:
		CHotKeyUI();
		~CHotKeyUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
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
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		void SetSysKey(bool bSysKey = true);
		bool IsSysKey() const;

		CDuiString GetKeyASCIIToText(UINT nChar = 0);

		void SetCtrlKeyDown(bool bEmpty = false);
		void SetShiftKeyDown(bool bEmpty = false);
		void SetAltKeyDown(bool bEmpty = false);
		void SetKey(UINT nKey);

		UINT GetCtrlKeyDown() ;
		UINT GetShiftKeyDown() ;
		UINT GetAltKeyDown() ;
		UINT GetKey() ;

		void SetOldCtrlKeyDown();
		void SetOldShiftKeyDown();
		void SetOldAltKeyDown();
		void SetOldKey(UINT nKey);

		UINT GetOldCtrlKeyDown() ;
		UINT GetOldShiftKeyDown() ;
		UINT GetOldAltKeyDown() ;
		UINT GetOldKey() ;

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);

		void SetCharOnly(bool bChar = true);
		void SetNumberOnly(bool bNumber = true);

		bool IsCharOnly() const;
		bool IsNumberOnly() const;

		bool CheckHotKey(CDuiString szText,bool bSendNotify = false);

		IHotKeyCallbackUI* GetHotKeyCallback() const;
		void SetHotKeyCallback(IHotKeyCallbackUI* pCallback); // 同分组下的热键重复回调函数

	protected:
		CHotKeyWnd* m_pWindow;

		UINT m_uMaxChar;
		bool m_bReadOnly;
		UINT m_uButtonState;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;
		DWORD m_dwEditbkColor;
		int m_iWindowStyls;

		bool m_bSysKey;//是否允许使用系统键
		bool m_bChar; //是否只支持字符
		bool m_bNumber; //是否只支持数字
		UINT m_nCtrlKeyDown;
		UINT m_nShiftKeyDown;
		UINT m_nAltKeyDown;
		UINT m_nKey;

		UINT m_nOldCtrlKeyDown;
		UINT m_nOldShiftKeyDown;
		UINT m_nOldAltKeyDown;
		UINT m_nOldKey;

		CDuiString		m_sGroupName;
		IHotKeyCallbackUI* m_pCallback;
	};
}
#endif // __UIHOTKEY_H__
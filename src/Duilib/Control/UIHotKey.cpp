#include "stdafx.h"
#include "UIHotKey.h"

namespace DuiLib
{
	class CHotKeyWnd : public CWindowWnd
	{
	public:
		CHotKeyWnd();

		void Init(CHotKeyUI* pOwner);
		RECT CalPos();
		void InitHotKey();
		void UpdateHotKey();
		void SetHotKeyFromOldHotKey();
		CDuiString GetHotKey();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CHotKeyUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};


	CHotKeyWnd::CHotKeyWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void CHotKeyWnd::Init(CHotKeyUI* pOwner)
	{
		m_pOwner = pOwner;
		RECT rcPos = CalPos();
		//UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
		UINT uStyle = WS_POPUP | ES_AUTOHSCROLL | WS_VISIBLE;
		RECT rcWnd={0};
		::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
		rcPos.left += rcWnd.left;
		rcPos.right += rcWnd.left;
		rcPos.top += rcWnd.top;
		rcPos.bottom += rcWnd.top;

		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
		HFONT hFont=NULL;
		int iFontIndex=m_pOwner->GetFont();
		if (iFontIndex!=-1)
			hFont=m_pOwner->GetManager()->GetFont(iFontIndex);
		if (hFont==NULL)
			hFont=m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;

		SetWindowFont(m_hWnd, hFont, TRUE);
		Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
		Edit_SetText(m_hWnd, m_pOwner->GetText());
		Edit_SetModify(m_hWnd, FALSE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
		Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
		//Styls
		LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
		styleValue |= pOwner->GetWindowStyls();
		::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);
		m_bInit = true;    
	}

	RECT CHotKeyWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		RECT rcInset = m_pOwner->GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right;
		rcPos.bottom -= rcInset.bottom;
		LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
		if( lEditHeight < rcPos.GetHeight() ) {
			rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}
		return rcPos;
	}

	void CHotKeyWnd::InitHotKey()
	{
		m_pOwner->SetCtrlKeyDown(true);
		m_pOwner->SetShiftKeyDown(true);
		m_pOwner->SetAltKeyDown(true);
		m_pOwner->SetKey(0);
		Edit_SetText(m_hWnd,L"");
	}

	void CHotKeyWnd::SetHotKeyFromOldHotKey()
	{
		if (m_pOwner->GetOldAltKeyDown() != NULL)
			m_pOwner->SetAltKeyDown();
		else
			m_pOwner->SetAltKeyDown(true);

		if (m_pOwner->GetOldCtrlKeyDown() != NULL)
			m_pOwner->SetCtrlKeyDown();
		else
			m_pOwner->SetCtrlKeyDown(true);

		if (m_pOwner->GetOldShiftKeyDown() != NULL)
			m_pOwner->SetShiftKeyDown();
		else
			m_pOwner->SetShiftKeyDown(true);

		m_pOwner->SetKey(m_pOwner->GetOldKey());
	}

	CDuiString CHotKeyWnd::GetHotKey()
	{
		CDuiString szText = _T("");
		CDuiString szKey = _T("");
		szKey = m_pOwner->GetKeyASCIIToText();
		if (m_pOwner->IsSysKey() == false)
		{
			if (m_pOwner->GetCtrlKeyDown() != NULL)
				szText += _T("Ctrl + ");
			if (m_pOwner->GetShiftKeyDown() != NULL)
				szText += _T("Shift + ");
			if (m_pOwner->GetAltKeyDown() != NULL)
				szText += _T("Atl + ");
		}
		if (!szKey.IsEmpty())
		{
			szText += szKey;
		}
		else
		{
			m_pOwner->SetKey(0);
		}
		return szText;
	}

	void CHotKeyWnd::UpdateHotKey()
	{
		CDuiString szText = GetHotKey();
		if (m_pOwner->GetKey() > 0)
		{//当快捷键 有键值的时候比较是否重复
			if (m_pOwner->CheckHotKey(szText) == true)
			{//如果检查有重复
				IHotKeyCallbackUI* pCallBakc = m_pOwner->GetHotKeyCallback();
				if (pCallBakc)
					pCallBakc->RepeatHotKey(GetParent(m_hWnd));
				SetHotKeyFromOldHotKey();//获取上一次的快捷键
				szText = GetHotKey();
				if (m_pOwner->CheckHotKey(szText,true) == true)
				{//再次检查重复 清空
					szText = _T("");
				}
			}
		}
		//检查 同一组 的 是否有重复
		Edit_SetText(m_hWnd, szText.GetData());
		return ;
		InitHotKey();
	}

	LPCTSTR CHotKeyWnd::GetWindowClassName() const
	{
		return _T("HotKeyWnd");
	}

	LPCTSTR CHotKeyWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	void CHotKeyWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		m_pOwner->Invalidate();
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		m_pOwner->m_pWindow = NULL;
		delete this;
	}

	LRESULT CHotKeyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		if(uMsg == WM_CHAR) return lRes; //取消字符输出
		BOOL bHandled = TRUE;
		if ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
		{
			//按键消息捕获 保存上次按键记录
			if (m_pOwner->GetCtrlKeyDown() != NULL)
				m_pOwner->SetOldCtrlKeyDown();
			if (m_pOwner->GetShiftKeyDown() != NULL)
				m_pOwner->SetOldShiftKeyDown();
			if (m_pOwner->GetAltKeyDown() != NULL)
				m_pOwner->SetOldAltKeyDown();
			UINT nKey = m_pOwner->GetKey();
			m_pOwner->SetKey(0);
			if (nKey > 0)
				m_pOwner->SetOldKey(nKey);

			//保存本次按键记录
			if (::GetAsyncKeyState(VK_MENU)<0)
				m_pOwner->SetAltKeyDown();
			else
				m_pOwner->SetAltKeyDown(true);
			if (::GetAsyncKeyState(VK_SHIFT)<0)
				m_pOwner->SetShiftKeyDown();
			else
				m_pOwner->SetShiftKeyDown(true);
			if (::GetAsyncKeyState(VK_CONTROL)<0)
				m_pOwner->SetCtrlKeyDown();
			else
				m_pOwner->SetCtrlKeyDown(true);

			if (wParam>0 && wParam != VK_CONTROL && wParam != VK_SHIFT && wParam != VK_MENU)
			{
				m_pOwner->SetKey((UINT)wParam);
			}
			//展示文本
			UpdateHotKey();
			return -1;
		}
		if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
		{
			UINT nKey = (UINT)wParam;
			UINT nKey1 = m_pOwner->GetKey();
			if (nKey1 < 1)
			{
				//保存本次按键记录
				if (::GetAsyncKeyState(VK_MENU)<0)
					m_pOwner->SetAltKeyDown();
				else
					m_pOwner->SetAltKeyDown(true);
				if (::GetAsyncKeyState(VK_SHIFT)<0)
					m_pOwner->SetShiftKeyDown();
				else
					m_pOwner->SetShiftKeyDown(true);
				if (::GetAsyncKeyState(VK_CONTROL)<0)
					m_pOwner->SetCtrlKeyDown();
				else
					m_pOwner->SetCtrlKeyDown(true);

				UpdateHotKey();
			}
			return 0;
		}
		if( uMsg == WM_KILLFOCUS ) lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}
		}
		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
			if( m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
			::SetBkMode((HDC)wParam, TRANSPARENT);
			DWORD dwTextColor = m_pOwner->GetTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
			if( m_hBkBrush == NULL ) {
				DWORD clrColor = m_pOwner->GetNativeEditBkColor();
				m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
			}
			return (LRESULT)m_hBkBrush;
		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CHotKeyWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		PostMessage(WM_CLOSE);
		return lRes;
	}

	LRESULT CHotKeyWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if( !m_bInit ) return 0;
		if( m_pOwner == NULL ) return 0;
		// Copy text back
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		m_pOwner->m_sText = pstr;
		m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		return 0;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CHotKeyUI::CHotKeyUI() : m_pWindow(NULL), m_uMaxChar(255), m_bReadOnly(false), m_uButtonState(0), 
		m_dwEditbkColor(0xFFFFFFFF), m_iWindowStyls(0)
	{
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		SetBkColor(0xFFFFFFFF);
		SetCtrlKeyDown(true);
		SetShiftKeyDown(true);
		SetAltKeyDown(true);
		SetKey(0);
		m_bSysKey = false;
		m_sGroupName = _T("");
		m_bChar = false;
		m_bNumber = false;
		m_pCallback = NULL;
	}

	CHotKeyUI::~CHotKeyUI()
	{
		if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveHotKeyGroup(m_sGroupName, this);
	}

	LPCTSTR CHotKeyUI::GetClass() const
	{
		return _T("HotKeyUI");
	}

	LPVOID CHotKeyUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_HOTKEY) == 0 ) return static_cast<CHotKeyUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CHotKeyUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CControlUI::SetManager(pManager, pParent, bInit);
		if( bInit && !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddHotKeyGroup(m_sGroupName, this);
		}
	}

	UINT CHotKeyUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void CHotKeyUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindow != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if( m_pWindow ) return;
			m_pWindow = new CHotKeyWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this);
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && m_pWindow == NULL )
				{
					m_pWindow = new CHotKeyWnd();
					ASSERT(m_pWindow);
					m_pWindow->Init(this);

					if( PtInRect(&m_rcItem, event.ptMouse) )
					{
						int nSize = GetWindowTextLength(*m_pWindow);
						if( nSize == 0 )
							nSize = 1;

						Edit_SetSel(*m_pWindow, 0, nSize);
					}
				}
				else if( m_pWindow != NULL )
				{
#if 1
					int nSize = GetWindowTextLength(*m_pWindow);
					if( nSize == 0 )
						nSize = 1;

					Edit_SetSel(*m_pWindow, 0, nSize);
#else
					POINT pt = event.ptMouse;
					pt.x -= m_rcItem.left + m_rcTextPadding.left;
					pt.y -= m_rcItem.top + m_rcTextPadding.top;
					::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CLabelUI::DoEvent(event);
	}

	void CHotKeyUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	void CHotKeyUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_sText);
		Invalidate();
	}

	void CHotKeyUI::SetMaxChar(UINT uMax)
	{
		m_uMaxChar = uMax;
		if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
	}

	UINT CHotKeyUI::GetMaxChar()
	{
		return m_uMaxChar;
	}

	void CHotKeyUI::SetReadOnly(bool bReadOnly)
	{
		if( m_bReadOnly == bReadOnly ) return;

		m_bReadOnly = bReadOnly;
		if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
		Invalidate();
	}

	bool CHotKeyUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	int CHotKeyUI::GetWindowStyls() const 
	{
		return m_iWindowStyls;
	}

	LPCTSTR CHotKeyUI::GetNormalImage()
	{
		return m_sNormalImage;
	}

	void CHotKeyUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CHotKeyUI::GetHotImage()
	{
		return m_sHotImage;
	}

	void CHotKeyUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CHotKeyUI::GetFocusedImage()
	{
		return m_sFocusedImage;
	}

	void CHotKeyUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CHotKeyUI::GetDisabledImage()
	{
		return m_sDisabledImage;
	}

	void CHotKeyUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	void CHotKeyUI::SetNativeEditBkColor(DWORD dwBkColor)
	{
		m_dwEditbkColor = dwBkColor;
	}

	DWORD CHotKeyUI::GetNativeEditBkColor() const
	{
		return m_dwEditbkColor;
	}

	void CHotKeyUI::SetSel(long nStartChar, long nEndChar)
	{
		if( m_pWindow != NULL ) Edit_SetSel(*m_pWindow, nStartChar,nEndChar);
	}

	void CHotKeyUI::SetSelAll()
	{
		SetSel(0,-1);
	}

	void CHotKeyUI::SetReplaceSel(LPCTSTR lpszReplace)
	{
		if( m_pWindow != NULL ) Edit_ReplaceSel(*m_pWindow, lpszReplace);
	}

	void CHotKeyUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if( m_pWindow != NULL ) {
			RECT rcPos = m_pWindow->CalPos();
			::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void CHotKeyUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	void CHotKeyUI::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	SIZE CHotKeyUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CHotKeyUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("syskey")) == 0 ) SetSysKey(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("charonly")) == 0 ) SetCharOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcscmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("nativebkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetNativeEditBkColor(clrColor);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CHotKeyUI::PaintStatusImage(HDC hDC)
	{
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
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
				else return;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else return;
		}
	}

	void CHotKeyUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;

		CDuiString sText = m_sText;

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top + 2;
		rc.bottom -= m_rcTextPadding.bottom + 2;
		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);

		}
	}

	void CHotKeyUI::SetSysKey(bool bSysKey)
	{
		if (bSysKey == m_bSysKey) return;
		m_bSysKey = bSysKey;
	}

	bool CHotKeyUI::IsSysKey() const
	{
		return m_bSysKey;
	}

	CDuiString CHotKeyUI::GetKeyASCIIToText(UINT nChar)
	{
		if (nChar == 0)
		{
			nChar = m_nKey;
		}
		if (nChar == 0)
		{
			return _T("");
		}
		CDuiString szKey = _T("");
		if (nChar>='A'&&nChar<='Z')
		{
			if (!IsCharOnly() && IsNumberOnly())
			{
				return szKey;
			}
			TCHAR   lpszFile[2];
			lpszFile[0]=nChar;
			lpszFile[1]='\0';
			szKey.Format(_T("%s"),lpszFile);
		}
		else if (nChar>='0'&&nChar<='9')
		{
			if (IsCharOnly() && !IsNumberOnly())
			{
				return szKey;
			}
			TCHAR   lpszFile[2];
			lpszFile[0]=nChar;
			lpszFile[1]='\0';
			szKey.Format(_T("%s"),lpszFile);
		}
		else
		{
			if (IsCharOnly() || IsNumberOnly())
			{
				return szKey;
			}
			switch(nChar)
			{
			case 32:
				szKey = _T("Space");
				break;
			case 27:
				szKey = _T("Esc");
				break;
			case 192:
				szKey = _T("~");
				break;
			case 112:
				szKey = _T("F1");
				break;
			case 113:
				szKey = _T("F2");
				break;
			case 114:
				szKey = _T("F3");
				break;
			case 115:
				szKey = _T("F4");
				break;
			case 116:
				szKey = _T("F5");
				break;
			case 117:
				szKey = _T("F6");
				break;
			case 118:
				szKey = _T("F7");
				break;
			case 119:
				szKey = _T("F8");
				break;
			case 120:
				szKey = _T("F9");
				break;
			case 121:
				szKey = _T("F10");
				break;
			case 122:
				szKey = _T("F11");
				break;
			case 123:
				szKey = _T("F12");
				break;
			case 189:
				szKey = _T("-");
				break;
			case 187:
				szKey = _T("=");
				break;
			case 8:
				szKey = _T("Backspace");
				break;
			case 219:
				szKey = _T("[");
				break;
			case 13:
				szKey = _T("Enter");
				break;
			case 220:
				szKey = _T("\\");
				break;
			case 221:
				szKey = _T("]");
				break;
			case 222:
				szKey = _T("'");
				break;
			case 186:
				szKey = _T(";");
				break;
			case 188:
				szKey = _T(",");
				break;
			case 190:
				szKey = _T(".");
				break;
			case 191:
				szKey = _T("/");
				break;
			case 37:
				szKey = _T("←");
				break;
			case 38:
				szKey = _T("↑");
				break;
			case 39:
				szKey = _T("→");
				break;
			case 40:
				szKey = _T("↓");
				break;
			case 45:
				szKey = _T("Insert");
				break;
			case 46:
				szKey = _T("Delete");
				break;
			case 33:
				szKey = _T("Page Up");
				break;
			case 34:
				szKey = _T("Page Down");
				break;
			case 35:
				szKey = _T("End");
				break;
			case 36:
				szKey = _T("Home");
				break;
			case 96:
				szKey = _T("Num0");
				break;
			case 97:
				szKey = _T("Num1");
				break;
			case 98:
				szKey = _T("Num2");
				break;
			case 99:
				szKey = _T("Num3");
				break;
			case 100:
				szKey = _T("Num4");
				break;
			case 101:
				szKey = _T("Num5");
				break;
			case 102:
				szKey = _T("Num6");
				break;
			case 103:
				szKey = _T("Num7");
				break;
			case 104:
				szKey = _T("Num8");
				break;
			case 105:
				szKey = _T("Num9");
				break;
			case 106:
				szKey = _T("Num*");
				break;
			case 107:
				szKey = _T("Num+");
				break;
			case 109:
				szKey = _T("Num-");
				break;
			case 111:
				szKey = _T("Num/");
				break;
			}
		}
		return szKey;
	}

	void CHotKeyUI::SetCtrlKeyDown(bool bEmpty)
	{
		if(bEmpty) 
		{
			m_nCtrlKeyDown = NULL;
			return ;
		}
		if (m_nCtrlKeyDown == VK_CONTROL) return;
		m_nCtrlKeyDown = VK_CONTROL;
	}

	void CHotKeyUI::SetShiftKeyDown(bool bEmpty)
	{
		if(bEmpty) 
		{
			m_nShiftKeyDown = NULL;
			return ;
		}
		if (m_nShiftKeyDown == VK_SHIFT) return;
		m_nShiftKeyDown = VK_SHIFT;
	}

	void CHotKeyUI::SetAltKeyDown(bool bEmpty )
	{
		if(bEmpty) 
		{
			m_nAltKeyDown = NULL;
			return ;
		}
		if(m_nAltKeyDown == VK_MENU) return;
		m_nAltKeyDown = VK_MENU;
	}

	void CHotKeyUI::SetKey(UINT nKey)
	{
		if( m_nKey == nKey) return ;
		m_nKey = nKey;
	}

	UINT CHotKeyUI::GetCtrlKeyDown() 
	{
		m_nOldCtrlKeyDown = NULL;
		return m_nCtrlKeyDown;
	}

	UINT CHotKeyUI::GetShiftKeyDown() 
	{
		m_nOldShiftKeyDown = NULL;
		return m_nShiftKeyDown;
	}

	UINT CHotKeyUI::GetAltKeyDown() 
	{
		m_nOldAltKeyDown = NULL;
		return m_nAltKeyDown;
	}

	UINT CHotKeyUI::GetKey() 
	{
		m_nOldKey = NULL;
		return m_nKey;
	}

	void CHotKeyUI::SetOldCtrlKeyDown()
	{
		if ( m_nOldCtrlKeyDown == VK_CONTROL) return ;
		m_nOldCtrlKeyDown = VK_CONTROL;
	}

	void CHotKeyUI::SetOldShiftKeyDown()
	{
		if (m_nOldShiftKeyDown == VK_SHIFT) return ;
		m_nOldShiftKeyDown = VK_SHIFT;
	}

	void CHotKeyUI::SetOldAltKeyDown()
	{
		if (m_nOldAltKeyDown == VK_MENU) return;
		m_nOldAltKeyDown = VK_MENU;
	}

	void CHotKeyUI::SetOldKey(UINT nKey)
	{
		if (m_nOldKey == nKey) return ;
		m_nOldKey = nKey;
	}

	UINT CHotKeyUI::GetOldCtrlKeyDown() 
	{
		return m_nOldCtrlKeyDown;
	}

	UINT CHotKeyUI::GetOldShiftKeyDown() 
	{
		return m_nOldShiftKeyDown;
	}

	UINT CHotKeyUI::GetOldAltKeyDown() 
	{
		return m_nOldShiftKeyDown;
	}

	UINT CHotKeyUI::GetOldKey() 
	{
		return m_nOldKey;
	}

	LPCTSTR CHotKeyUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void CHotKeyUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveHotKeyGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddHotKeyGroup(m_sGroupName, this);
		}
		else {
			if (m_pManager) m_pManager->RemoveHotKeyGroup(m_sGroupName, this);
		}
	}

	void CHotKeyUI::SetCharOnly(bool bChar)
	{
		if (m_bChar == bChar) return ;
		m_bChar = bChar;
	}
	void CHotKeyUI::SetNumberOnly(bool bNumber)
	{
		if (bNumber == m_bNumber) return ;
		m_bNumber = bNumber;
	}

	bool CHotKeyUI::IsCharOnly() const
	{
		return m_bChar;
	}
	bool CHotKeyUI::IsNumberOnly() const
	{
		return m_bNumber;
	}

	bool CHotKeyUI::CheckHotKey(CDuiString szText,bool bSendNotify)
	{
		bool bRet = false;
		CDuiString szTmpText = _T("");
		int nCount = 0;
		if( m_pManager != NULL && !szText.IsEmpty() ) 
		{
			if( !m_sGroupName.IsEmpty() ) 
			{
				CStdPtrArray* aHotKeyGroup = m_pManager->GetHotKeyGroup(m_sGroupName);
				for( int i = 0; i < aHotKeyGroup->GetSize(); i++ ) 
				{
					CHotKeyUI* pControl = static_cast<CHotKeyUI*>(aHotKeyGroup->GetAt(i));
					if( pControl != this ) 
					{
						szTmpText = pControl->GetText();
						if (szTmpText == szText)
						{
							nCount += 1;
							bRet = true;
						}
					}
					if (nCount > 0)
						break;
				}

			}
		}
		if (bSendNotify)
		{
			m_pManager->SendNotify(this, _T("hotkeychanged"));
		}
		return bRet;
	}

	IHotKeyCallbackUI* CHotKeyUI::GetHotKeyCallback() const
	{
		return m_pCallback;
	}

	void CHotKeyUI::SetHotKeyCallback(IHotKeyCallbackUI* pCallback)
	{
		m_pCallback = pCallback;
	}
}

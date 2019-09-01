#include "stdafx.h"
#include "UICustomEdit.h"

namespace DuiLib
{
	ControlDrawCaret::ControlDrawCaret()
	{
		m_hWnd = NULL;
		m_pControl = NULL;
		m_ptLast.x = m_ptLast.y = -1;
		m_sizeCaret.cx = m_sizeCaret.cy = 0;
		m_rcDraw.Empty();
		m_bVisible = false;
		m_bOn = false;
		m_nTimerId = 0;
		m_hBitmap = NULL;
		m_pRenderBitmap = NULL;
	}
	ControlDrawCaret::~ControlDrawCaret()
	{
		DestroyTimer();
		if(m_pRenderBitmap){
			if(!m_pRenderBitmap->IsNull()) m_pRenderBitmap->Destroy();
		}
		if(m_pRenderBitmap){
			delete m_pRenderBitmap;
			m_pRenderBitmap = NULL;
		}
	}

	BOOL  ControlDrawCaret::Create(CControlUI* pControl, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
	{
		Destroy(false);

		m_hWnd = hWndParent;
		m_pControl = pControl;
		m_sizeCaret.cx = nWidth;
		m_sizeCaret.cy = nHeight;
		m_hBitmap = hbmp;

		if (m_hBitmap > (HBITMAP)1)
		{
			BITMAP bm = {0};
			::GetObject(m_hBitmap, sizeof(bm), &bm);
			m_sizeCaret.cx = bm.bmWidth;
			m_sizeCaret.cy = bm.bmHeight;

			if(!m_pRenderBitmap){
				m_pRenderBitmap = new CDuiImage;
			}
			//CreateCaretRenderBitmap(hbmp, &m_pRenderBitmap);
		}

		return TRUE;
	}
	void  ControlDrawCaret::Destroy(bool bRedraw)
	{
		Hide(bRedraw);

		m_hWnd = NULL;
		m_pControl = NULL;
		m_ptLast.x = m_ptLast.y = -1;
		m_sizeCaret.cx = m_sizeCaret.cy = 0;
		m_rcDraw.Empty();
		m_hBitmap = NULL;
		if(m_pRenderBitmap){
			if(!m_pRenderBitmap->IsNull()) m_pRenderBitmap->Destroy();
		}
		if(m_pRenderBitmap){
			delete m_pRenderBitmap;
			m_pRenderBitmap = NULL;
		}
	}
	void  ControlDrawCaret::SetPos(int x, int y, bool bRedraw)
	{
		m_ptLast.x = x;
		m_ptLast.y = y;

		if (!m_bOn)
			return;

		DestroyTimer();
		CreateTimer();
		m_bVisible = true;  // 保持显示

		if (bRedraw && m_pControl)
			m_pControl->NeedUpdate();
	}

	void  ControlDrawCaret::OnTimer()
	{
		m_bVisible = !m_bVisible;
		if (m_pControl)
			m_pControl->NeedUpdate();
	}

	void  ControlDrawCaret::Show(bool bRedraw)
	{
		CreateTimer();

		m_bOn = true;
		m_bVisible = true;

		if (bRedraw && m_pControl)
			m_pControl->NeedUpdate();
	}
	void  ControlDrawCaret::Hide(bool bRedraw)
	{
		DestroyTimer();

		m_bOn = false;
		m_bVisible = false;

		if (bRedraw && m_pControl)
			m_pControl->NeedUpdate();   
	}


	void  ControlDrawCaret::CreateTimer()
	{
		if (!m_pControl)
			return;

		m_pControl->GetManager()->KillTimer(m_pControl);

		m_pControl->GetManager()->SetTimer(m_pControl,m_nTimerId,GetCaretBlinkTime());
	}
	void  ControlDrawCaret::DestroyTimer()
	{
		m_pControl->GetManager()->KillTimer(m_pControl);
	}

	void  ControlDrawCaret::OnControlPaint(HDC hDC)
	{
		if (!m_bVisible)
			return;
	}

	ICaret* CCaret::m_pCaret = NULL;
	POINT CCaret::m_ptLast = {0, 0};

	CCaret::CCaret()
	{ 

	}
	CCaret::~CCaret()
	{
		if(m_pCaret){
			delete m_pCaret;
			m_pCaret = NULL;
		}
	}

	BOOL CCaret::CreateCaret(CControlUI* pControl, HBITMAP hbmp, int nWidth, int nHeight)
	{
		if(!pControl) return FALSE;
		HWND hWnd = pControl->GetManager()->GetPaintWindow();
		if(!::IsWindow(hWnd)) return FALSE;
		if (m_pCaret && ( m_pCaret->GetControlUI() != pControl)){
			if(m_pCaret){
				delete m_pCaret;
				m_pCaret = NULL;
			}
		}

		if (!m_pCaret)
		{

		}

		if (m_pCaret)
		{
			return m_pCaret->Create(pControl, hWnd, hbmp, nWidth, nHeight);
		}

		return TRUE;
	}
	void CCaret::DestroyCaret(CControlUI* pControl, bool bRedraw)
	{
		if (m_pCaret && m_pCaret->GetControlUI() == pControl)
		{   
			m_pCaret->Destroy(bRedraw);
			if(m_pCaret){
				delete m_pCaret;
				m_pCaret = NULL;
			}
		}
	}

	// 2013.6.17 将SetCaretPos的参数修改为相对于控件，而不是窗口
	// 这样才好解决Edit滚动的问题
	void CCaret::SetCaretPos(CControlUI* pControl, int x, int y, bool bRedraw)
	{
		if (m_pCaret && m_pCaret->GetControlUI() == pControl)
		{
			m_pCaret->SetPos(x, y, bRedraw);

			m_ptLast.x = x;
			m_ptLast.y = y;
		}
	}
	void CCaret::ShowCaret(CControlUI* pControl, bool bRedraw)
	{
		if (m_pCaret && m_pCaret->GetControlUI() == pControl)
		{
			m_pCaret->Show(bRedraw);
		}
	}
	void CCaret::HideCaret(CControlUI* pControl, bool bRedraw)
	{
		if (m_pCaret && m_pCaret->GetControlUI() == pControl)
		{
			m_pCaret->Hide(bRedraw);
		}
	}
	void  CCaret::OnControlPaint(CControlUI* pControl, HDC hDC)
	{
		if (m_pCaret && m_pCaret->GetControlUI() == pControl)
		{
			m_pCaret->OnControlPaint(hDC);
		}
	}

	CCustomEditUI::CCustomEditUI() :  m_uMaxChar(255), m_bReadOnly(false), 
		m_bPasswordMode(false), m_cPasswordChar(_T('*')), m_uButtonState(0), 
		m_dwEditbkColor(0xFFFFFFFF), m_iWindowStyls(0)
	{
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		//SetBkColor(0xFF000000);//修改 使其能透明
		SetBacTextkColor(0xFFD2D2D2);
		SetBackText(_T(""));
	}

	CCustomEditUI::~CCustomEditUI()
	{

	}

	LPCTSTR CCustomEditUI::GetClass() const
	{
		return _T("CustomEditUI");
	}

	LPVOID CCustomEditUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_CUSTOMEDIT) == 0 ) return static_cast<CCustomEditUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CCustomEditUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void CCustomEditUI::DoEvent(TEventUI& event)
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
			m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			 return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
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

	void CCustomEditUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	void CCustomEditUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		Invalidate();
	}

	void CCustomEditUI::SetMaxChar(UINT uMax)
	{
		m_uMaxChar = uMax;
	}

	UINT CCustomEditUI::GetMaxChar()
	{
		return m_uMaxChar;
	}

	void CCustomEditUI::SetReadOnly(bool bReadOnly)
	{
		if( m_bReadOnly == bReadOnly ) return;

		m_bReadOnly = bReadOnly;
		Invalidate();
	}

	bool CCustomEditUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CCustomEditUI::SetNumberOnly(bool bNumberOnly)
	{
		if( bNumberOnly )
		{
			m_iWindowStyls |= ES_NUMBER;
		}
		else
		{
			m_iWindowStyls |= ~ES_NUMBER;
		}
	}

	bool CCustomEditUI::IsNumberOnly() const
	{
		return m_iWindowStyls&ES_NUMBER ? true:false;
	}

	int CCustomEditUI::GetWindowStyls() const 
	{
		return m_iWindowStyls;
	}

	void CCustomEditUI::SetPasswordMode(bool bPasswordMode)
	{
		if( m_bPasswordMode == bPasswordMode ) return;
		m_bPasswordMode = bPasswordMode;
		Invalidate();
	}

	bool CCustomEditUI::IsPasswordMode() const
	{
		return m_bPasswordMode;
	}

	void CCustomEditUI::SetPasswordChar(TCHAR cPasswordChar)
	{
		if( m_cPasswordChar == cPasswordChar ) return;
		m_cPasswordChar = cPasswordChar;
		Invalidate();
	}

	TCHAR CCustomEditUI::GetPasswordChar() const
	{
		return m_cPasswordChar;
	}

	LPCTSTR CCustomEditUI::GetNormalImage()
	{
		return m_sNormalImage;
	}

	void CCustomEditUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CCustomEditUI::GetHotImage()
	{
		return m_sHotImage;
	}

	void CCustomEditUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CCustomEditUI::GetFocusedImage()
	{
		return m_sFocusedImage;
	}

	void CCustomEditUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CCustomEditUI::GetDisabledImage()
	{
		return m_sDisabledImage;
	}

	void CCustomEditUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	void CCustomEditUI::SetNativeEditBkColor(DWORD dwBkColor)
	{
		m_dwEditbkColor = dwBkColor;
	}

	DWORD CCustomEditUI::GetNativeEditBkColor() const
	{
		return m_dwEditbkColor;
	}

	void CCustomEditUI::SetSel(long nStartChar, long nEndChar)
	{

	}

	void CCustomEditUI::SetSelAll()
	{
		SetSel(0,-1);
	}

	void CCustomEditUI::SetReplaceSel(LPCTSTR lpszReplace)
	{

	}

	RECT CCustomEditUI::CalEditPos()
	{
		CDuiRect rcPos = m_rcItem;
		RECT rcInset = GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right;
		rcPos.bottom -= rcInset.bottom;
		return rcPos;
	}



	void CCustomEditUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);     
	}

	void CCustomEditUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
	}

	void CCustomEditUI::SetInternVisible(bool bVisible)
	{
	}

	SIZE CCustomEditUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CCustomEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0);
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
		else if( _tcscmp(pstrName, _T("backtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetBacTextkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("backtext")) == 0 ) SetBackText(pstrValue);
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CCustomEditUI::PaintStatusImage(HDC hDC)
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
				else return ;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else return;
		}
	}

	void CCustomEditUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) 
		{
			if (m_szBackText.IsEmpty())
			{
				return ;
			}
			CDuiString szTemp = m_szBackText;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;
			if( IsEnabled() ) {
				CRenderEngine::DrawText(hDC, m_pManager, rc, szTemp, m_dwBackTextColor, \
					m_iFont,  m_uTextStyle);
			}
			else {
				CRenderEngine::DrawText(hDC, m_pManager, rc, szTemp, m_dwBackTextColor, \
					m_iFont,  m_uTextStyle);

			}
			return;
		}

		CDuiString sText = m_sText;
		if( m_bPasswordMode ) {
			sText.Empty();
			LPCTSTR p = m_sText.GetData();
			while( *p != _T('\0') ) {
				sText += m_cPasswordChar;
				p = ::CharNext(p);
			}
		}

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top + 2;
		rc.bottom -= m_rcTextPadding.bottom + 2;
		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, \
				m_iFont,  m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
				m_iFont,  m_uTextStyle);

		}
	}

	void CCustomEditUI::SetBackText(LPCTSTR pstrValue)
	{
		m_szBackText = pstrValue;
	}

	void CCustomEditUI::SetBacTextkColor(DWORD dwColor)
	{
		m_dwBackTextColor = dwColor;
	}

}

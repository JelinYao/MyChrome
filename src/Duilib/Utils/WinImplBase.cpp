#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

#include "stdafx.h"

namespace DuiLib
{
	//////////////////////////////////////////////////////////////////////////

	LPBYTE WindowImplBase::m_lpResourceZIPBuffer=NULL;

	DUI_BEGIN_MESSAGE_MAP(WindowImplBase,CNotifyPump)
		DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
		DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
		DUI_END_MESSAGE_MAP()

	WindowImplBase::WindowImplBase()
		:m_pBtn_Close(NULL)
		,m_pBtn_Max(NULL)
		,m_pBtn_Min(NULL)
		,m_pBtn_Restore(NULL)
		,m_pShadowWindow(NULL)
	{	
		
	}
	void WindowImplBase::OnFinalMessage( HWND hWnd )
	{
		if(m_pShadowWindow)
		{
			delete m_pShadowWindow;
			m_pShadowWindow = NULL;
		}
		m_PaintManager.RemovePreMessageFilter(this);
		m_PaintManager.RemoveNotifier(this);
		m_PaintManager.ReapObjects(m_PaintManager.GetRoot());
	}

	UINT WindowImplBase::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}

	UILIB_RESOURCETYPE WindowImplBase::GetResourceType() const
	{
		return UILIB_FILE;
	}

	CDuiString WindowImplBase::GetZIPFileName() const
	{
		return _T("");
	}

	LPCTSTR WindowImplBase::GetResourceID() const
	{
		return _T("");
	}

	CControlUI* WindowImplBase::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	LRESULT WindowImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{
		//if (uMsg == WM_KEYDOWN)
		if (uMsg == WM_SYSKEYDOWN)
		{
			switch (wParam)
			{
			case VK_F4:
				if (m_pBtn_Close)
				{
					m_PaintManager.SendNotify(m_pBtn_Close,_T("click"));
				}
				else
				{
					bHandled = TRUE;
				}
				return TRUE;
			default:
				break;
			}
		}
		return FALSE;
	}

	LRESULT WindowImplBase::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

#if defined(WIN32) && !defined(UNDER_CE)
	LRESULT WindowImplBase::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT WindowImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPRECT pRect=NULL;

		if ( wParam == TRUE)
		{
			LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
			pRect=&pParam->rgrc[0];
		}
		else
		{
			pRect=(LPRECT)lParam;
		}

		if ( ::IsZoomed(m_hWnd))
		{	// 最大化时，计算当前显示器最适合宽高度
			MONITORINFO oMonitor = {};
			oMonitor.cbSize = sizeof(oMonitor);
			::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
			CDuiRect rcWork = oMonitor.rcWork;
			CDuiRect rcMonitor = oMonitor.rcMonitor;
			rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

			pRect->right = pRect->left + rcWork.GetWidth();
			pRect->bottom = pRect->top + rcWork.GetHeight();
			return WVR_REDRAW;
		}

		return 0;
	}

	LRESULT WindowImplBase::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT WindowImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) )
		{
			RECT rcSizeBox = m_PaintManager.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}

			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_PaintManager.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
			if ( NULL == pControl )
				return HTCLIENT;
			if ( HitTestControl(pControl) )
				return HTCLIENT;
			return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT WindowImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		CDuiRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		// 计算最大化时，正确的原点坐标
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;

		lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
		lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();

		lpMMI->ptMinTrackSize.x =m_PaintManager.GetMinInfo().cx;
		lpMMI->ptMinTrackSize.y =m_PaintManager.GetMinInfo().cy;

		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
#endif

	LRESULT WindowImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_PaintManager.GetRoundCorner();

		if (!::IsIconic(m_hWnd))
		{
			if ( szRoundCorner.cx != 0 || szRoundCorner.cy != 0 )
			{
				CDuiRect rcWnd;
				::GetWindowRect(*this, &rcWnd);
				rcWnd.Offset(-rcWnd.left, -rcWnd.top);
				rcWnd.right++; rcWnd.bottom++;
				HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
				::SetWindowRgn(*this, hRgn, TRUE);
				::DeleteObject(hRgn);

			}
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == SC_CLOSE)
		{
			bHandled = TRUE;
			OnCloseWnd();
			return 1;
		}
#if defined(WIN32) && !defined(UNDER_CE)
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ){
			if(m_pBtn_Max) m_pBtn_Max->SetVisible(TRUE == bZoomed);
			if(m_pBtn_Restore) m_pBtn_Restore->SetVisible(FALSE == bZoomed);
		}
#else
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
		return lRes;
	}

	LRESULT WindowImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(m_hWnd, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		::SetWindowPos(m_hWnd, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

		m_PaintManager.Init(m_hWnd);
		m_PaintManager.AddPreMessageFilter(this);

		CDialogBuilder builder;
		CDuiString strResourcePath=m_PaintManager.GetInstancePath();
		strResourcePath+=GetSkinFolder().GetData();
		m_PaintManager.SetResourcePath(strResourcePath.GetData());

		switch(GetResourceType())
		{
		case UILIB_ZIP:
			m_PaintManager.SetResourceZip(GetZIPFileName().GetData(), true);
			break;
		case UILIB_ZIPRESOURCE:
			{
				HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), GetResourceID(), _T("DAT"));
				if( hResource == NULL )
					return 0L;
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
				if( hGlobal == NULL ) 
				{
					return 0L;
				}
				dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
				if( dwSize == 0 )
					return 0L;
				m_lpResourceZIPBuffer = (LPBYTE)::LockResource(hGlobal);
				m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
			}
			break;
		}

		CControlUI* pRoot=NULL;
		if (GetResourceType()==UILIB_RESOURCE)
		{
			STRINGorID xml(_ttoi(GetSkinFile().GetData()));
			pRoot = builder.Create(xml, _T("xml"), this, &m_PaintManager);
		}
		else
			pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_PaintManager);
// 		OutputDebugString(L"打印资源文件…………");
// 		OutputDebugString(GetSkinFile());
		ASSERT(pRoot);
		if (pRoot==NULL)
		{
			MessageBox(NULL,_T("加载资源文件失败"),_T("Duilib"),MB_OK|MB_ICONERROR);
			ExitProcess(1);
			return 0;
		}
		m_PaintManager.AttachDialog(pRoot);
		m_PaintManager.AddNotifier(this);

		m_pBtn_Close	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_sys_close")));
		m_pBtn_Max		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_sys_max")));
		m_pBtn_Restore	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_sys_restore")));
		m_pBtn_Min		= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_sys_min")));
		if(m_pBtn_Restore) 
			m_pBtn_Restore->SetVisible(false);
		InitWindow();
		return 0;
	}

	LRESULT WindowImplBase::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg)
		{
		case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEHOVER:		lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
		//case WM_RESKIN:			lRes = OnReSkin(uMsg,wParam,lParam,bHandled); break;
		case WM_COMMAND:		lRes = OnCommand(uMsg, wParam, lParam, bHandled); break;
		default:				bHandled = FALSE; break;
		}
		if (bHandled) return lRes;

		lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;
		
		if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
		{
			if ( uMsg == WM_PAINT )
			{//延时做这个事情 
				if(CWndShadow::s_hInstance != INVALID_HANDLE_VALUE)
				{
					if(m_PaintManager.IsUseShadow() && NULL == m_pShadowWindow )
					{
						m_pShadowWindow = new CWndShadow;
						m_pShadowWindow->SetBkImage(m_PaintManager.GetShadowImage());
						m_pShadowWindow->SetBorder(m_PaintManager.GetShadowBorder());
						m_pShadowWindow->Create(m_hWnd, &m_PaintManager);
					}
				}
				if( m_pShadowWindow && ( !IsIconic(m_hWnd) || ::IsWindowVisible(m_hWnd) ) )
					m_pShadowWindow->SetPrepared();
			}		
			return lRes;
		}
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT WindowImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LONG WindowImplBase::GetStyle()
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;

		return styleValue;
	}

	void WindowImplBase::OnCloseWnd()
	{
		Close();
	}

	void WindowImplBase::OnPrepare(TNotifyUI& msg)
	{
	}

	void WindowImplBase::OnClick(TNotifyUI& msg)
	{
		if(msg.pSender == m_pBtn_Close)
		{
			OnCloseWnd();
			return; 
		}
		if(msg.pSender == m_pBtn_Min)
		{ 
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
			return; 
		}
		if(msg.pSender == m_pBtn_Max)
		{ 
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
			if(m_pBtn_Max) m_pBtn_Max->SetVisible(false);
			if(m_pBtn_Restore) m_pBtn_Restore->SetVisible(true);
			return; 
		}
		if(msg.pSender == m_pBtn_Restore)
		{ 
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
			if(m_pBtn_Max) m_pBtn_Max->SetVisible(true);
			if(m_pBtn_Restore) m_pBtn_Restore->SetVisible(false);
			return; 
		}
	}

	void WindowImplBase::Notify(TNotifyUI& msg)
	{
		if( _tcscmp (DUI_MSGTYPE_WINDOWINIT,msg.sType) == 0) OnPrepare(msg);
		return CNotifyPump::NotifyPump(msg);
	}

	LRESULT WindowImplBase::OnCommand( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		WORD highParam = HIWORD(wParam);
		WORD lowParam = LOWORD(wParam);
		LRESULT lr = 0;
		if ( highParam == 0 )
			lr = OnMenuClick(lowParam, bHandled);
		bHandled = FALSE;
		return lr;
	}

	LRESULT WindowImplBase::OnMenuClick( WORD wId, BOOL& bHandled )
	{
		return TRUE;
	}

	bool WindowImplBase::HitTestControl(CControlUI* pCtrl)
	{
		if ( _tcsicmp(pCtrl->GetClass(), _T("ButtonUI")) == 0 
			|| _tcsicmp(pCtrl->GetClass(), _T("OptionUI")) == 0 
			|| _tcsicmp(pCtrl->GetClass(), _T("EditUI")) == 0 )
			return true;
		return false;
	}

	DUI_BEGIN_MESSAGE_MAP(CMenuImplBase,CNotifyPump)
		DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
		DUI_END_MESSAGE_MAP()

		CMenuImplBase::CMenuImplBase(void)
	{
		m_bIsExit = false;
		m_pParentMenu = NULL;
		m_pChildMenu = NULL;
	}

	CMenuImplBase::~CMenuImplBase(void)
	{
	}

	LPCTSTR CMenuImplBase::GetWindowClassName() const
	{
		return _T("菜单");
	}	

	void CMenuImplBase::OnFinalMessage(HWND hWnd)
	{
		WindowImplBase::OnFinalMessage(hWnd);
	}

	void CMenuImplBase::InitWindow(void)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		::SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE) 
			&~WS_EX_APPWINDOW|WS_EX_TOOLWINDOW ); //去掉任务栏窗口
	}

	void CMenuImplBase::Notify(TNotifyUI& msg)
	{
		if(msg.sType == DUI_MSGTYPE_WINDOWINIT){
			m_bIsExit = true;
		}
		return WindowImplBase::Notify(msg);
	}

	LRESULT CMenuImplBase::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		switch(uMsg)
		{
		case WM_NCLBUTTONDBLCLK:
			bHandled = TRUE;
			break;
		case WM_KEYDOWN:
			break;
		default:
			break;
		}
		return S_OK;
	}

	LRESULT CMenuImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = WindowImplBase::OnNcHitTest(uMsg, wParam, lParam, bHandled);
		return lRes;
	}

	LRESULT CMenuImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return WindowImplBase::OnSize(uMsg,wParam,lParam,bHandled);
	}

	void CMenuImplBase::OnCloseWnd()
	{
		Close();
	}

	void CMenuImplBase::OnClick(TNotifyUI& msg)
	{
		WindowImplBase::OnClick(msg);
	}

	LRESULT CMenuImplBase::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		if( !IsCloseChildMenu() ) 
			return 0;
		if( m_pChildMenu && IsWindow(*m_pChildMenu) ) 
		{
			m_pChildMenu->Close();
			//判断下当前鼠标位置 是否在窗口上的
			POINT pt = {0,0};
			GetCursorPos(&pt);
			RECT rc;
			GetWindowRect(GetHWND(),&rc);
			if(!PtInRect(&rc,pt))
				Close();
			return 0;
		}
		Close();
		return 0;
	}

	void CMenuImplBase::SetChildMenu(CMenuImplBase* pChildMenu)
	{
		m_pChildMenu = pChildMenu;
		m_pChildMenu->m_pParentMenu = this;
	}

	bool CMenuImplBase::IsCloseChildMenu()
	{
		if(m_pChildMenu&&IsWindow(*m_pChildMenu)){
			if(m_pChildMenu->m_bIsExit&&m_bIsExit)
				return true;
			else 
				return false;
		}
		return m_bIsExit;
	}
}
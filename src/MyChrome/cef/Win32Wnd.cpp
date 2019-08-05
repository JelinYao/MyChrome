#include "StdAfx.h"
#include "Win32Wnd.h"
#include "resource.h"
#include <windowsx.h>

#define CHECK_WINDOW(x) assert(::IsWindow(x))
#define UI_TEXT	L"Win32WndClass"

////////////////////////////////////////////////////////////////
//消息处理函数

LRESULT CALLBACK CWin32Wnd::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		{
			CWin32Wnd* pWnd = (CWin32Wnd*)::GetProp(hWnd, UI_TEXT);
			if( pWnd && pWnd->GetWnd()==hWnd )
				pWnd->OnMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		break;
	case WM_CLOSE:
		{
			CWin32Wnd* pWnd = (CWin32Wnd*)::GetProp(hWnd, UI_TEXT);
			if (pWnd && pWnd->GetWnd() == hWnd)
				pWnd->OnClose();
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/////////////////////////////////////////////////
CWin32Wnd::CWin32Wnd( const wstring& strHomePage/*=L""*/ )
: m_hWnd(NULL)
, m_cefHandles(new CCefHandler(this, ""))
{

}

CWin32Wnd::~CWin32Wnd(void)
{

}

ATOM CWin32Wnd::MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_HOOKDEMO);
	wcex.lpszClassName	= UI_TEXT;
	wcex.hIconSm		= NULL;
	return RegisterClassEx(&wcex);
}

BOOL CWin32Wnd::InitInstance(HINSTANCE hInstance, HWND hWndParent)
{
	HWND hWnd = CreateWindow(UI_TEXT, NULL, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWndParent, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;
	m_hWnd = hWnd;
	OnCreate(hInstance);
	return TRUE;
}

void CWin32Wnd::Show( const bool bShow/*=true*/ )
{
	CHECK_WINDOW(m_hWnd);
	UINT uMode = bShow?SW_SHOW:SW_HIDE;
	ShowWindow(m_hWnd, uMode);
	UpdateWindow(m_hWnd);
}

bool CWin32Wnd::Create(HINSTANCE hInst, HWND hWndParent/* = NULL*/)
{
	MyRegisterClass(hInst);
	return (TRUE == InitInstance(hInst, hWndParent));
}

void CWin32Wnd::CenterWindow()
{
	CHECK_WINDOW(m_hWnd);
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWnd=m_hWnd;
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	if (hWndCenter!=NULL)
		hWnd=hWndCenter;
	if(::IsIconic(hWndCenter) || !::IsWindowVisible(hWndCenter)){
		hWndCenter = ::GetWindowOwner(GetDesktopWindow());
		if (hWndCenter!=NULL)
			hWnd=hWndCenter;
	}
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if( hWndCenter == NULL )
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;
	if( xLeft < rcArea.left ) xLeft = rcArea.left;
	else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
	if( yTop < rcArea.top ) yTop = rcArea.top;
	else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
	::MoveWindow(m_hWnd,xLeft,yTop,DlgWidth,DlgHeight,FALSE);
}

void CWin32Wnd::OnCreate(HINSTANCE hInstance)
{
	::SetProp(m_hWnd, UI_TEXT, (HANDLE)this);
	HICON hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
	hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
}

void CWin32Wnd::OnClose()
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefBrowserHost> pHost = m_pWebBrowser->GetHost();
		if (pHost.get())
			pHost->CloseBrowser(false);
	}
	::RemoveProp(m_hWnd, UI_TEXT);
	delete this;
}

void CWin32Wnd::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
	m_pWebBrowser = browser;
}

void CWin32Wnd::OnSetTitle(const std::wstring& title)
{
	if (IsWindow(m_hWnd) && !title.empty())
		::SetWindowText(m_hWnd, title.c_str());
}

void CWin32Wnd::OnMove(int nWidth, int nHeight)
{
	if (m_pWebBrowser.get())
	{
		HWND hWnd = m_pWebBrowser->GetHost()->GetWindowHandle();
		::MoveWindow(hWnd, 0, 0, nWidth, nHeight, TRUE);
	}
}

VOID CWin32Wnd::FullScreen()
{
	CHECK_WINDOW(m_hWnd);
 	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	::SetWindowPos( m_hWnd, NULL, rcWork.left, rcWork.top, rcWork.GetWidth(), rcWork.GetHeight(), SWP_SHOWWINDOW | SWP_NOZORDER );
}

void CWin32Wnd::ShowMaxmized() 
{ 
	CHECK_WINDOW(m_hWnd); 
	::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
}

void CWin32Wnd::LoadUrl(LPCTSTR lpUrl)
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefFrame> pFrame = m_pWebBrowser->GetMainFrame();
		if (pFrame.get())
			pFrame->LoadURL(CefString(lpUrl));
	}
}

bool CWin32Wnd::ShowBrowserHostWnd(CefRefPtr<CefBrowser> browser, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	windowInfo.SetAsChild(m_hWnd, rect);
	client = m_cefHandles;
	return true;
}

bool CWin32Wnd::CreateBrowserWnd(CefRefPtr<CefClient>& client, CefWindowInfo& windowInfo, CefBrowserSettings& settings, const CefString& url)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	windowInfo.SetAsChild(m_hWnd, rect);
	client = m_cefHandles;
	//return CefBrowserHost::CreateBrowser(windowInfo, m_cefHandles, url, settings, NULL);
	return NULL != CefBrowserHost::CreateBrowserSync(windowInfo, m_cefHandles, url, settings, NULL);
}

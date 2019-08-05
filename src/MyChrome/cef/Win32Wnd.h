/*
///////////////////////////////
Win32ÄÚÇ¶IE¿Ø¼þ´°¿Ú
mailto:jelinyao@163.com
*/

#pragma once
#include <assert.h>
#include <string>
using std::wstring;


class CWin32Wnd
	:public Delegate
{
public:
	CWin32Wnd(const wstring& strHomePage=L"");
	~CWin32Wnd(void);
	HWND GetWnd()const						{ return m_hWnd; }
	CefRefPtr<CefClient> GetCefClient()		{ return m_cefHandles; }
	bool Create(HINSTANCE hInst, HWND hWndParent=NULL);
	void Show(const bool bShow=true);
	void ShowMaxmized();
	void LoadUrl(LPCTSTR lpUrl);
	bool ShowBrowserHostWnd(CefRefPtr<CefBrowser> browser, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings);
	bool CreateBrowserWnd(CefRefPtr<CefClient>& client, CefWindowInfo& windowInfo, CefBrowserSettings& settings, const CefString& url);
	void CenterWindow();
	void FullScreen();

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	ATOM MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE hInstance, HWND hWndParent);
	void OnCreate(HINSTANCE hInstance);
	void OnMove(int nWidth, int nHeight);
	void OnClose();

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
	virtual void OnSetTitle(const std::wstring& title);

private:
	HWND m_hWnd;
	CefRefPtr<CefBrowser>	m_pWebBrowser;
	CefRefPtr<CefClient>	m_cefHandles;
};
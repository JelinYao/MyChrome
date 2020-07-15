#include "stdafx.h"
#include "MainWnd.h"


LRESULT CMainWnd::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch( uMsg )
	{
	case WM_SETTITLE_MSG:
		{
			bHandled = TRUE;
			break;
		}
	case WM_OPENURL_MSG:
		{
			bHandled = TRUE;
			wchar_t* pUrl = (wchar_t*)lParam;
			AddNewBrowser(pUrl);
			free(pUrl);
			break;
		}
	case WM_SIZE:
		{
			if ( wParam == SIZE_MINIMIZED )
				break;
			if ( NULL == m_pTabWeb )
				break;
			//m_webManager.OnSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			int nCount = m_pTabWeb->GetCount();
			if ( 0 == nCount )
				break;
			int nIndex = m_pTabWeb->GetCurSel();
			CChromePage* pPage = static_cast<CChromePage*>(m_pTabWeb->GetItemAt(nIndex));
			if ( pPage )
			{
				pPage->OnSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			break;
		}
	case WM_SET_STATE_TEXT_MSG:
		{
			SetStatus((LPCWSTR)lParam);
			break;
		}
	case WM_CLOSE:
		{
			if (!m_bClose)
			{
				CloseAllWeb();
				bHandled = TRUE;
			}
			break;
		}
	case WM_TIMER:
		OnTimer(wParam, lParam);
		break;
	case WM_MOUSEWHEEL: {
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (fwKeys&MK_CONTROL) {
			CChromePage* web = GetCurrentSelPage();
			if (web) {
				int ratio = web->GetWebkit()->GetZoomRatio();
				if (zDelta == 120) {
					ratio += 10;
				}
				else {
					ratio -= 10;
				}
				if (web->GetWebkit()->SetZoomRatio(ratio))
				{
					CDuiString text;
					text.Format(L"Ëõ·Å±ÈÀý£º%d%%", ratio);
					SetStatus(text);
				}
			}
		}
		break;
	}
	}
	return 0;
}

void CMainWnd::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIMER_ID_STATUS_TEXT)
	{
		KillTimer(m_hWnd, TIMER_ID_STATUS_TEXT);
		m_pLblState->SetText(L"");
	}
}

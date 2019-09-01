#include "stdafx.h"
#include "UIDateTime.h"
#include <time.h>
namespace DuiLib
{
	class CCalendarWnd : public CWindowWnd
	{
	public:
		void Init(CDateTimeUI* pOwner);
		LPCTSTR GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

#if(_WIN32_WINNT >= 0x0501)
		virtual UINT GetClassStyle() const;
#endif

	public:
		CPaintManagerUI m_pm;
		CDateTimeUI* m_pOwner;
		CCalendarUI* m_pCalendar;
	};

	void CCalendarWnd::Init(CDateTimeUI* pOwner)
	{
		ASSERT(pOwner);
		m_pOwner	= pOwner;

		CDuiRect rc;     
		CDuiRect rcOwner = m_pOwner->GetPos();
		rc = rcOwner;

		rc.left  += 0;
		rc.top   += rcOwner.GetHeight(); // // CVerticalLayoutUI 默认的Inset 调整
		rc.right  = rc.left + 220;
		rc.bottom = rc.top + 168;	// 计算弹出窗口高度

		::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		if( ( rc.bottom > rcWork.bottom ) && ( rc.right > rcWork.right )) //如果超过边界，则往左上弹出
		{
			rc = rcOwner;

			rc.bottom = rcOwner.top;
			rc.top    = rc.bottom - 168;
			rc.left  -= rcOwner.GetWidth();
			rc.right  = rc.left + 220;
			::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		}
		else if( rc.bottom > rcWork.bottom ) //如果超过边界，则往上弹出
		{
			rc = rcOwner;

			rc.bottom = rcOwner.top;
			rc.top    = rc.bottom - 168;
			rc.left  += 0; 
			rc.right  = rc.left + 220;
			::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		}
		else if( rc.right > rcWork.right ) //如果超过边界，则往左弹出
		{
			rc = rcOwner;

			rc.top   += rcOwner.GetHeight(); // // CVerticalLayoutUI 默认的Inset 调整
			rc.bottom = rc.top + 168;
			rc.left  -= rcOwner.GetWidth();
			rc.right  = rc.left + 220;

			::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		}

		Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
		pOwner->SetDateTimeWnd(m_hWnd);
		// HACK: Don't deselect the parent's caption
		HWND hWndParent = m_hWnd;
		while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
		::ShowWindow(m_hWnd, SW_SHOW);
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
	}

	LPCTSTR CCalendarWnd::GetWindowClassName() const
	{
		return _T("CalendarWnd");
	}

	void CCalendarWnd::OnFinalMessage(HWND hWnd)
	{
		m_pOwner->SetDateTimeWnd(NULL);
		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
		m_pOwner->Invalidate();
		delete this;
	}

	LRESULT CCalendarWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE ) {
			m_pm.Init(m_hWnd);
			// The trick is to add the items to the new container. Their owner gets
			// reassigned by this operation - which is why it is important to reassign
			// the items back to the righfull owner/manager when the window closes.
			m_pCalendar = new CCalendarUI;
			m_pm.UseParentResource(m_pOwner->GetManager());
			m_pCalendar->SetDateTime(m_pOwner->GetDateTime());
			m_pCalendar->SetManager(&m_pm, NULL, true);
			m_pCalendar->SetDateTimeControl(m_pOwner);
			m_pCalendar->ApplyAttributeList(m_pOwner->GetCalendarAttr());
			m_pm.AttachDialog(m_pCalendar);
			return 0;
		}
		else if( uMsg == WM_CLOSE ) {
			m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
			m_pOwner->SetPos(m_pOwner->GetPos());
			m_pOwner->SetFocus();
		}
		else if( uMsg == WM_KEYDOWN ) {
			switch( wParam ) {
		case VK_ESCAPE:
		case VK_RETURN:
			PostMessage(WM_KILLFOCUS);
			break;
		default:
			break;
			}
		}
		else if( uMsg == WM_KILLFOCUS ) {
			if( m_hWnd != (HWND) wParam ) 
			{
				if( m_hWnd != (HWND) wParam )
				{
					if (!m_pCalendar->IsCobWnd((HWND) wParam))
					{
						::PostMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_KILLFOCUS,wParam,lParam);
						PostMessage(WM_CLOSE);
					}
				}
			}
		}

		LRESULT lRes = 0;
	    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	CCalendarUI::CCalendarUI(void)
	{
		//初始化当前日期
		m_pDateTime = NULL;
		m_pCalendar = new TCalendarInfo;
		m_bIsClose = true;
		time_t now;	
		time(&now);
		m_pCurDateTime = localtime(&now);
		m_pCurDateTime->tm_year	= m_pCurDateTime->tm_year-100+2000;
		m_pCurDateTime->tm_mon	= m_pCurDateTime->tm_mon+1;
		m_Today					= m_pCurDateTime->tm_mday;

		CalDateTime(m_pCurDateTime->tm_year,m_pCurDateTime->tm_mon,m_Today);

		//初始化缺省样式
		m_DefaultStyle.nMainTitleBkColor	= _T("#FF8de316");
		m_DefaultStyle.nSubTitleBkColor		= _T("#FFFFFFFF");
		m_DefaultStyle.nWeekIntervalColorA	= _T("#FFFFFFFF");
		m_DefaultStyle.nWeekIntervalColorB	= _T("#FFFFFFFF");
		m_DefaultStyle.nWeekendColorA		= _T("#FFFFFFFF");
		m_DefaultStyle.nWeekendColorB		= _T("#FFFFFFFF"); 
		m_DefaultStyle.nDayHotColor			= _T("#FF3721F1");
		m_DefaultStyle.nDaySelectColor		= _T("#FF3721F1");
		m_DefaultStyle.nNoCurMonthDayColor	= _T("#FFD0D0D0");
		m_DefaultStyle.nStatusBkColor		= _T("#FFf3ed8e");
		m_DefaultStyle.nWeekEndTextColor    = _T("#FFFF0000");
		m_DefaultStyle.nWeekTextColor		= _T("#FFFF0000");
		m_DefaultStyle.nTextColor			= _T("#FF000000");


		m_SubTitleString.nSundayStr			= _T("日");
		m_SubTitleString.nMondayStr			= _T("一");
		m_SubTitleString.nTuesdayStr		= _T("二");
		m_SubTitleString.nWednesdayStr		= _T("三");
		m_SubTitleString.nThursdayStr		= _T("四");
		m_SubTitleString.nFridayStr			= _T("五");
		m_SubTitleString.nSaturdayStr		= _T("六");
		m_SubTitleString.nToDayString		= _T("返回今天");

		m_rcInset.left = 1;
		m_rcInset.top = 1;
		m_rcInset.right = 1;
		m_rcInset.bottom = 1;
		AddSubTitleString(_T("Default"),m_SubTitleString);

		//初始化日历主标题容器与元素

		m_pMainTitleHoriz = new CHorizontalLayoutUI;
		m_pMainTitleLeft= new CHorizontalLayoutUI;
		m_pMainTitleRight = new CHorizontalLayoutUI;
		m_pcob_year = new CComboUI;
		m_pcob_month = new CComboUI;
		m_pbtn_today = new CButtonUI;
		m_plbl_year = new CLabelUI;
		m_plbl_month = new CLabelUI;

		m_pMainTitleHoriz->ApplyAttributeList(_T("height=\"24\" bkcolor=\"#FFF3ED8F\""));
		m_pMainTitleLeft->ApplyAttributeList(_T("height=\"24\""));
		m_pMainTitleRight->ApplyAttributeList(_T("width=\"54\" height=\"24\""));
		m_plbl_year->ApplyAttributeList(_T("float=\"true\" pos=\"3,2,0,0\" width=\"25\" height=\"20\" text=\"年：\""));
		m_pcob_year->ApplyAttributeList(_T("float=\"true\" pos=\"28,2,0,0\" width=\"50\" height=\"20\" bkcolor=\"#FFA6CAF0\" itemtextpadding=\"6,0,0,0\" itemtextcolor=\"#FF000000\" itemselectedtextcolor=\"#FF000000\" itemselectedbkcolor=\"#FFC1E3FF\" itemhottextcolor=\"#FF000000\" itemhotbkcolor=\"#FFE9F5FF\" itemdisabledtextcolor=\"#FFCCCCCC\" itemdisabledbkcolor=\"#FFFFFFFF\" dropboxsize=\"0,130\" textpadding=\"2,0,0,0\""));
		m_plbl_month->ApplyAttributeList(_T("float=\"true\" pos=\"78,2,0,0\" width=\"25\" height=\"20\" text=\"月：\""));
		m_pcob_month->ApplyAttributeList(_T("float=\"true\" pos=\"103,2,0,0\" width=\"40\" height=\"20\" bkcolor=\"#FFA6CAF0\" itemtextpadding=\"6,0,0,0\" itemtextcolor=\"#FF000000\" itemselectedtextcolor=\"#FF000000\" itemselectedbkcolor=\"#FFC1E3FF\" itemhottextcolor=\"#FF000000\" itemhotbkcolor=\"#FFE9F5FF\" itemdisabledtextcolor=\"#FFCCCCCC\" itemdisabledbkcolor=\"#FFFFFFFF\" dropboxsize=\"0,130\" textpadding=\"2,0,0,0\""));
		m_pbtn_today->ApplyAttributeList(_T("float=\"true\" pos=\"2,3,0,0\" width=\"50\" height=\"18\" bkcolor=\"#FFA6CAF0\" textcolor=\"#FF000000\" disabledtextcolor=\"#FFA7A6AA\" transstroke=\"255\""));
		m_pbtn_today->SetText(m_SubTitleString.nToDayString);
		m_pMainTitleLeft->Add(m_plbl_year);
		m_pMainTitleLeft->Add(m_pcob_year);
		m_pMainTitleLeft->Add(m_plbl_month);
		m_pMainTitleLeft->Add(m_pcob_month);

		m_pMainTitleRight->Add(m_pbtn_today);

		m_pMainTitleHoriz->Add(m_pMainTitleLeft);
		m_pMainTitleHoriz->Add(m_pMainTitleRight);

		m_pcob_year->OnNotify += MakeDelegate(this,&CCalendarUI::OnSelectYear);
		m_pcob_month->OnNotify += MakeDelegate(this,&CCalendarUI::OnSelectMonth);
		m_pbtn_today->OnNotify += MakeDelegate(this,&CCalendarUI::OnToday);
		
		//加入星期几容器
		m_pSubTitleHoriz = new CHorizontalLayoutUI;
		m_pSubTitleHoriz->ApplyAttributeList(_T("height=\"22\""));
		for(int nWeekIndex = 0;nWeekIndex < 7;nWeekIndex++)
		{
			CControlUI* pWeek = CreateWeekPanel(m_pSubTitleHoriz,nWeekIndex);
			InitWeekPanel(pWeek,nWeekIndex);

			if(pWeek) 
				m_pSubTitleHoriz->Add(pWeek);
		}


		
		//加载日期容器
		m_pWeekPanelHoriz = new CHorizontalLayoutUI;
		m_pWeekPanelVert = new CVerticalLayoutUI;

		CDuiString TmpDef;
		TmpDef.SmallFormat(_T("CalendGroupName_%d"),this);

		for(int nWeekLine = 1;nWeekLine < 7;nWeekLine++)
		{
			CHorizontalLayoutUI* pWeekLine = new CHorizontalLayoutUI();
			for(int nDayIndex = 1;nDayIndex < 8;nDayIndex++)
			{
				CHorizontalLayoutUI* pDayParent = new CHorizontalLayoutUI();

				CControlUI* pDay = CreateDayPanel(nDayIndex,TmpDef.GetData());

				pDayParent->Add(pDay);

				TCalendarInfo* pCalendarInfo	= new TCalendarInfo();
				pDayParent->SetTag((UINT_PTR)pCalendarInfo);
				m_TCalendarInfoArray.Add(pCalendarInfo);

				pWeekLine->Add(pDayParent);
			}
			m_pWeekPanelVert->Add(pWeekLine);
		}
		m_pWeekPanelHoriz->Add(m_pWeekPanelVert);


		InitYearData();
		InitMonthData();

		Add(m_pMainTitleHoriz);
		Add(m_pSubTitleHoriz);
		Add(m_pWeekPanelHoriz);

		//初始化默认样式
		SetCalendarStyle(m_DefaultStyle);

		//初始化日历数据
		InitCalendarDis(m_pCurDateTime->tm_year,m_pCurDateTime->tm_mon);

	}


	void CCalendarUI::SetDateTimeControl(void* pControl)
	{
		if (m_pDateTime == pControl) return ;
		m_pDateTime = pControl;
	}
	CCalendarUI::~CCalendarUI(void)
	{
		if (m_pCalendar)
		{
			delete m_pCalendar;
		}
		RemoveAllSubTitleString();
	}

	LPCTSTR CCalendarUI::GetClass() const
	{ 
		return _T("CalendarUI"); 
	}

	LPVOID CCalendarUI::GetInterface( LPCTSTR pstrName )
	{ 
		if( _tcscmp(pstrName, _T("Calendar")) == 0 ) return static_cast<CCalendarUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CCalendarUI::DoEvent( TEventUI& event )
	{ 
		CVerticalLayoutUI::DoEvent(event);
	}

	CControlUI* CCalendarUI::CreateWeekPanel( CControlUI* _Panent,int _Week )
	{
		CButtonUI* pWeek	= new CButtonUI();
		pWeek->SetManager(GetManager(),this,false);
		_Panent->SetFixedHeight(22);
		pWeek->SetMouseEnabled(false);
		return pWeek;
	}

	CControlUI* CCalendarUI::CreateDayPanel( int _Week,LPCTSTR _GroupName )
	{ 
		COptionUI*	pDay = new COptionUI();
		pDay->SetManager(GetManager(),this,false);
		int heighta = pDay->GetHeight();
		int hegithb = pDay->GetFixedHeight();
		pDay->SetGroup(_GroupName);
		pDay->SetName(_GroupName);
		pDay->SetAttribute(_T("algin"),_T("right"));
		pDay->OnNotify+= MakeDelegate(this,&CCalendarUI::OnSelcetDay);

		return pDay;
	}


	void CCalendarUI::SetCalendarStyle( TCalendarStyle& _TCalendarStyle )
	{
		this->SetAttribute(_T("bordercolor"),_TCalendarStyle.nCalenderBorderColor);
		this->SetAttribute(_T("bkcolor"),_TCalendarStyle.nCalendarBkColor);
		m_pMainTitleHoriz->SetAttribute(_T("bkcolor"),_TCalendarStyle.nMainTitleBkColor);
		m_pcob_year->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
		m_pcob_month->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
	//	m_pcob_festival->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
		m_pbtn_today->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);

		m_pSubTitleHoriz->SetAttribute(_T("bkcolor"),_TCalendarStyle.nSubTitleBkColor);

		for(int nWeekIndex = 0;nWeekIndex < 6;nWeekIndex++)
		{
			CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(m_pWeekPanelVert->GetItemAt(nWeekIndex));
			if(!pWeekLine)
				continue;

			pWeekLine->SetAttribute(_T("bkcolor"),!(nWeekIndex%2)?_TCalendarStyle.nWeekIntervalColorA:_TCalendarStyle.nWeekIntervalColorB);

			for(int nDayIndex = 0;nDayIndex < 7;nDayIndex++)
			{
				CVerticalLayoutUI* pDayParent = static_cast<CVerticalLayoutUI*>(pWeekLine->GetItemAt(nDayIndex));
				if(!pDayParent)
					continue;

				if(nWeekIndex%2 && (nDayIndex == 0 || nDayIndex == 6))
					pDayParent->SetAttribute(_T("bkcolor"),_TCalendarStyle.nWeekendColorA);
				else if(!(nWeekIndex%2) && (nDayIndex == 0 || nDayIndex == 6))
					pDayParent->SetAttribute(_T("bkcolor"),_TCalendarStyle.nWeekendColorB);

				COptionUI* pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));

				if(!pDay || CDuiString(pDay->GetClass()) != CDuiString(_T("OptionUI")))
					continue;

				pDay->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
				pDay->SetAttribute(_T("selectedbkcolor"),_TCalendarStyle.nDaySelectColor);
			}
		}
	}

	bool CCalendarUI::AddSubTitleString( LPCTSTR _Name,TSubTitleString& _SubTitleString )
	{
		if(!_Name && m_SubTitleStringArray.Find(_Name) != NULL )
			return false;

		TSubTitleString* pSubTitleString = new TSubTitleString();
		pSubTitleString->nSundayStr		= _SubTitleString.nSundayStr;
		pSubTitleString->nMondayStr		= _SubTitleString.nMondayStr;
		pSubTitleString->nWednesdayStr	= _SubTitleString.nWednesdayStr;
		pSubTitleString->nThursdayStr	= _SubTitleString.nThursdayStr;
		pSubTitleString->nFridayStr		= _SubTitleString.nFridayStr;
		pSubTitleString->nSaturdayStr	= _SubTitleString.nSaturdayStr;

		m_SubTitleStringArray.Set(_Name,pSubTitleString);

		return true;
	}

	bool CCalendarUI::AddSubTitleString( LPCTSTR _Name,LPCTSTR _Sunday,LPCTSTR _Monday,LPCTSTR _Tuesday,LPCTSTR _Wednesday,LPCTSTR _Thursday,LPCTSTR _Friday,LPCTSTR _Saturday )
	{
		if(!_Name || !_Sunday || !_Monday || !_Tuesday || !_Wednesday || !_Thursday || !_Friday || !_Saturday)
			return false;

		TSubTitleString* pSubTitleString = new TSubTitleString();
		pSubTitleString->nSundayStr		= _Sunday;
		pSubTitleString->nMondayStr		= _Monday;
		pSubTitleString->nWednesdayStr	= _Wednesday;
		pSubTitleString->nThursdayStr	= _Thursday;
		pSubTitleString->nFridayStr		= _Friday;
		pSubTitleString->nSaturdayStr	= _Saturday;

		if(pSubTitleString != NULL)
		{
			if( m_SubTitleStringArray.Find(_Name) == NULL )
				m_SubTitleStringArray.Set(_Name,pSubTitleString);
			else
				delete pSubTitleString;
			pSubTitleString = NULL;
			return true;
		}
		return false;
	}

	TSubTitleString& CCalendarUI::GetSubTitleString( LPCTSTR _Name /*= NULL*/ )
	{
		if(!_Name)
			return m_SubTitleString;

		TSubTitleString* pSubTitleString = static_cast<TSubTitleString*>(m_SubTitleStringArray.Find(_Name));

		return pSubTitleString?*pSubTitleString:m_SubTitleString;
	}

	bool CCalendarUI::RemoveAtSubTitleString( LPCTSTR _Name )
	{
		if(!_Name)
			return false;

		TSubTitleString* pSubTitleString = static_cast<TSubTitleString*>(m_SubTitleStringArray.Find(_Name));

		if( !pSubTitleString )
			return false;

		delete pSubTitleString;
		return m_SubTitleStringArray.Remove(_Name);
	}

	void CCalendarUI::RemoveAllSubTitleString()
	{
		TSubTitleString* pSubTitleString = NULL;
		for( int i = 0; i< m_SubTitleStringArray.GetSize(); i++ ) {
			if(LPCTSTR key = m_SubTitleStringArray.GetAt(i)) {
				pSubTitleString = static_cast<TSubTitleString*>(m_SubTitleStringArray.Find(key));
				delete pSubTitleString;
			}
		}
	}

	bool CCalendarUI::IsLeapYear( int _Year )
	{
		return ((_Year % 4 == 0 && _Year % 100 != 0) || _Year % 400 == 0);
	}

	int CCalendarUI::GetNumLeapYear( int _Year )
	{
		int count=0;
		for(int i=1;i<_Year;i++)
			if(IsLeapYear(i))
				count++;
		return count;
	}

	int CCalendarUI::DaysOfMonth( int _Mooth,int _Year /*= -1*/ )
	{
		int nYear = _Year == -1?m_pCurDateTime->tm_year:_Year;

		switch (_Mooth)
		{
		case 0: return 0;
		case 1 :
		case 3 :
		case 5 :
		case 7 :
		case 8 :
		case 10 :
		case 12 : return 31 ;
		case 4 :
		case 6 :
		case 9 :
		case 11 : return 30 ;
		case 2 : return IsLeapYear(nYear)?29:28;
		}  
		return 0 ;
	}

	void CCalendarUI::InitCalendarDis( int _Year,int _Month )
	{
		if(_Month > 12){
			_Month = 1;
			_Year++;
		}

		if(_Month < 1) {
			_Month = 12;
			_Year--;
		}

		int nLastMaxDay		= _Month == 1 ?DaysOfMonth(12,_Year-1):DaysOfMonth(_Month-1,_Year);
		int nLastMinDay		= 0;
		int nAsMooth		= 0;
		int nDays			= DaysOfMonth(_Month,_Year);
		int nWeek			= 0;
		int nFDay			= 0;
		int nCalDay			= 0;
		for(int i=1;i<=13;i++)
		{
			nFDay += DaysOfMonth(i-1);

			int nCurFday = (nFDay+(_Year-1)*365+GetNumLeapYear(_Year))%7+1;
			if(i == _Month)
				nWeek = nCurFday;
		}

		nAsMooth	= nWeek == 0?0:_Month-1;
		nLastMinDay = nWeek == 0?0:nLastMaxDay-nWeek;
		for(int nWeekIndex = 0;nWeekIndex < 6;nWeekIndex++)
		{
			CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(m_pWeekPanelVert->GetItemAt(nWeekIndex));

			for(int nDayIndex = 0;nDayIndex < 7;nDayIndex++)
			{
				CVerticalLayoutUI* pDayParent = static_cast<CVerticalLayoutUI*>(pWeekLine->GetItemAt(nDayIndex));
				if(!pDayParent)
					continue;

				TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pDayParent->GetTag();
				if(!pTCalendarInfo)
					continue;

				COptionUI *pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));

				if(!(nWeek && nDayIndex < nWeek && nCalDay <= nDays))
				{
					nCalDay++;
					nAsMooth = nCalDay <= nDays?_Month:_Month+1;
				}

				if((nLastMinDay <= nLastMaxDay && nAsMooth != _Month) || nCalDay >= nDays){
					nLastMinDay++;
					nAsMooth	= _Month+1;
				}

				if(nCalDay == nDays) {
					nLastMinDay = 0;
					nAsMooth	= _Month;
				}

				pTCalendarInfo->nYear		= _Year;
				pTCalendarInfo->nMooth		= _Month;
				pTCalendarInfo->nDay		= ((nWeek && nDayIndex < nWeek) || nCalDay > nDays)?nLastMinDay:nCalDay;
				pTCalendarInfo->nWeekLine	= nWeekIndex;
				pTCalendarInfo->nWeek		= nWeek;
				pTCalendarInfo->nAsMooth	= _Month;

				if(nCalDay < 1)
					pTCalendarInfo->nAsMooth= _Month -1;
				else if(nCalDay > nDays)
					pTCalendarInfo->nAsMooth= _Month +1;

				InitDayPanel(pDay,(nDayIndex == 0 || nDayIndex == 6),pTCalendarInfo->nYear,_Month,pTCalendarInfo->nDay,pTCalendarInfo->nWeek,pTCalendarInfo->nWeekLine,pTCalendarInfo->nAsMooth);

				pDay->Selected(nCalDay == m_Today);

			}
			nWeek = 0;
		}
	}


	bool CCalendarUI::OnSelcetDay( void* param )
	{
		TNotifyUI* pTNotifyUI = (TNotifyUI*)param;
		if( pTNotifyUI->pSender)
		{	
			TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pTNotifyUI->pSender->GetParent()->GetTag();

			m_Today = pTCalendarInfo->nDay;
			if(m_pCurDateTime->tm_mday == m_Today) 
			{
				if (m_pDateTime)
				{
					((CDateTimeUI*)m_pDateTime)->SetDateTime(m_pCurDateTime);
					if (IsClose())
					{
						((CDateTimeUI*)m_pDateTime)->CloseWnd();
					}
				}
				return true;
			}
			m_pCurDateTime->tm_mday = m_Today;

			CalDateTime(pTCalendarInfo->nYear,pTCalendarInfo->nMooth,pTCalendarInfo->nDay);
			if (m_pDateTime)
			{
				((CDateTimeUI*)m_pDateTime)->SetDateTime(m_pCurDateTime);
				if (IsClose())
				{
					((CDateTimeUI*)m_pDateTime)->CloseWnd();
				}
			}
		}
		return true;
	}

	bool CCalendarUI::OnSelectYear(void* param)
	{
		TNotifyUI* pTNotifyUI = (TNotifyUI*)param;
		if (pTNotifyUI->pSender == m_pcob_year)
		{
			CDuiString szText = m_pcob_year->GetText().GetData();
			szText.Replace(_T("年"),_T(""));
			int nyear = m_pCurDateTime->tm_year;
			LPTSTR pstr = NULL;
			nyear = _tcstol(szText.GetData(), &pstr, 10);
			m_pCurDateTime->tm_year = nyear;
			m_bIsClose = false;
			InitCalendarDis(m_pCurDateTime->tm_year,m_pCurDateTime->tm_mon);
			m_bIsClose = true;
			if (m_pDateTime)
			{
				((CDateTimeUI*)m_pDateTime)->SetDateTime(m_pCurDateTime);
			}
		}
		return true;
	}

	bool CCalendarUI::OnSelectMonth(void* param)
	{
		TNotifyUI* pTNotifyUI = (TNotifyUI*)param;
		if (pTNotifyUI->pSender == m_pcob_month)
		{
			CDuiString szText = m_pcob_month->GetText().GetData();
			szText.Replace(_T("月"),_T(""));
			int nyear = m_pCurDateTime->tm_mon;
			LPTSTR pstr = NULL;
			nyear = _tcstol(szText.GetData(), &pstr, 10);
			m_pCurDateTime->tm_mon = nyear;
			m_bIsClose = false;
			InitCalendarDis(m_pCurDateTime->tm_year,m_pCurDateTime->tm_mon);
			m_bIsClose = true;
			if (m_pDateTime)
			{
				((CDateTimeUI*)m_pDateTime)->SetDateTime(m_pCurDateTime);
			}
		}
		return true;
	}

	void CCalendarUI::InitWeekPanel( CControlUI* _Control,int _Week )
	{
		switch (_Week)
		{
		case 0:
			{
				_Control->SetText(m_SubTitleString.nSundayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekEndTextColor);
			}
			break;
		case 1:
			{
				_Control->SetText(m_SubTitleString.nMondayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekTextColor);
			}
			break;
		case 2:
			{
				_Control->SetText(m_SubTitleString.nTuesdayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekTextColor);
			}
			break;
		case 3:
			{
				_Control->SetText(m_SubTitleString.nWednesdayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekTextColor);
			}
			break;
		case 4:
			{
				_Control->SetText(m_SubTitleString.nThursdayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekTextColor);
			}
			break;
		case 5:
			{
				_Control->SetText(m_SubTitleString.nFridayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekTextColor);
			}
			break;
		case 6:
			{
				_Control->SetText(m_SubTitleString.nSaturdayStr);
				_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekEndTextColor);
			}
			break;
		default:
			break;
		}
	}

	void CCalendarUI::InitDayPanel( CControlUI* _Control,bool bWeekEnd,TCalendarInfo& _CalendarInfo )
	{ 
		InitDayPanel(_Control,bWeekEnd,_CalendarInfo.nYear,_CalendarInfo.nMooth,_CalendarInfo.nDay,_CalendarInfo.nWeek,_CalendarInfo.nWeekLine,_CalendarInfo.nAsMooth);
	}

	void CCalendarUI::InitDayPanel( CControlUI* _Control,bool bWeekEnd,int _Year,int _Mooth,int _Day,int _Week,int _WeekLine,int _AsMooth )
	{
		if(_AsMooth != _Mooth)
		{
			_Control->SetAttribute(_T("bkcolor"),m_DefaultStyle.nNoCurMonthDayColor);
			_Control->SetEnabled(false);
		}
		else 
		{
			_Control->SetAttribute(_T("bkcolor"),_T(""));
			_Control->SetEnabled(true);
		}
		

		CDuiString TmpDef;

		if(bWeekEnd)
		{
			TmpDef.SmallFormat(_T("%d"),_Day); 
			_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nWeekEndTextColor);
		}
		else
		{
			TmpDef.SmallFormat(_T("%d"),_Day); 
			_Control->SetAttribute(_T("textcolor"),m_DefaultStyle.nTextColor);
		}

		_Control->SetText(TmpDef.GetData());
	}


	void CCalendarUI::InitWeekData(int nIndex,LPCTSTR pstrValue)
	{
		CControlUI* pControl = m_pSubTitleHoriz->GetItemAt(nIndex);
		if (pControl)
		{
			pControl->SetText(pstrValue);
		}
		
	}

	void CCalendarUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("style")) == 0 ) AnyCalendarStyle(pstrValue);//
		else if( _tcscmp(pstrName, _T("cobyear")) == 0 ) m_pcob_year->ApplyAttributeList(pstrValue);
		else if( _tcscmp(pstrName, _T("cobmonth")) == 0 ) m_pcob_month->ApplyAttributeList(pstrValue);
		else if( _tcscmp(pstrName, _T("btntoday")) == 0 ) m_pbtn_today->ApplyAttributeList(pstrValue);
		else if( _tcscmp(pstrName, _T("lblyear")) == 0 ) m_plbl_year->ApplyAttributeList(pstrValue);
		else if( _tcscmp(pstrName, _T("lblmonth")) == 0 ) m_plbl_month->ApplyAttributeList(pstrValue);
		else if( _tcscmp(pstrName, _T("week0")) == 0 ) InitWeekData(0,pstrValue);
		else if( _tcscmp(pstrName, _T("week1")) == 0 ) InitWeekData(1,pstrValue);
		else if( _tcscmp(pstrName, _T("week2")) == 0 ) InitWeekData(2,pstrValue);
		else if( _tcscmp(pstrName, _T("week3")) == 0 ) InitWeekData(3,pstrValue);
		else if( _tcscmp(pstrName, _T("week4")) == 0 ) InitWeekData(4,pstrValue);
		else if( _tcscmp(pstrName, _T("week5")) == 0 ) InitWeekData(5,pstrValue);
		else if( _tcscmp(pstrName, _T("week6")) == 0 ) InitWeekData(6,pstrValue);
		else CVerticalLayoutUI::SetAttribute(pstrName,pstrValue);
	}


	bool CCalendarUI::OnToday(void* param)
	{
		TNotifyUI* pTNotifyUI = (TNotifyUI*)param;
		if( pTNotifyUI->pSender == m_pbtn_today)
		{
			//初始化当前日期
			time_t now;	
			time(&now);
			m_pCurDateTime = localtime(&now);
			m_pCurDateTime->tm_year	= m_pCurDateTime->tm_year-100+2000;
			m_pCurDateTime->tm_mon	= m_pCurDateTime->tm_mon+1;
			m_Today					= m_pCurDateTime->tm_mday;
			m_bIsClose = false;
			InitCalendarDis(m_pCurDateTime->tm_year,m_pCurDateTime->tm_mon);
			CDuiString szYear = _T("");
			CDuiString szMonth = _T("");
			szYear.SmallFormat(_T("%.4d"),m_pCurDateTime->tm_year);
			szMonth.SmallFormat(_T("%d"),m_pCurDateTime->tm_mon);
			m_pcob_year->SelectItem(m_pcob_year->GetItemIndex(szYear));
			m_pcob_month->SelectItem(m_pcob_month->GetItemIndex(szMonth));
			m_bIsClose = true;
			return false;
		}
		return true;
	}

	void CCalendarUI::AnyCalendarStyle( LPCTSTR _StyleStr,TCalendarStyle* _TCalendarStyle /*= NULL*/ )
	{
		CDuiString sItem;
		CDuiString sValue;
		LPTSTR pstr = NULL;

		TCalendarStyle* pCalendarInfo = _TCalendarStyle?_TCalendarStyle:&m_DefaultStyle;

		while( *_StyleStr != _T('\0') ) {
			sItem.Empty();
			sValue.Empty();
			while( *_StyleStr > _T('\0') && *_StyleStr <= _T(' ') ) _StyleStr = ::CharNext(_StyleStr);
			while( *_StyleStr != _T('\0') && *_StyleStr != _T('=') && *_StyleStr > _T(' ') ) {
				LPTSTR pstrTemp = ::CharNext(_StyleStr);
				while( _StyleStr < pstrTemp) {
					sItem += *_StyleStr++;
				}
			}
			while( *_StyleStr > _T('\0') && *_StyleStr <= _T(' ') ) _StyleStr = ::CharNext(_StyleStr);
			if( *_StyleStr++ != _T('=') ) break;
			while( *_StyleStr > _T('\0') && *_StyleStr <= _T(' ') ) _StyleStr = ::CharNext(_StyleStr);
			if( *_StyleStr++ != _T('\'') ) break;
			while( *_StyleStr != _T('\0') && *_StyleStr != _T('\'') ) {
				LPTSTR pstrTemp = ::CharNext(_StyleStr);
				while( _StyleStr < pstrTemp) {
					sValue += *_StyleStr++;
				}
			}
			if( *_StyleStr++ != _T('\'') ) break;
			if( !sValue.IsEmpty() ) {
				if( sItem == _T("bbcolor") ) pCalendarInfo->nCalenderBorderColor = sValue;
				else if( sItem == _T("bkcolor") ) pCalendarInfo->nCalendarBkColor = sValue;
				else if( sItem == _T("tbkcolor") ) pCalendarInfo->nMainTitleBkColor = sValue;
				else if( sItem == _T("sbkcolor") ) pCalendarInfo->nSubTitleBkColor = sValue;
				else if( sItem == _T("webkcolora") ) pCalendarInfo->nWeekendColorA = sValue;
				else if( sItem == _T("webkcolorb") ) pCalendarInfo->nWeekendColorB = sValue;
				else if( sItem == _T("dayhotcolor") ) pCalendarInfo->nDayHotColor = sValue;
				else if( sItem == _T("daypushedcolor") ) pCalendarInfo->nDayPushedColor = sValue;
				else if( sItem == _T("dayselcolor") ) pCalendarInfo->nDaySelectColor = sValue;
				else if( sItem == _T("daydiscolor") ) pCalendarInfo->nDayDisabledColor = sValue;
				else if( sItem == _T("nocurmonthdaycolor") ) pCalendarInfo->nNoCurMonthDayColor = sValue;
				else if( sItem == _T("weekicolora") ) pCalendarInfo->nWeekIntervalColorA = sValue;
				else if( sItem == _T("weekicolorb") ) pCalendarInfo->nWeekIntervalColorB = sValue;
				else if( sItem == _T("ibkcolor") ) pCalendarInfo->nStatusBkColor = sValue;
				else if( sItem == _T("iweekendtextcolor") )	pCalendarInfo->nWeekEndTextColor = sValue;
				else if( sItem == _T("iweektextcolor") )	pCalendarInfo->nWeekTextColor = sValue;
				else if( sItem == _T("itextcolor") )	pCalendarInfo->nTextColor = sValue;
			}
			if( *_StyleStr++ != _T(' ') ) break;
		}
		SetCalendarStyle(*pCalendarInfo);
	}

	void CCalendarUI::CalDateTime( TCalendarInfo* _TCalendarInfo )
	{
		if(_TCalendarInfo)
			CalDateTime(_TCalendarInfo->nYear,_TCalendarInfo->nMooth,_TCalendarInfo->nDay);
	}

	void CCalendarUI::CalDateTime( int _Year,int _Mooth,int _Day )
	{
		m_nCurYear = _Year;
		m_nCurMooth = _Mooth;
		m_nCurDay = _Day;
		m_pCalendar->nYear = _Year;
		m_pCalendar->nMooth = _Mooth;
		m_pCalendar->nDay = _Day;
		m_szCurData.SmallFormat(_T("%.04d-%.02d-%.02d"),_Year,_Mooth,_Day);
	}

	TCalendarInfo*	CCalendarUI::GetDateTime() const
	{
		return m_pCalendar;
	}

	void CCalendarUI::SetDateTime(TCalendarInfo* tm)
	{
		if (m_pCurDateTime->tm_year == tm->nYear && m_pCurDateTime->tm_mon == tm->nMooth && m_pCurDateTime->tm_mday == tm->nDay)
		{
			return ;
		}
		if (tm->nYear == 0 || tm->nMooth == 0 || tm->nDay == 0)
		{
			return ;
		}
		memcpy(m_pCalendar,tm,sizeof(m_pCalendar));
		m_pCurDateTime->tm_year = tm->nYear;
		m_pCurDateTime->tm_mon = tm->nMooth;
		m_pCurDateTime->tm_mday = tm->nDay;
		m_Today = tm->nDay;
		m_bIsClose = false;
		InitCalendarDis(m_pCurDateTime->tm_year,m_pCurDateTime->tm_mon);
		CDuiString szYear = _T("");
		CDuiString szMonth = _T("");
		szYear.SmallFormat(_T("%.4d"),m_pCurDateTime->tm_year);
		szMonth.SmallFormat(_T("%d"),m_pCurDateTime->tm_mon);
		m_pcob_year->SelectItem(m_pcob_year->GetItemIndex(szYear));
		m_pcob_month->SelectItem(m_pcob_month->GetItemIndex(szMonth));
		m_bIsClose = true;
	}

	bool CCalendarUI::IsClose()
	{
		return m_bIsClose;
	}

	void CCalendarUI::InitYearData()
	{
		CDuiString szYear = _T("");
		int nYear = 0;
		if (m_pcob_year->GetCount() < 1)
		{
			for (int i = 0;i< 60 ;i++)
			{
				nYear = m_nCurYear - 30 + i;
				szYear.Format(_T("%d"),nYear);
				m_pcob_year->Add(szYear);
				if (nYear == m_nCurYear)
				{
					m_pcob_year->SelectItem(i);
				}
			}
		}
	}

	void CCalendarUI::InitMonthData()
	{
		CDuiString szYear = _T("");
		int nYear = 0;
		if (m_pcob_month->GetCount() < 1)
		{
			for (int i = 0;i< 12 ;i++)
			{
				nYear = i+1;
				szYear.Format(_T("%d"),nYear);
				m_pcob_month->Add(szYear);
				if (nYear == m_nCurMooth)
				{
					m_pcob_month->SelectItem(i);
				}
			}
		}
	}

	bool CCalendarUI::IsCobWnd(HWND hWnd)
	{
		if (hWnd == NULL)
		{
			return false;
		}
		if (m_pcob_month == NULL)
		{
			return false;
		}
		if (m_pcob_year == NULL)
		{
			return false;
		}
		if (m_pcob_month->GetCobWnd() == hWnd)
		{
			return true;
		}
		if (m_pcob_year->GetCobWnd() == hWnd)
		{
			return true;
		}
		return false;
	}


	CDateTimeUI::CDateTimeUI()
	{
		m_pWindow = NULL;
		m_szCalendarAttr = _T("");
		m_szCalendarStyle = _T("");
		m_pCurDateTime = new TCalendarInfo;
		memset(m_pCurDateTime,0,sizeof(m_pCurDateTime));
		time_t now;	
		time(&now);
		struct tm* tm = localtime(&now);
		m_pCurDateTime->nYear = tm->tm_year-100+2000;
		m_pCurDateTime->nMooth = tm->tm_mon+1;
		m_pCurDateTime->nDay = tm->tm_mday;
		m_hDateTimeWnd = NULL;
		m_szFormatText = _T("");
		SetFormatStyle(0);
		//Invalidate();
	}
	CDateTimeUI::~CDateTimeUI()
	{
		if (m_pCurDateTime)
		{
			delete m_pCurDateTime;
			m_pCurDateTime = NULL;
		}
	}

	void CDateTimeUI::SetFormatStyle(int nStyle)
	{
		m_nFormatStyle = nStyle;
		if (m_nFormatStyle == 1)
		{
			m_szFormatText = _T("%.04d年%d月%d日");
		}
		else
		{
			m_szFormatText = _T("%.04d-%.02d-%.02d");
		}
		SetText(Format(m_szFormatText));
	}

	LPCTSTR CDateTimeUI::GetClass() const
	{
		return _T("DateTimeUI");
	}

	LPVOID CDateTimeUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DATETIME) == 0 ) return static_cast<CDateTimeUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void CDateTimeUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KEYDOWN )
		{
			if (IsKeyboardEnabled()) {
				if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
					Activate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				//Activate();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, _T("menu"), event.wParam, event.lParam);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_SETCURSOR ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CLabelUI::DoEvent(event);
	}

	bool CDateTimeUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pWindow ) 
		{
			return false;
		}
		m_pWindow = new CCalendarWnd;
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		Invalidate();
		return true;
	}

	void CDateTimeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("style")) == 0 ) SetCalendarStyle(pstrValue);//
		else if( _tcscmp(pstrName, _T("calendar")) == 0 ) SetCalendarAttr(pstrValue);
		else if ( _tcscmp(pstrName, _T("text")) == 0 ) return ;
		else if ( _tcscmp(pstrName, _T("format")) == 0 ) 
		{
			LPTSTR pstr = NULL;
			int nstyle = _tcstol(pstrValue, &pstr, 10);
			SetFormatStyle(nstyle);
		}
		CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void CDateTimeUI::SetDateTime(struct tm* pCurDateTime)
	{
		if (pCurDateTime->tm_year == m_pCurDateTime->nYear && pCurDateTime->tm_mon == m_pCurDateTime->nMooth && pCurDateTime->tm_mday == m_pCurDateTime->nDay) return ;
		m_pCurDateTime->nYear = pCurDateTime->tm_year;
		m_pCurDateTime->nMooth = pCurDateTime->tm_mon;
		m_pCurDateTime->nDay = pCurDateTime->tm_mday;
		SetText(Format(m_szFormatText));
	}

	TCalendarInfo* CDateTimeUI::GetDateTime()
	{
		return m_pCurDateTime;
	}

	CDuiString CDateTimeUI::Format(CDuiString szFormat)
	{
		CDuiString szData = _T("");
		szData.SmallFormat(szFormat.GetData(),m_pCurDateTime->nYear,m_pCurDateTime->nMooth,m_pCurDateTime->nDay);
		return szData;
	}

	void CDateTimeUI::SetCalendarStyle(LPCTSTR pstrValue)
	{
		m_szCalendarStyle = pstrValue;
	}

	CDuiString CDateTimeUI::GetCalendarStyle() const
	{
		return m_szCalendarStyle;
	}

	void CDateTimeUI::SetCalendarAttr(LPCTSTR pstrValue)
	{
		m_szCalendarAttr = pstrValue;
	}

	CDuiString CDateTimeUI::GetCalendarAttr() const
	{
		return m_szCalendarAttr;
	}

	void CDateTimeUI::SetDateTimeWnd(HWND hWnd)
	{
		m_hDateTimeWnd = hWnd;
	}

	HWND CDateTimeUI::GetDateTimeWnd()
	{
		return m_hDateTimeWnd;
	}
	
	void CDateTimeUI::CloseWnd()
	{
		if (m_pWindow)
		{
			m_pWindow->Close();
		}
	}
}

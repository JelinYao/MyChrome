#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__
#pragma once

namespace DuiLib
{
	typedef struct tag_CalendarStyle
	{
		CDuiString	nCalenderBorderColor;		//日历边框颜色
		CDuiString	nCalendarBkColor;			//日历背景颜色
		CDuiString	nMainTitleBkColor;			//日历主标题背景颜色
		CDuiString	nSubTitleBkColor;			//日历副标题背景颜色
		CDuiString	nWeekendColorA;				//周末的日期隔行背景颜色
		CDuiString	nWeekendColorB;				//周末的日期隔行背景颜色
		CDuiString	nDayHotColor;				//日期获得焦点时背景颜色
		CDuiString	nDayPushedColor;			//日期被按下时背景颜色
		CDuiString	nDaySelectColor;			//日期被选中时背景颜色
		CDuiString	nDayDisabledColor;			//日期被禁用时的背景色
		CDuiString	nNoCurMonthDayColor;		//非本月日期的背景颜色
		CDuiString	nWeekIntervalColorA;		//周隔行颜色A
		CDuiString	nWeekIntervalColorB;		//周隔行颜色B
		CDuiString	nStatusBkColor;				//底部信息栏背景色
		CDuiString	nWeekEndTextColor;			//周末字颜色
		CDuiString	nWeekTextColor;				//周一到周5字颜色 仅标题部分
		CDuiString  nTextColor;					//日历部分 字体颜色
	}TCalendarStyle;

	typedef struct tag_SubTitleString
	{
		CDuiString	nSundayStr;
		CDuiString	nMondayStr;
		CDuiString	nTuesdayStr;
		CDuiString	nWednesdayStr;
		CDuiString	nThursdayStr;
		CDuiString	nFridayStr;
		CDuiString	nSaturdayStr;
		CDuiString	nToDayString;
	}TSubTitleString;

	typedef struct UILIB_API tag_CalendarInfo
	{
		int		nYear;
		int		nMooth;
		int		nDay;
		int		nAsMooth;
		int		nWeek;
		int		nWeekLine;
	}TCalendarInfo;

	class UILIB_API CCalendarUI : public CVerticalLayoutUI
	{
	public:
		CCalendarUI(void);
		~CCalendarUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoEvent(TEventUI& event);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CControlUI* CreateWeekPanel(CControlUI* _Panent,int _Week);
		CControlUI* CreateDayPanel(int _Week,LPCTSTR _GroupName);
		void InitWeekPanel(CControlUI* _Control,int _Week);
		void InitDayPanel( CControlUI* _Control,bool bWeekEnd,TCalendarInfo& _CalendarInfo );
		void InitDayPanel( CControlUI* _Control,bool bWeekEnd,int _Year,int _Mooth,int _Day,int _Week,int _WeekLine,int _AsMooth );
	public:
		bool IsLeapYear(int _Year);
		int  GetNumLeapYear(int _Year);
		int  DaysOfMonth(int _Mooth,int _Year = -1);
		void InitCalendarDis(int _Year,int _Month);
		void CalDateTime(TCalendarInfo* _TCalendarInfo);
		void CalDateTime(int _Year,int _Mooth,int _Day);
		TCalendarInfo*	GetDateTime() const;
		void SetDateTime(TCalendarInfo* tm);
	public:
		bool SetSubTitleString(LPCTSTR _Name);
		void SetSubTitleString(TSubTitleString& _SubTitleString);
		void SetCalendarStyle(TCalendarStyle& _TCalendarStyle);
		bool AddSubTitleString(LPCTSTR _Name,TSubTitleString& _SubTitleString);
		bool AddSubTitleString(LPCTSTR _Name,LPCTSTR _Sunday,LPCTSTR _Monday,LPCTSTR _Tuesday,LPCTSTR _Wednesday,LPCTSTR _Thursday,LPCTSTR _Friday,LPCTSTR _Saturday);
		TSubTitleString& GetSubTitleString(LPCTSTR _Name = NULL);
		bool RemoveAtSubTitleString(LPCTSTR _Name);
		void RemoveAllSubTitleString();

	public:
		bool OnToday(void* param);
		bool OnSelcetDay( void* param );
		bool OnSelectYear(void* param);
		bool OnSelectMonth(void* param);
		void SetDateTimeControl(void* pControl);

	public:

		void AnyCalendarStyle(LPCTSTR _StyleStr,TCalendarStyle* _TCalendarStyle = NULL);
		void InitYearData();//初始年数据
		void InitMonthData();//初始月数据

		bool IsCobWnd(HWND hWnd);
		bool IsClose();

		CComboUI* GetYearCob(){return m_pcob_year;}
		CComboUI* GetMonthCob(){return m_pcob_month;}
		CButtonUI* GetToDayBtn(){return m_pbtn_today;}
		void InitWeekData(int nIndex,LPCTSTR pstrValue);

	private:
		CHorizontalLayoutUI*	m_pMainTitleHoriz;//标题部分容器
		CHorizontalLayoutUI*	m_pMainTitleLeft;//左边容器
		CHorizontalLayoutUI*	m_pMainTitleRight;//右边容器
		CLabelUI*				m_plbl_year;
		CLabelUI*				m_plbl_month;
		CComboUI*				m_pcob_year;//年
		CComboUI*				m_pcob_month;//月
		CButtonUI*				m_pbtn_today;//返回今天

		CHorizontalLayoutUI*	m_pSubTitleHoriz;//星期几容器
		CHorizontalLayoutUI*    m_pWeekPanelHoriz;//日期容器
		CVerticalLayoutUI*		m_pWeekPanelVert;//日期容器

		CDuiString				m_szCurData;//获取当前日期
		CDuiString				m_szYCurData;//阴历 日期
		int						m_nCurYear;//当前选中年
		int						m_nCurMooth;//月
		int						m_nCurDay;//日

		int						m_Today;
		struct tm*				m_pCurDateTime;

		TCalendarStyle			m_DefaultStyle;
		TSubTitleString			m_SubTitleString;
		CStdPtrArray			m_TCalendarInfoArray;
		CStdStringPtrMap		m_SubTitleStringArray;

		int m_nWidth;
		int m_nHeight;

		bool m_bIsClose;

		void* m_pDateTime;
		TCalendarInfo*			m_pCalendar;
	};

	class CCalendarWnd;
	/// 时间选择控件
	class UILIB_API CDateTimeUI : public CButtonUI
	{
		friend class CCalendarWnd;
	public:
		CDateTimeUI();
		~CDateTimeUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);

		void SetDateTimeWnd(HWND hWnd);
		HWND GetDateTimeWnd();

		bool Activate();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		
		void SetDateTime(struct tm* pCurDateTime);
		TCalendarInfo* GetDateTime();

		CDuiString Format(CDuiString szFormat);

		void SetCalendarStyle(LPCTSTR pstrValue);
		CDuiString GetCalendarStyle() const;
		void SetCalendarAttr(LPCTSTR pstrValue);
		CDuiString GetCalendarAttr() const;

		void SetFormatStyle(int nStyle);

		void CloseWnd();
		

	protected:
		CCalendarWnd* m_pWindow;
		HWND m_hDateTimeWnd;

		TCalendarInfo*			m_pCurDateTime;
		CDuiString				m_szCalendarAttr;
		CDuiString				m_szCalendarStyle;
		int						m_nFormatStyle;//格式化样式
		CDuiString				m_szFormatText;
	};
}
#endif // __UIDATETIME_H__
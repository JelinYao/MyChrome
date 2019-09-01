#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

namespace DuiLib
{

#ifndef WM_RESKIN
#define WM_RESKIN WM_USER + 100
#endif

#ifndef WM_FINISH_WINDOWINIT
#define WM_FINISH_WINDOWINIT	WM_USER + 101
#endif


	enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,				// 来自磁盘文件
		UILIB_ZIP,						// 来自磁盘zip压缩包
		UILIB_RESOURCE,			// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};

	class CButtonUI;
	class UILIB_API WindowImplBase
		: public CWindowWnd
		, public CNotifyPump
		, public INotifyUI
		, public IMessageFilterUI
		, public IDialogBuilderCallback
	{
	public:
		WindowImplBase();
		virtual ~WindowImplBase(){};
		virtual void	InitWindow(){};
		virtual void	OnFinalMessage( HWND hWnd );
		virtual void	Notify(TNotifyUI& msg);
		virtual void	OnCloseWnd();
		virtual LPCTSTR GetWindowClassName(void) const = 0 ;
		//获取窗口绘制dc用于 获取窗口快照
		virtual HDC		GetPaintDC(){ return m_PaintManager.GetPaintDC();}
		virtual HDC		GetMemDC(){ return m_PaintManager.GetMemImageDC();}
		virtual void	ReleaseMemDC(){ m_PaintManager.ReleaseMemImageDC() ;}
		virtual void	SetCanDraw(bool bCanDraw){m_PaintManager.SetCanDraw(bCanDraw);}
		virtual bool	IsCanDraw(){return m_PaintManager.IsCanDraw();}
		virtual void	ReDraw(){ m_PaintManager.ReDraw(); }

		DUI_DECLARE_MESSAGE_MAP()
		virtual void	OnClick(TNotifyUI& msg);
		virtual void	OnSelectChanged(TNotifyUI& msg) {}
		virtual void	OnPrepare(TNotifyUI& msg);
	protected:
		virtual CDuiString	GetSkinFolder() = 0;
		virtual CDuiString	GetSkinFile() = 0;
	public:
		virtual UINT	GetClassStyle() const;
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual CDuiString GetZIPFileName() const;
		virtual LPCTSTR GetResourceID() const;
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
		virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
		virtual LRESULT	OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#endif
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LONG	GetStyle();
		virtual LRESULT OnCommand( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
		virtual LRESULT OnMenuClick(WORD wId, BOOL& bHandled);
		virtual bool	HitTestControl(CControlUI* pCtrl);

		CPaintManagerUI m_PaintManager;
protected:
		CButtonUI*	m_pBtn_Close;
		CButtonUI*	m_pBtn_Max;
		CButtonUI*	m_pBtn_Restore;
		CButtonUI*	m_pBtn_Min;
		CWndShadow*	m_pShadowWindow;
		static LPBYTE m_lpResourceZIPBuffer;
	};

	class UILIB_API CMenuImplBase : public WindowImplBase
	{
	public:
		CMenuImplBase(void);
		~CMenuImplBase(void);

		virtual LPCTSTR GetWindowClassName() const; 

		virtual void OnFinalMessage(HWND hWnd);

		virtual void InitWindow(void);

		virtual void Notify(TNotifyUI& msg);

		virtual LRESULT HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual void OnCloseWnd();

		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		DUI_DECLARE_MESSAGE_MAP()
			virtual void OnClick(TNotifyUI& msg);
		
	public:
		bool m_bIsExit;

		void SetChildMenu(CMenuImplBase* pChildMenu);
		bool IsCloseChildMenu();

		CMenuImplBase* m_pChildMenu;
		CMenuImplBase* m_pParentMenu;
	};
}

#endif // WIN_IMPL_BASE_HPP

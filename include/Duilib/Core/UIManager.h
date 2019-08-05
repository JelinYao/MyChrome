#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once
#include <list>
using namespace std;
namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CControlUI;


/////////////////////////////////////////////////////////////////////////////////////
//

typedef enum EVENTTYPE_UI
{
	UIEVENT__ALL = 0,
    UIEVENT__FIRST,
    UIEVENT__KEYBEGIN,
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,
    UIEVENT__MOUSEBEGIN,
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
    UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT__MOUSEEND,
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
    UIEVENT_WINDOWSIZE,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_NOTIFY,
    UIEVENT_COMMAND,
    UIEVENT__LAST,
};

/////////////////////////////////////////////////////////////////////////////////////
//

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



/////////////////////////////////////////////////////////////////////////////////////
//

typedef struct tagTFontInfo
{
    HFONT	hFont;
    TCHAR	szFaceName[32];
    int		iSize;
    bool	bBold;
    bool	bUnderline;
    bool	bItalic;
    TEXTMETRIC tm;
	tagTFontInfo()
	{
		hFont = NULL;
		memset(szFaceName, 0, sizeof(TCHAR)*32);
		iSize		= 0;
		bBold		= false;
		bUnderline	= false;
		bItalic		= false;
		memset(&tm, 0, sizeof(TEXTMETRIC));
	}
} TFontInfo;

// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
    int Type;
    CControlUI* pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    TCHAR chKey;
    WORD wKeyState;
    WPARAM wParam;
    LPARAM lParam;
} TEventUI;

// Structure for relative position to the parent
typedef struct tagTRelativePosUI
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
}TRelativePosUI;

typedef struct tagTImageInfo
{
	HBITMAP hBitmap;
	int nX;
	int nY;
	int delay;
	bool alphaChannel;
	CDuiString sResType;
	DWORD dwMask;
} TImageInfo;


class CGifHandler
{
public:
	CGifHandler();

	virtual ~CGifHandler();

	int GetFrameCount();

	void AddFrameInfo(TImageInfo* pFrameInfo);

	TImageInfo* GetNextFrameInfo();

	TImageInfo* GetCurrentFrameInfo();

	TImageInfo* GetFrameInfoAt(int index);

	int GetCurrentFrameIndex();

	int GetNextFrameIndex();

	void SetCurrentFrameIndex(int nIndex = 0);

private:
	CStdPtrArray  ImageInfos;
	int nCurrentFrame;
	int nFrameCount;
	bool isDeleting;

};

//实现消息循环中等待HANDEL 精确时钟
struct UILIB_API IWaitForHandleCallback
{
	virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM) = 0;
};
// 
class UILIB_API WaitForHandle
{
public:
	WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l){
		m_hHandle = h;
		m_pCallback = pCB;
		m_lParam = l;
	}
	HANDLE  GetHandle() { return m_hHandle; }
	IWaitForHandleCallback* GetCB() { return m_pCallback; }
	LPARAM  GetLParam() { return m_lParam; }

protected:
	HANDLE  m_hHandle;
	IWaitForHandleCallback*  m_pCallback;
	LPARAM  m_lParam;
};
// 
class UILIB_API WaitForHandlesMgr
{
public:
	WaitForHandlesMgr(){
		m_pHandles = NULL;
		m_nHandleCount = 0;
	}
	~WaitForHandlesMgr(){

		for(int i = 0;i<m_Handlelist.GetSize();i++)
		{
			delete (WaitForHandle*)m_Handlelist[i];
		}
		m_Handlelist.Empty();
		m_nHandleCount = 0;
		if(m_pHandles){
			delete[] m_pHandles;
			m_pHandles = NULL;
		}
		m_nHandleCount = 0;
	}

	WaitForHandle* FindHandle(HANDLE h){
		if (NULL == h)
			return NULL;

		for(int i = 0;i<m_Handlelist.GetSize();i++)
		{
			if(h == ((WaitForHandle*)m_Handlelist[i])->GetHandle())
				return (WaitForHandle*)m_Handlelist[i];
		}

		return NULL;
	}
	int FindHandleIndex(HANDLE h){
		if (NULL == h)
			return -1;

		for(int i = 0;i<m_Handlelist.GetSize();i++)
		{
			if(h == ((WaitForHandle*)m_Handlelist[i])->GetHandle())
				return i;
		}
		return -1;
	}
	bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l){
		if (NULL == h || NULL == pCB)
			return false;

		if (FindHandle(h))
			return false;

		WaitForHandle* p = new WaitForHandle(h, pCB, l);
		m_Handlelist.Add(p);

		UpdateHandleArray();
		return true;
	}
	bool  RemoveHandle(HANDLE h){
		if (NULL == h)
			return false;

		int index = FindHandleIndex(h);
		if (index == -1)
			return false;
		
		WaitForHandle* p = (WaitForHandle*)m_Handlelist[index];
		if(p){
			delete p;
			p = NULL;
		}
		m_Handlelist.Remove(index);

		UpdateHandleArray();
		return true;
	}
	bool  UpdateHandleArray(){
		if(m_pHandles){
			delete[] m_pHandles;
			m_pHandles = NULL;
		}
		m_nHandleCount = m_Handlelist.GetSize();

		if (0 == m_nHandleCount)
			return true;

		m_pHandles = new HANDLE[m_nHandleCount];

		for (int i = 0; i < (int)m_nHandleCount; i++)
		{
			m_pHandles [i] = ((WaitForHandle*)m_Handlelist[i])->GetHandle();
		}
		return true;
	}
	void  Do(HANDLE h){
		WaitForHandle* pWaitForHandle = this->FindHandle(h);
		if (NULL == pWaitForHandle)
			return;

		pWaitForHandle->GetCB()->OnWaitForHandleObjectCallback(h, pWaitForHandle->GetLParam());
	}

	HANDLE*  GetHandles() { return m_pHandles; }

protected:
	CStdPtrArray  m_Handlelist;
public:
	HANDLE*  m_pHandles;
	DWORD    m_nHandleCount;  // 因为要在while循环中调用，因此作为public变量直接调用
};

// Listener interface
class INotifyUI
{
public:
    virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);

//class AnimateManager;
class UILIB_API CPaintManagerUI
{
public:
    CPaintManagerUI();
    ~CPaintManagerUI();

public:
    void Init(HWND hWnd);
    void NeedUpdate();
    void Invalidate(RECT& rcItem);

    HDC GetPaintDC() const;
    HWND GetPaintWindow() const;
    HWND GetTooltipWindow() const;

    POINT GetMousePos() const;
    SIZE GetClientSize() const;
    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);
    RECT& GetSizeBox();
    void SetSizeBox(RECT& rcSizeBox);
    RECT& GetCaptionRect();
    void SetCaptionRect(RECT& rcCaption);
    SIZE GetRoundCorner() const;
    void SetRoundCorner(int nx,int ny);
    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);
	int GetTransparent() const;
    void SetTransparent(int nOpacity);
    void SetBackgroundTransparent(bool bTrans);
	bool GetBackgroundTransparent();
    bool IsShowUpdateRect() const;
    void SetShowUpdateRect(bool show);
	bool IsEscCloseWnd() const;
	void SetEscCloseWnd(bool bClose);
	bool IsUseAero();
	void SetAero(bool bUseAero = true);
	RECT GetAeroMargins();
	void SetAeroMargins(int cx,int cy,int nx,int ny);
	bool IsUseShadow();
	void SetShadow(bool bShadow);
	RECT GetShadowBorder();
	void SetShadowBorder(int cx,int cy,int nx,int ny);
	const	TCHAR*	GetShadowImage()const;
	void	SetShadowImage(const TCHAR* pShadowImage);

    static HINSTANCE GetInstance();
    static CDuiString GetInstancePath();
    static CDuiString GetCurrentPath();
    static HINSTANCE GetResourceDll();
    static const TCHAR*		GetResourcePath();
    static const TCHAR*		GetResourceZip();
    static bool IsCachedResourceZip();
    static HANDLE GetResourceZipHandle();
    static void SetInstance(HINSTANCE hInst);
    static void SetCurrentPath(LPCTSTR pStrPath);
    static void SetResourceDll(HINSTANCE hInst);
    static void SetResourcePath(LPCTSTR pStrPath);
	static void SetResourceZip(LPVOID pVoid, unsigned int len);
    static void SetResourceZip(LPCTSTR pstrZip, bool bCachedResourceZip = false);
    static void GetHSL(short* H, short* S, short* L);
    static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
    static void ReloadSkin();
	static void ReLoadLanguage(LPCTSTR pStrName);//加载语言 不支持showhtmltext

    bool UseParentResource(CPaintManagerUI* pm);
    CPaintManagerUI* GetParentResource() const;

    DWORD GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(DWORD dwColor);
    DWORD GetDefaultFontColor() const;
    void SetDefaultFontColor(DWORD dwColor);
    DWORD GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(DWORD dwColor);
    DWORD GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(DWORD dwColor);
    DWORD GetDefaultSelectedBkColor() const;
    void SetDefaultSelectedBkColor(DWORD dwColor);
    TFontInfo* GetDefaultFontInfo();
    void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    DWORD GetCustomFontCount() const;
    HFONT AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    HFONT AddFontAt(int index, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    HFONT GetFont(int index);
    HFONT GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool FindFont(HFONT hFont);
    bool FindFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    int GetFontIndex(HFONT hFont);
    int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool RemoveFont(HFONT hFont);
    bool RemoveFontAt(int index);
    void RemoveAllFonts();
    TFontInfo* GetFontInfo(int index);
    TFontInfo* GetFontInfo(HFONT hFont);

    const TImageInfo* GetImage(LPCTSTR bitmap);
    const TImageInfo* GetImageEx(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha);
	const TImageInfo* AddImage(LPCTSTR bitmap, LPBYTE pData, DWORD dwSize, DWORD mask);//自定义扩展
    bool RemoveImage(LPCTSTR bitmap);
    void RemoveAllImages();
    void ReloadAllImages();

    void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList);
    LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
    bool RemoveDefaultAttributeList(LPCTSTR pStrControlName);
    const CStdStringPtrMap& GetDefaultAttribultes() const;
    void RemoveAllDefaultAttributeList();

	void AddLanguageAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList);
	LPCTSTR GetLanguageAttributeList(LPCTSTR pStrControlName) const;
	bool RemoveLanguageAttributeList(LPCTSTR pStrControlName);
	const CStdStringPtrMap& GetLanguagetAttribultes() const;
	void RemoveAllLanguageAttributeList();

    bool AttachDialog(CControlUI* pControl);
    bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
    void ReapObjects(CControlUI* pControl);

    bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
    void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    void RemoveAllOptionGroups();

	bool AddHotKeyGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	CStdPtrArray* GetHotKeyGroup(LPCTSTR pStrGroupName);
	void RemoveHotKeyGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	void RemoveAllHotKeyGroups();

    CControlUI* GetFocus() const;
    void SetFocus(CControlUI* pControl);
    void SetFocusNeeded(CControlUI* pControl);

    bool SetNextTabControl(bool bForward = true);

    bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
    bool KillTimer(CControlUI* pControl, UINT nTimerID);
    void KillTimer(CControlUI* pControl);
    void RemoveAllTimers();

    void SetCapture();
    void ReleaseCapture();
    bool IsCaptured();

	bool IsCanDraw();
	void SetCanDraw(bool bCanDraw);
	void Commit2PaintWindow(HDC hMemDC,int nStyle = ULW_OPAQUE ,int nx = 0,int ny = 0); //提交DC到 到窗口
	void ReDraw();
	HDC GetMemImageDC();
	void ReleaseMemImageDC();

    bool AddNotifier(INotifyUI* pControl);
    bool RemoveNotifier(INotifyUI* pControl);   
    void SendNotify(TNotifyUI& Msg, bool bAsync = false);
    void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

    bool AddPreMessageFilter(IMessageFilterUI* pFilter);
    bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

    bool AddMessageFilter(IMessageFilterUI* pFilter);
    bool RemoveMessageFilter(IMessageFilterUI* pFilter);

    int GetPostPaintCount() const;
    bool AddPostPaint(CControlUI* pControl);
    bool RemovePostPaint(CControlUI* pControl);
    bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

    void AddDelayedCleanup(CControlUI* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

    CControlUI* GetRoot() const;
    CControlUI* FindControl(POINT pt) const;
    CControlUI* FindControl(LPCTSTR pstrName) const;
    CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
    CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
    CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
    CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);
    CStdPtrArray* GetSubControlsByClass();

    static void MessageLoop();
    static bool TranslateMessage(const LPMSG pMsg);
	static void Term();

    bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
    bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	void UsedVirtualWnd(bool bUsed);

	//获取到时间handel管理对象
	//WaitForHandlesMgr* GetWaitForHandlesMgr();
	//AnimateManager* GetAnimateManager();
	void ReleaseAnimateManager();
// 	//定义消息绑定函数原型
// 	typedef  BOOL  (WINAPI * CallBackMessageHandler)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes,void* pThis);
// 	//设置消息绑定函数
// 	void SetCallBackMessageHandler( BOOL (WINAPI * pCallBackMessageHandler)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes,void* pThis),void* pThis );
// 	//消息绑定函数变量
// 	BOOL (WINAPI * m_pCallBackMessageHandlerFun)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes,void* pThis);//消息回调函数
// 	void* m_pThis;//保存回调函数类的指针

private:
    static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromUpdate(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);

private:
    HWND	m_hWndPaint;
	int		m_nOpacity;
    HDC		m_hDcPaint;
	LPBYTE	m_pBmpBackgroundBits;
    HDC		m_hDcOffscreen;
    HDC		m_hDcBackground;
    HBITMAP m_hbmpOffscreen;
    HBITMAP m_hbmpBackground;
    HWND	m_hwndTooltip;
    TOOLINFO m_ToolTip;
    bool	m_bShowUpdateRect;
    //
    CControlUI* m_pRoot;
    CControlUI* m_pFocus;
    CControlUI* m_pEventHover;
    CControlUI* m_pEventClick;
    CControlUI* m_pEventKey;
    //
    POINT m_ptLastMousePos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;
    RECT m_rcSizeBox;
    SIZE m_szRoundCorner;
    RECT m_rcCaption;
    UINT m_uTimerID;
    bool m_bFirstLayout;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;
    bool m_bOffscreenPaint;
    bool m_bAlphaBackground;
	bool m_bEscCloseWnd;
    bool m_bMouseTracking;
    bool m_bMouseCapture;
	bool m_bUsedVirtualWnd;

	bool m_bActive;//判断窗口是否激活
	bool m_bEscClose;
	bool m_bIsUserAero;//是否使用aero特效 只有win7系统才有
	RECT m_rcAeroMargins;//aero区域
    //
    CStdPtrArray m_aNotifiers;
    CStdPtrArray m_aTimers;
    CStdPtrArray m_aPreMessageFilters;
    CStdPtrArray m_aMessageFilters;
    CStdPtrArray m_aPostPaintControls;
    CStdPtrArray m_aDelayedCleanup;
    CStdPtrArray m_aAsyncNotify;
    CStdPtrArray m_aFoundControls;
    CStdStringPtrMap m_mNameHash;
    CStdStringPtrMap m_mOptionGroup;
	CStdStringPtrMap m_mHotKeyGroup;
    //
    CPaintManagerUI* m_pParentResourcePM;
    DWORD m_dwDefaultDisabledColor;
    DWORD m_dwDefaultFontColor;
    DWORD m_dwDefaultLinkFontColor;
    DWORD m_dwDefaultLinkHoverFontColor;
    DWORD m_dwDefaultSelectedBkColor;
    CStdPtrArray m_aCustomFonts;
    CStdStringPtrMap m_mImageHash;
    CStdStringPtrMap m_DefaultAttrHash;
    //
    static HINSTANCE m_hInstance;
    static HINSTANCE m_hResourceInstance;
    static TCHAR	m_szResDirPath[MAX_PATH+1];
    static TCHAR	m_szResZipPath[MAX_PATH+1];
    static bool		m_bCachedResourceZip;
    static HANDLE	m_hResourceZip;
    static short	m_H;
    static short	m_S;
    static short	m_L;
    static CStdPtrArray m_aPreMessages;
	bool	m_bCanDraw;
	//阴影窗口相关属性
	bool	m_bIsShadow;
	TCHAR	m_szShadowBkImage[MAX_PATH+1];
	RECT	m_rcShadowBorder;
public:
	static	bool	m_bInitAtFirst;
	static	TFontInfo m_DefaultFontInfo;
	static	CStdStringPtrMap m_LanguageAttrHash;
	static	bool bLoadingLanguage;
	CStdPtrArray m_aTranslateAccelerator;
//	WaitForHandlesMgr*   m_WaitForHandlesMgr;
};

} // namespace DuiLib

#endif // __UIMANAGER_H__

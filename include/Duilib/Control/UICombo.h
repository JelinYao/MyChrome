#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboWnd;
public:
    CComboUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

    CDuiString GetText() const;
    void SetEnabled(bool bEnable = true);

    CDuiString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    int GetCurSel() const;  
    bool SelectItem(int iIndex, bool bTakeFocus = false);
	
	int GetItemIndex(CDuiString szText);
    bool SetItemIndex(CControlUI* pControl, int iIndex);
    bool Add(CControlUI* pControl);
	bool Add(CDuiString szText,CListLabelElementUI* pListLabel = NULL);
    bool AddAt(CControlUI* pControl, int iIndex);
	bool AddAt(CDuiString szText, int iIndex,CListLabelElementUI* pListLabel = NULL);
    bool Remove(CControlUI* pControl);
	bool Remove(CDuiString szText);
    bool RemoveAt(int iIndex);
	bool RemoveAt(CDuiString szText);
    void RemoveAll();

    bool Activate();

    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    TListInfoUI* GetListInfo();
    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
	DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
	DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
	LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
	void SetAltBkColor(DWORD dwAltBkColor);
	DWORD GetAltBkColor() const;
	DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
	DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
	LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
	DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
	DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
	LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
	DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
	DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
	LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
	DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);


    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    
    void DoPaint(HDC hDC, const RECT& rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

	HWND GetCobWnd(); //增加 为了该控件在弹出窗口时候 关闭的时候 检测是否是本窗口句柄
	void SetCobWnd(HWND hWnd);

protected:
    CComboWnd* m_pWindow;
	HWND m_CobWnd;

    int m_iCurSel;
    RECT m_rcTextPadding;
    CDuiString m_sDropBoxAttributes;
    SIZE m_szDropBox;
    UINT m_uButtonState;

    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sPushedImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sDisabledImage;

    TListInfoUI m_ListInfo;

	bool m_bIsCheckBox;//是否带check
	bool m_bIsDeleteBtn;//是否带删除按钮
	
	CDuiString m_szDeleteBtnAttribute;//删除按钮属性
	CDuiString m_szLabelAttribute;//文字属性
	CDuiString m_szOptionAttribute;//选择框属性
	CDuiString m_szHorAttribute;//容器属性
};

} // namespace DuiLib

#endif // __UICOMBO_H__

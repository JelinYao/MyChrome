#pragma once
#include <map>
using std::map;

template<class T, int row=0, int column=0>
class CMyArray
{
public:
	CMyArray()
		:m_pData(NULL)
		,m_nRow(row)
		,m_nColumn(column)
	{
		if ( m_nRow>0 && m_nColumn>0 )
			Allocate();
	}
	~CMyArray()
	{
		Free();
	}
	bool Resize(const int nRow, const int nColumn)
	{
		if ( nRow<=0 || nColumn<=0 )
			return false;
		m_nRow		= nRow;
		m_nColumn	= nColumn;
		Allocate();
		return true;
	}
	T GetAt(const int nRow, const int nColumn)
	{
		if ( nRow>=m_nRow || nColumn>=m_nColumn )
			return T();
		return *(m_pData+nRow*m_nColumn+nColumn);
	}
	void SetAt(const int nRow, const int nColumn, const T& data)
	{
		if ( nRow>=m_nRow || nColumn>=m_nColumn )
			return ;
		*(m_pData+nRow*m_nColumn+nColumn) = data;
	}
protected:
	void Allocate()
	{
		Free();
		m_pData = (T*)malloc(m_nRow*m_nColumn*sizeof(T));
	}
	void Free()
	{
		if ( m_pData )
		{
			free(m_pData);
			m_pData = NULL;
		}
	}
private:
	T*	m_pData;
	int	m_nRow;
	int m_nColumn;
};

class CSelectColorCallback
{
public:
	virtual void	ColorTest(DWORD color)=0;
};

class CColorSkinUI
	: public CControlUI
{
public:
	CColorSkinUI();
	virtual ~CColorSkinUI();
	virtual void	SetPos(RECT rc);
	virtual void	DoInit();
	virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void	DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void	DoEvent(TEventUI& event);
	void	SetSelectColorCallback(CSelectColorCallback* pCallback) { m_pSelectCallback = pCallback; }
	DWORD	GetSelectColor();
	int		GetCurSelLine()					{ return m_nCurSelLine; }
	int		GetCurSelCol()					{ return m_nCurSelCol; }
	void	SetCurSelLine(const int nLine)	{ m_nCurSelLine = nLine; }
	void	SetCurSelCol(const int nColumn) { m_nCurSelCol = nColumn; }
	void	SetCurSelPos(const int nLine, const int nColumn);
protected:
	void	InitBitmap(int nWidth, int nHeight);
	void	PaintPalette(bool bPaintAll=true, const int nCurLine=0, const int nCulCol=0);
	DWORD	PaintSide(HDC hDC, const int nLine, const int nColumn, bool bSelect);
private:
	int		m_nSideLen;
	int		m_nLine;
	int		m_nColumn;
	HDC		m_hMemDC;
	HBITMAP	m_hMemBmp;
	BITMAP	m_bitmap;
	void*	m_pBuffer;
	int		m_nPreWidth;
	int		m_nPreHeight;
	int		m_nCurSelLine;
	int		m_nCurSelCol;
	int		m_nPreCurSelLine;
	int		m_nPreCurSelCol;
	CMyArray<DWORD> m_arrColor;
	CSelectColorCallback*	m_pSelectCallback;
};



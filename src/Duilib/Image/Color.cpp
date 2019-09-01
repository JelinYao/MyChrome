#ifndef Color_h__
#define Color_h__

#include "stdafx.h"

namespace DuiLib
{
	DuiColor::DuiColor()
	{
		m_col = 0;
		lRef = 0;
	}
	DuiColor::DuiColor(int _r, int _g, int _b, int _a)
	{
		lRef = 0;
		this->a = _a;
		this->r = _r;
		this->g = _g;
		this->b = _b;
	}
	DuiColor::DuiColor(DWORD rgba)
	{
		lRef = 0;
		m_col = rgba;
	}
	DuiColor* DuiColor::CreateInstance(COLORREF color)
	{
		DuiColor* p = new DuiColor(color);
		p->AddRef();
		return p;
	}

	DuiColor::operator COLORREF()
	{
		COLORREF c = m_col;
		c &= 0x00FFFFFF;
		return c;
	}

	int   DuiColor::GetR() { return r; }
	int   DuiColor::GetG() { return g; }
	int   DuiColor::GetB() { return b; }
	int   DuiColor::GetA() { return a; }

	void  DuiColor::SetR(byte n) { r = n; }
	void  DuiColor::SetG(byte n) { g = n; }
	void  DuiColor::SetB(byte n) { b = n; }
	void  DuiColor::SetA(byte n) { a = n; }

	// ±£ÁôALPHAWÖµÑÕÉ«¿½±´
	void  DuiColor::ReplaceRGB(COLORREF c)
	{
		byte a_old = a;
		m_col = c;
		a = a_old;
	}
	void  DuiColor::ReplaceRGB(BYTE r1, BYTE g1, BYTE b1)
	{
		r = r1; g = g1; b = b1;
	}
	void  DuiColor::SetColor(COLORREF c)
	{
		m_col = c;
		a = 255;
	}

	COLORREF  DuiColor::GetGDICompatibleValue()
	{
		COLORREF gdiColor = 0;
		if (255 == a)
		{
			gdiColor = RGB(r, g, b);
		}
		else if (0 == a)
		{
		}
		else
		{
			DuiColor c;
			byte br = r * a >> 8;
			byte bg = g * a >> 8;
			byte bb = b * a >> 8;

			gdiColor = RGB(c.r, c.g, c.b);
		}

		return gdiColor;
	}

	long  DuiColor::AddRef() { ++ lRef;  return lRef; }
	long  DuiColor::Release()
	{
		--lRef;
		if (lRef == 0)
		{
			delete this;
			return 0;
		}
		return lRef;
	}

	inline void Image9Region::Set( WORD w )
	{
		topleft = top = topright = left = right = bottomleft = bottom = bottomright = w;
	}

	inline void Image9Region::Set( WORD wLeftRight, WORD wTopBottom )
	{
		topleft = topright = left = right = bottomleft = bottomright = wLeftRight;
		top = bottom = wTopBottom;
	}

	inline void Image9Region::Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom )
	{
		topleft = left = bottomleft = wLeft;
		top = wTop;
		topright = right = bottomright = wRight;
		bottom = wBottom;
	}
}
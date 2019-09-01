#ifndef Image_h__
#define Image_h__
#include "stdafx.h"
namespace DuiLib
{
	CDuiImage::CInitGDIPlus		CDuiImage::s_initGDIPlus;
	CDCCache			    CDuiImage::s_cache;

	// 将一张图片的边缘渐变化，用于修改一张要作为窗口皮肤的图片，使边缘部分与背景颜色融合
	void  CDuiImage::AlphaEdge(int nEdge, int nSpeed)
	{
		// 将自己转成带alpha channel的格式
		if (m_nBPP != 32)
		{
			CDuiImage alphaimage;
			alphaimage.Create(m_nWidth, m_nHeight, 32, createAlphaChannel);
			HDC hDC = alphaimage.BeginDrawToMyself();
			this->BitBlt(hDC,0,0);
			alphaimage.EndDrawToMyself();
			alphaimage.SetAlpha(255);
			this->Destroy();
			*this = alphaimage;
			alphaimage.Detach();
		}

		if (nEdge & ImageAlphaEdge_Bottom)
		{
			byte* p = (byte*)m_pBits + (m_nHeight-1)*m_nPitch;
			int  bBytesPerline = abs(m_nPitch);

			int alpha = 0;
			for (int y = m_nHeight; y > 0 && alpha <= 255; y--, alpha+=nSpeed)
			{
				for (int x = 0; x < bBytesPerline; x++)
				{
					p[x] = p[x]*alpha >> 8;
				}

				p -= m_nPitch;
			}
		}
		if (nEdge & ImageAlphaEdge_Top)
		{
			byte* p = (byte*)m_pBits;
			int  bBytesPerline = abs(m_nPitch);

			int alpha = 0;
			for (int y = 0; y < m_nHeight && alpha <= 255; y++, alpha+=nSpeed)
			{
				for (int x = 0; x < bBytesPerline; x++)
				{
					p[x] = p[x]*alpha >> 8;
				}

				p += m_nPitch;
			}
		}

		if (nEdge & ImageAlphaEdge_Left)
		{
			byte* p = (byte*)m_pBits;
			int  bBytesPerline = abs(m_nPitch);

			for (int y = 0; y < m_nHeight; y++)
			{
				int alpha = 0;
				for (int x = 0; x < bBytesPerline && alpha <= 255; alpha+=nSpeed)
				{
					p[x] = p[x]*alpha >> 8; x++;
					p[x] = p[x]*alpha >> 8; x++;
					p[x] = p[x]*alpha >> 8; x++;
					p[x] = p[x]*alpha >> 8; x++;
				}

				p += m_nPitch;
			}
		}

		if (nEdge & ImageAlphaEdge_Right)
		{
			byte* p = (byte*)m_pBits;
			int  bBytesPerline = abs(m_nPitch);

			for (int y = 0; y < m_nHeight; y++)
			{
				int alpha = 0;
				for (int x = bBytesPerline-1; x > 0 && alpha <= 255; alpha+=nSpeed)
				{
					p[x] = p[x]*alpha >> 8;  x--;
					p[x] = p[x]*alpha >> 8;  x--;
					p[x] = p[x]*alpha >> 8;  x--;
					p[x] = p[x]*alpha >> 8;  x--;
				}

				p += m_nPitch;
			}
		}
	}



	// 注：已被Util::FixGdiAlpha取代，但该函数仍然可以使用
	//
	// 将nAlphaIsZero==1的位置重新设置alpha=0（配置SetAlpha使用）
	// 将其它位置alpha设置为255。 用于richedit在分层窗口上的绘制
	bool CDuiImage::FixGDIAlphaChannel(int nAlphaIsZero)
	{
		if (m_nBPP != 32)
			return false;

		BYTE* pImageBits = (BYTE*)m_pBits;
		for (int row = 0; row < m_nHeight; row ++)
		{
			for (int i = 0; i < m_nWidth; i ++)
			{
				int nAlphaIndex = 4*i+3;
				int nAlpha = pImageBits[nAlphaIndex];
				if (nAlphaIsZero == nAlpha)
				{
					pImageBits[nAlphaIndex] = 0;
				}
				else if (0 == nAlpha)
				{
					pImageBits[nAlphaIndex] = 255;
				}
			}
			pImageBits+= m_nPitch;
		}

		return true;
	}

	DWORD  CDuiImage::GetAverageColor(RECT* prc)
	{
		if (NULL == prc)
			return 0;
		if (m_nBPP != 24 && m_nBPP != 32)   // 仅支持24、32位的图片
			return 0;

		if (NULL == m_hBitmap)
			return 0;

		int nWidth = prc->right-prc->left;
		int nHeight = prc->bottom-prc->top;

		int nCount = nWidth*nHeight;
		if (0 == nCount)
			return 0;

		BYTE* pTemp = (BYTE*)m_pBits + prc->top*m_nPitch;

		int   bytesperpx    = m_nBPP>>3;
		//   int   bytesperline   = bytesperpx*nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
		bool  bHaveAlphaChannel = GetBPP() == 32;

		LONGLONG  a = 0, g = 0, b = 0, r = 0;
		if (bHaveAlphaChannel)
		{
			for (int row = prc->top; row < prc->bottom; row ++)
			{
				int nLeftPos = prc->left*bytesperpx;
				int nRightPos = prc->right*bytesperpx;
				for( int i = nLeftPos; i < nRightPos; i += bytesperpx)
				{
					b += pTemp[i];
					g += pTemp[i+1];
					r += pTemp[i+2];
					a += pTemp[i+3];
				}
				pTemp += m_nPitch;
			}
		}
		else
		{
			for (int row = prc->top; row < prc->bottom; row ++ )
			{
				int nLeftPos = prc->left*bytesperpx;
				int nRightPos = prc->right*bytesperpx;
				for( int i = nLeftPos; i < nRightPos; i += bytesperpx)
				{
					b += pTemp[i];
					g += pTemp[i+1];
					r += pTemp[i+2];
				}
				pTemp += m_nPitch;
			}
		}

		a = a/nCount;
		r = r/nCount;
		g = g/nCount;
		b = b/nCount;

		return ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)));
	}

	// 获取图像的平均色值
	DWORD CDuiImage::GetAverageColor()
	{
		RECT  rc = {0, 0, m_nWidth, m_nHeight};
		return GetAverageColor(&rc);

		//     if (m_nBPP != 24 && m_nBPP != 32)   // 仅支持24、32位的图片
		//         return 0;
		// 
		//     if (NULL == m_hBitmap)
		//         return 0;
		// 
		//     int nCount = m_nWidth*m_nHeight;
		//     if (0 == nCount)
		//         return 0;
		// 
		//     BYTE* pTemp = (BYTE*)m_pBits;
		//     int   bytesperpx    = m_nBPP>>3;
		//     int   bytesperline   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
		//     bool  bHaveAlphaChannel = GetBPP() == 32;
		// 
		//     LONGLONG  a = 0, g = 0, b = 0, r = 0;
		//     if (bHaveAlphaChannel)
		//     {
		//         for (int row = 0; row < m_nHeight; row ++ )
		//         {
		//             for( int i = 0; i < bytesperline; i += bytesperpx )
		//             {
		//                 b += pTemp[i];
		//                 g += pTemp[i+1];
		//                 r += pTemp[i+2];
		//                 a += pTemp[i+3];
		//             }
		//             pTemp += m_nPitch;
		//         }
		//     }
		//     else
		//     {
		//         for (int row = 0; row < m_nHeight; row ++ )
		//         {
		//             for( int i = 0; i < bytesperline; i += bytesperpx )
		//             {
		//                 b += pTemp[i];
		//                 g += pTemp[i+1];
		//                 r += pTemp[i+2];
		//             }
		//             pTemp += m_nPitch;
		//         }
		//     }
		// 
		//     a = a/nCount;
		//     r = r/nCount;
		//     g = g/nCount;
		//     b = b/nCount;
		// 
		//     return ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)));
	}


	HBITMAP CDuiImage::CopyImageByRect(RECT* prc)
	{
		if (NULL == prc)
			return NULL;

		int nWidth = prc->right - prc->left;
		int nHight = prc->bottom - prc->top;

		CDuiImage image;
		image.Create(nWidth, nHight, 32, CDuiImage::createAlphaChannel);

		HDC hDC = image.GetDC();
		this->BitBlt(hDC, 0,0, nWidth, nHight, prc->left, prc->top);
		image.ReleaseDC();

		HBITMAP hRetValue = image.Detach();
		return hRetValue;
	}	
}
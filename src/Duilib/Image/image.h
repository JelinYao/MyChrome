#ifndef Image_h__
#define Image_h__
#pragma once

#pragma warning(disable:4312)
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#pragma warning( push, 3 )
#pragma warning( pop )


#include <shlwapi.h>
#include <malloc.h>

// 在版本小于0x0500时，显示PNG图片不会透明
#ifndef WINVER				 
	#define WINVER 0x0500	 
#elif WINVER < 0x0500
		#undef  WINVER
		#define WINVER 0x0500
#endif

#define CLR_USE_COLOR_DIRECT    0xFFFFFFFE

// 注意：请屏蔽掉stdafx.h中的 
// #define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// 否则会报错
#include <GdiPlus.h>
#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;

#include <assert.h>
#include <string>
#include <vector>
using namespace std;

#define MIN_HUE_VALUE           0              // 色调  HLS范围定义
#define MAX_HUE_VALUE           240
#define MIN_LUMINANCE_VALUE     0              // 亮度
#define MAX_LUMINANCE_VALUE     240            
#define MIN_SATURATION_VALUE    0              // 饱和度
#define MAX_SATURATION_VALUE    240   


#define CHANGE_SKIN_HLS_FLAG_H             1
#define CHANGE_SKIN_HLS_FLAG_L             2
#define CHANGE_SKIN_HLS_FLAG_S             4
#define CHANGE_SKIN_HLS_FLAG_HLS           (CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_L|CHANGE_SKIN_HLS_FLAG_S)
#define CHANGE_SKIN_HLS_FALG_REPLACE_MODE  8 // 着色模式


#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#if WINVER >= 0x0500
#pragma comment(lib, "msimg32.lib")
#endif  // WINVER >= 0x0500


namespace DuiLib
{
	const int CIMAGE_DC_CACHE_SIZE = 4;

	class CDuiImage;
	// 
	//	图片内存数据拷贝，用于皮肤改变色调时保存其原始数据，并兼容gdi/gdi+图片
	//
	class UILIB_API ImageData
	{
	public:
		ImageData()
		{
			m_ptr = m_pScan0 = 0;
			m_nbpp = m_nStride = m_nWidth = m_nHeight = 0;
		}
		~ImageData()
		{
			if( NULL != m_ptr )
				delete[] m_ptr;
			m_ptr = m_pScan0 = 0;
			m_nbpp = m_nStride = m_nWidth = m_nHeight = 0;
		}

		BYTE*  m_ptr;         // 数据首地址，用于new/delete
		BYTE*  m_pScan0;      // 图片第一行的数据地址
		int    m_nbpp;        // bit per pixel
		int    m_nStride;     // Offset, in bytes, between consecutive scan lines of the bitmap. If the stride is positive, the bitmap is top-down. If the stride is negative, the bitmap is bottom-up.
		int    m_nWidth;      // 图片宽度
		int    m_nHeight;     // 图片高度
	};

	class UILIB_API CImageDC
	{
	public:
		CImageDC( const CDuiImage& image ) throw( ... );
		~CImageDC() throw();

		operator HDC() const throw();

	private:
		const CDuiImage& m_image;
		HDC m_hDC;
	};

	class UILIB_API CDCCache
	{
	public:
		CDCCache() throw();
		~CDCCache() throw();

		HDC GetDC() throw();
		void ReleaseDC( HDC ) throw();

	private:
		HDC m_ahDCs[CIMAGE_DC_CACHE_SIZE];
	};


	class UILIB_API CDuiImage
	{
	private:
		class CInitGDIPlus
		{
		public:
			CInitGDIPlus();
			~CInitGDIPlus() throw();

			bool Init() throw();
			void ReleaseGDIPlus() throw();
			void IncreaseCImageCount() throw();
			void DecreaseCImageCount() throw();

		private:
			ULONG_PTR m_dwToken;
			CRITICAL_SECTION m_sect;
			LONG m_nCImageObjects;
		};

	public:
		static const DWORD createAlphaChannel = 0x01;

		static const DWORD excludeGIF = 0x01;
		static const DWORD excludeBMP = 0x02;
		static const DWORD excludeEMF = 0x04;
		static const DWORD excludeWMF = 0x08;
		static const DWORD excludeJPEG = 0x10;
		static const DWORD excludePNG = 0x20;
		static const DWORD excludeTIFF = 0x40;
		static const DWORD excludeIcon = 0x80;
		static const DWORD excludeOther = 0x80000000;
		static const DWORD excludeDefaultLoad = 0;
		static const DWORD excludeDefaultSave = excludeIcon|excludeEMF|excludeWMF;
		static const DWORD excludeValid = 0x800000ff;

		enum DIBOrientation
		{
			DIBOR_DEFAULT,
			DIBOR_TOPDOWN,
			DIBOR_BOTTOMUP
		};

        enum ImageAlphaEdge
        {
            ImageAlphaEdge_Left = 1,
            ImageAlphaEdge_Top  = 2,
            ImageAlphaEdge_Right = 4,
            ImageAlphaEdge_Bottom = 8,
        };
	public:
		CDuiImage() throw();
		virtual ~CDuiImage() throw();

		operator HBITMAP() const throw();
		HBITMAP GetHBITMAP() { return m_hBitmap; }
#if WINVER >= 0x0500
		BOOL  AlphaBlend( HDC hDestDC, int xDest, int yDest, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
		BOOL  AlphaBlend( HDC hDestDC, const POINT& pointDest, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
		BOOL  AlphaBlend( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
		BOOL  AlphaBlend( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
#endif  // WINVER >= 0x0500
		void  Attach( HBITMAP hBitmap, DIBOrientation eOrientation = DIBOR_DEFAULT ) throw();
		BOOL  BitBlt( HDC hDestDC, int xDest, int yDest, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  BitBlt( HDC hDestDC, const POINT& pointDest, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  BitBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  BitBlt( HDC hDestDC, const RECT& rectDest, const POINT& pointSrc, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  Create( int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0 ) throw();
		BOOL  CreateEx( int nWidth, int nHeight, int nBPP, DWORD eCompression, const DWORD* pdwBitmasks = NULL, DWORD dwFlags = 0 ) throw();
		void  Destroy() throw();
		HBITMAP Detach() throw();
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, byte bAlpha=0xFF ) const throw();
		/* bo.li ADD 20101202 增加9宫绘制 */
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, Image9Region* pImage9Region, bool bDrawCenter=true, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, const POINT& pointDest, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, const RECT& rectDest, byte bAlpha=0xFF ) const throw();
		const void* GetBits() const throw();
		void* GetBits() throw();
		int   GetBPP() const throw();
		void  GetColorTable( UINT iFirstColor, UINT nColors, RGBQUAD* prgbColors ) const throw();
		HDC   GetDC() const throw();
		static HRESULT GetExporterFilterString( CDuiString& strExporters, vector< GUID >& aguidFileTypes, LPCTSTR pszAllFilesDescription = NULL, DWORD dwExclude = excludeDefaultSave, TCHAR chSeparator = _T( '|' ) );
		static HRESULT GetImporterFilterString( CDuiString& strImporters, vector< GUID >& aguidFileTypes, LPCTSTR pszAllFilesDescription = NULL, DWORD dwExclude = excludeDefaultLoad, TCHAR chSeparator = _T( '|' ) );
		static HDC   GetCacheDC(){ return s_cache.GetDC(); }
		static void  ReleaseCacheDC(HDC hDC) { s_cache.ReleaseDC(hDC); }

		int  GetHeight() const throw();
		int  GetMaxColorTableEntries() const throw();
		int  GetPitch() const throw();
		const void* GetPixelAddress( int x, int y ) const throw();
		void* GetPixelAddress( int x, int y ) throw();
		COLORREF GetPixel( int x, int y ) const throw();
		LONG GetTransparentColor() const throw();
		int  GetWidth() const throw();
		bool IsDIBSection() const throw();
		bool IsIndexed() const throw();
		bool IsNull() const throw();
		HRESULT Load( LPCTSTR pszFileName ) throw();
		HRESULT Load( IStream* pStream ) throw();
		void LoadFromResource( HINSTANCE hInstance, LPCTSTR pszResourceName ) throw();
		void LoadFromResource( HINSTANCE hInstance, UINT nIDResource ) throw();
		/* <BEGIN ADD libo 20101116 添加从资源中加载png图片的方法 */
		void LoadFromResource( HINSTANCE hInstance, UINT nIDResource, TCHAR* szResourceType );
		bool LoadFromData( void* pImageData, int nSize );
		/* ADD END> */
		HRESULT CreateFromGdiplusBitmap( Gdiplus::Bitmap& bmSrc ) throw();
		HRESULT Create32BPPFromGdiplusBitmap( Gdiplus::Bitmap& bmSrc ) throw();

		BOOL MaskBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, HBITMAP hbmMask, int xMask, int yMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL MaskBlt( HDC hDestDC, const RECT& rectDest, const POINT& pointSrc, HBITMAP hbmMask, const POINT& pointMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL MaskBlt( HDC hDestDC, int xDest, int yDest, HBITMAP hbmMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL MaskBlt( HDC hDestDC, const POINT& pointDest, HBITMAP hbmMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, HBITMAP hbmMask = NULL ) const throw();
		BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, HBITMAP hbmMask = NULL, int xMask = 0, int yMask = 0 ) const throw();
		/* ↓ 修改：将CPoint->Point,并去掉默认参数CPoint(0,0) */
		BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, const RECT& rectSrc, const POINT& pointMask, HBITMAP hbmMask = NULL ) const throw();
		void ReleaseDC() const throw();
		HRESULT Save( IStream* pStream, REFGUID guidFileType ) const throw();
		HRESULT Save( LPCTSTR pszFileName, REFGUID guidFileType = GUID_NULL ) const throw();
		void SetColorTable( UINT iFirstColor, UINT nColors, const RGBQUAD* prgbColors ) throw();
		void SetPixel( int x, int y, COLORREF color ) throw();
		void SetPixelIndexed( int x, int y, int iIndex ) throw();
		void SetPixelRGB( int x, int y, BYTE r, BYTE g, BYTE b ) throw();
		LONG SetTransparentColor( LONG iTransparentColor ) throw();
		COLORREF SetTransparentColor( COLORREF colorTransparent ) throw();
		BOOL StretchBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, DWORD dwROP = SRCCOPY ) const throw();
		BOOL StretchBlt( HDC hDestDC, const RECT& rectDest, DWORD dwROP = SRCCOPY ) const throw();
		BOOL StretchBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP = SRCCOPY ) const throw();
		BOOL StretchBlt( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, DWORD dwROP = SRCCOPY ) const throw();
#if WINVER >= 0x0500
		BOOL TransparentBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, UINT crTransparent = CLR_INVALID ) const throw();
		BOOL TransparentBlt( HDC hDestDC, const RECT& rectDest, UINT crTransparent = CLR_INVALID ) const throw();
		BOOL TransparentBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, UINT crTransparent = CLR_INVALID ) const throw();
		BOOL TransparentBlt( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, UINT crTransparent = CLR_INVALID ) const throw();
#endif  // WINVER >= 0x0500

		static BOOL IsTransparencySupported() throw();
		void   ForceUseAlpha(){m_bHasAlphaChannel = true ;}   // libo add 20110710 在调用Create时，我需要创建一个带通道的位图，但没有提供给我接口，因此增加一个
		bool   HasAlphaChannel() const { return m_bHasAlphaChannel; }
		bool   ChangeDIB2DDB(HDC hMemDC);                     // libo add 20120318 TransparentBlt只能适应于DDB，因此提供一个接口将DIB转换成DDB
		bool   CopyGrayImageFrom( const CDuiImage* pImage );         // libo add 20120321 去色
		bool   DrawColor(HDC hDC, const RECT& rc , DWORD color);
		bool   DrawColor(HDC hDC, DWORD color,int nx = 0,int ny = 0,int nwidth = 0,int nheight = 0);
		bool   DrawGray(HDC hDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);  // libo add 20120927
		bool   CreateEmptyImage(int nWidth,int nHeight,int nBitsCount,DWORD dwMode = CDuiImage::excludePNG ,CDuiImage* pImg = NULL);
		void   GetSystemPaletteEntries(RGBQUAD *pRGBQuad,int nBitsCount);
		bool   CopyImage(CDuiImage* psrcImage,int nx = 0,int ny = 0,int nWidth = 0,int nHeight = 0 ,int nBitsCount = 32);
		bool   CopyImageFromDC(HDC hDC,int nx,int ny,int nWidth,int nHeight,int nBitsCount);
		PBYTE GetBitmapBits();
		bool   ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag );
		bool   SaveBits( ImageData* pImageData, int* pNeedSize );
		void   RestoreBits( ImageData* pImageData );
		bool   ImageList_Draw(HDC hDestDC, int x, int y, int col, int row, int cx, int cy );  // libo add 20120401 增加图像列表绘制方法
		HBITMAP  CopyImageByRect(RECT* prc);    // libo add 20121019 增加拷贝图片的一部分的方法
		DWORD  GetAverageColor();      // libo add 20121128 增加获取图片平均色的方法
        DWORD  GetAverageColor(RECT* prc);
		bool   ModifyAlpha(ImageData* pOriginImageData, int nAlphaPercent);
		bool   SetAlpha(int nAlpha);
		bool   FixGDIAlphaChannel(int nAlphaIsZero=1);
        void   AlphaEdge(int nEdge, int nSpeed=5);
		
		// libo add 20121027 增加一个向该Bitmap绘制内容的方法
		HDC    BeginDrawToMyself() { this->GetDC(); return m_hDC; }
		void   EndDrawToMyself() { this->ReleaseDC(); }

	private:
		HBITMAP m_hBitmap;
		void* m_pBits;
		int   m_nWidth;
		int   m_nHeight;
		int   m_nPitch;
		int   m_nBPP;               // bit per pixel
		bool  m_bIsDIBSection;
		bool  m_bHasAlphaChannel;
		LONG  m_iTransparentColor;    // -1 表示未采用, -2 表示直接使用m_colorTransparent
		COLORREF m_colorTransparent; // 当m_iTransparentColor为-2时有效


		static CInitGDIPlus s_initGDIPlus;

	public:  // modified by libo 2012-2-17 为了使其它外部类也能引用该对象来加减引用计数
		inline static void ReleaseGDIPlus()      { DecreaseCImageCount(); s_initGDIPlus.ReleaseGDIPlus(); }
		inline static bool InitGDIPlus() throw() { IncreaseCImageCount(); return  s_initGDIPlus.Init(); }
		inline static void IncreaseCImageCount() { s_initGDIPlus.IncreaseCImageCount(); }
		inline static void DecreaseCImageCount() { s_initGDIPlus.DecreaseCImageCount(); }

		// Implementation
	private:
		static CLSID FindCodecForExtension( LPCTSTR pszExtension, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs );
		static CLSID FindCodecForFileType( REFGUID guidFileType, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs );
		static void BuildCodecFilterString( const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs, CDuiString& strFilter, vector< GUID >& aguidFileTypes, LPCTSTR pszAllFilesDescription, DWORD dwExclude, TCHAR chSeparator );
		static bool ShouldExcludeFormat( REFGUID guidFileType, DWORD dwExclude ) throw();
		void UpdateBitmapInfo( DIBOrientation eOrientation );


		static int ComputePitch( int nWidth, int nBPP )
		{
			return( (((nWidth*nBPP)+31)/32)*4 );
		}
		static void GenerateHalftonePalette( LPRGBQUAD prgbPalette );
	public:
		COLORREF GetTransparentRGB() const;

	private:
		mutable HDC m_hDC;
		mutable int m_nDCRefCount;
		mutable HBITMAP m_hOldBitmap;

		static CDCCache s_cache;
	};
	//CImage::CInitGDIPlus CImage::s_initGDIPlus;
	//CImage::CDCCache	CImage::s_cache;

	inline CImageDC::CImageDC( const CDuiImage& image ) throw( ... ) :
	m_image( image ),m_hDC( image.GetDC() )
	{
		if( m_hDC == NULL )
		{
			throw( E_OUTOFMEMORY );
		}
	}

	inline CImageDC::~CImageDC() throw()
	{
		m_image.ReleaseDC();
	}

	inline CImageDC::operator HDC() const throw()
	{
		return( m_hDC );
	}

	inline CDuiImage::CInitGDIPlus::CInitGDIPlus():
	m_dwToken( 0 ), m_nCImageObjects( 0 )
	{
		__try
		{
			InitializeCriticalSection(&m_sect);
		}
		__except( STATUS_NO_MEMORY == GetExceptionCode() )
		{
			throw( E_OUTOFMEMORY );
		}
	}

	inline CDuiImage::CInitGDIPlus::~CInitGDIPlus() throw()
	{
		ReleaseGDIPlus();
		DeleteCriticalSection(&m_sect);
	}

	inline bool CDuiImage::CInitGDIPlus::Init() throw()
	{
		EnterCriticalSection(&m_sect);
		bool fRet = true;
		if( m_dwToken == 0 )
		{
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartupOutput output;
			Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, &output );
			if( status != Gdiplus::Ok )
				fRet = false;
		}
		LeaveCriticalSection(&m_sect);
		return fRet;
	}

	inline void CDuiImage::CInitGDIPlus::ReleaseGDIPlus() throw()
	{
		EnterCriticalSection(&m_sect);
		if( m_dwToken != 0 )
		{
			Gdiplus::GdiplusShutdown( m_dwToken );
		}
		m_dwToken = 0;
		LeaveCriticalSection(&m_sect);
	}

	inline void CDuiImage::CInitGDIPlus::IncreaseCImageCount() throw()
	{
		EnterCriticalSection(&m_sect);
		m_nCImageObjects++;
		LeaveCriticalSection(&m_sect);
	}

	inline void CDuiImage::CInitGDIPlus::DecreaseCImageCount() throw()
	{
		EnterCriticalSection(&m_sect);
		if( --m_nCImageObjects == 0 )
			ReleaseGDIPlus();
		LeaveCriticalSection(&m_sect);
	}

	inline CDCCache::CDCCache() throw()
	{
		int iDC;

		for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			m_ahDCs[iDC] = NULL;
		}
	}

	inline CDCCache::~CDCCache() throw()
	{
		int iDC;

		for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			if( m_ahDCs[iDC] != NULL )
			{
				::DeleteDC( m_ahDCs[iDC] );
			}
		}
	}

	inline HDC CDCCache::GetDC() throw()
	{
		HDC hDC;

		for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			hDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), NULL ) );
			if( hDC != NULL )
			{
				return( hDC );
			}
		}

		hDC = ::CreateCompatibleDC( NULL );

		return( hDC );
	}

	inline void CDCCache::ReleaseDC( HDC hDC ) throw()
	{
		for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			HDC hOldDC;

			hOldDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), hDC ) );
			if( hOldDC == NULL )
			{
				return;
			}
			else
			{
				hDC = hOldDC;
			}
		}
		if( hDC != NULL )
		{
			::DeleteDC( hDC );
		}
	}

	inline CDuiImage::CDuiImage() throw() :
	m_hBitmap( NULL ),
		m_pBits( NULL ),
		m_hDC( NULL ),
		m_nDCRefCount( 0 ),
		m_hOldBitmap( NULL ),
		m_nWidth( 0 ),
		m_nHeight( 0 ),
		m_nPitch( 0 ),
		m_nBPP( 0 ),
		m_iTransparentColor( -1 ),
		m_colorTransparent(RGB(255,0,255)),
		m_bHasAlphaChannel( false ),
		m_bIsDIBSection( false )
	{
		s_initGDIPlus.IncreaseCImageCount();
	}

	inline CDuiImage::~CDuiImage() throw()
	{
		Destroy();
		s_initGDIPlus.DecreaseCImageCount();
	}

	inline CDuiImage::operator HBITMAP() const throw()
	{
		return( m_hBitmap );
	}

#if WINVER >= 0x0500
	inline BOOL CDuiImage::AlphaBlend( HDC hDestDC, int xDest, int yDest, 
		BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		return( AlphaBlend( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, 
			m_nWidth, m_nHeight, bSrcAlpha, bBlendOp ) );
	}

	inline BOOL CDuiImage::AlphaBlend( HDC hDestDC, const POINT& pointDest, 
		BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		return( AlphaBlend( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 
			0, 0, m_nWidth, m_nHeight, bSrcAlpha, bBlendOp ) );
	}

	inline BOOL CDuiImage::AlphaBlend( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
		int nSrcHeight, BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		BLENDFUNCTION blend;
		BOOL bResult;

		blend.SourceConstantAlpha = bSrcAlpha;
		blend.BlendOp = bBlendOp;
		blend.BlendFlags = 0;
		if( m_bHasAlphaChannel )
		{
			blend.AlphaFormat = AC_SRC_ALPHA;
		}
		else
		{
			blend.AlphaFormat = 0;
		}

		GetDC();

		bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC, 
			xSrc, ySrc, nSrcWidth, nSrcHeight, blend );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL CDuiImage::AlphaBlend( HDC hDestDC, const RECT& rectDest, 
		const RECT& rectSrc, BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		return( AlphaBlend( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
			rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, bSrcAlpha, 
			bBlendOp ) );
	}
#endif  // WINVER >= 0x0500

	inline void CDuiImage::Attach( HBITMAP hBitmap, DIBOrientation eOrientation ) throw()
	{
		assert( m_hBitmap == NULL );
		assert( hBitmap != NULL );

		m_hBitmap = hBitmap;

		UpdateBitmapInfo( eOrientation );
	}

	inline BOOL CDuiImage::BitBlt( HDC hDestDC, int xDest, int yDest, DWORD dwROP ) const throw()
	{
		return( BitBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, dwROP ) );
	}

	inline BOOL CDuiImage::BitBlt( HDC hDestDC, const POINT& pointDest, DWORD dwROP ) const throw()
	{
		return( BitBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight,
			0, 0, dwROP ) );
	}

	inline BOOL CDuiImage::BitBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, DWORD dwROP ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != NULL );
		assert( hDestDC != NULL );

		GetDC();

		bResult = ::BitBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC, 
			xSrc, ySrc, dwROP );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL CDuiImage::BitBlt( HDC hDestDC, const RECT& rectDest, 
		const POINT& pointSrc, DWORD dwROP ) const throw()
	{
		return( BitBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y, 
			dwROP ) );
	}

	// Remarks:
	//	如果nHeight为负值，表示要创建一个bottom-up类型的DIB，否则为一个top-down类型的
	// 
	inline BOOL CDuiImage::Create( int nWidth, int nHeight, int nBPP, DWORD dwFlags ) throw()
	{
		return( CreateEx( nWidth, nHeight, nBPP, BI_RGB, NULL, dwFlags ) );
	}

	inline BOOL CDuiImage::CreateEx( int nWidth, int nHeight, int nBPP, DWORD eCompression, 
		const DWORD* pdwBitfields, DWORD dwFlags ) throw()
	{
		//USES_ATL_SAFE_ALLOCA;
		LPBITMAPINFO pbmi;
		HBITMAP hBitmap;

		assert( (eCompression == BI_RGB) || (eCompression == BI_BITFIELDS) );
		if( dwFlags&createAlphaChannel )
		{
			assert( (nBPP == 32) && (eCompression == BI_RGB) );
		}

		pbmi = (LPBITMAPINFO)_alloca(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256);
		if( pbmi == NULL )
			return FALSE;

		memset( &pbmi->bmiHeader, 0, sizeof( pbmi->bmiHeader ) );
		pbmi->bmiHeader.biSize = sizeof( pbmi->bmiHeader );
		pbmi->bmiHeader.biWidth = nWidth;
		pbmi->bmiHeader.biHeight = nHeight;
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = USHORT( nBPP );
		pbmi->bmiHeader.biCompression = eCompression;
		if( nBPP <= 8 )
		{
			assert( eCompression == BI_RGB );
#pragma warning(push)
#pragma warning(disable:4068) //Disable unknown pragma warning that prefast pragma causes.
#pragma prefast(push)
#pragma prefast(disable:203, "no buffer overrun here, buffer was alocated properly")	
			memset( pbmi->bmiColors, 0, 256*sizeof( RGBQUAD ) );
#pragma prefast(pop)
#pragma warning(pop)
		}

		else 
		{
			if( eCompression == BI_BITFIELDS )
			{
				assert( pdwBitfields != NULL );
				/* < BEGIN 20100623 libo MODIFY，　在VS2005以下，没有memcpy_s函数 */
				/*Checked::*///memcpy_s(pbmi->bmiColors, 3*sizeof( DWORD ), pdwBitfields, 3*sizeof( DWORD ));
				memcpy( pbmi->bmiColors,  pdwBitfields, 3*sizeof( DWORD ) );
				/* 20100623 libo MODIFY END > */
			}
		}

		hBitmap = ::CreateDIBSection( NULL, pbmi, DIB_RGB_COLORS, &m_pBits, NULL, 0 );
		if (hBitmap == NULL)
		{
			return( FALSE );
		}

		Attach( hBitmap, (nHeight < 0) ? DIBOR_TOPDOWN : DIBOR_BOTTOMUP );

		if (dwFlags&createAlphaChannel)
		{
			m_bHasAlphaChannel = true;
		}

		return( TRUE );
	}

	inline void CDuiImage::Destroy() throw()
	{
		HBITMAP hBitmap;

		if( m_hBitmap != NULL )
		{
			hBitmap = Detach();
			::DeleteObject( hBitmap );
		}
		m_bHasAlphaChannel = false;
	}

	inline HBITMAP CDuiImage::Detach() throw()
	{
		HBITMAP hBitmap;

		assert( m_hBitmap != NULL );
		assert( m_hDC == NULL );

		hBitmap = m_hBitmap;
		m_hBitmap = NULL;
		m_pBits = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBPP = 0;
		m_nPitch = 0;
		m_iTransparentColor = -1;
		m_bHasAlphaChannel = false;
		m_bIsDIBSection = false;

		return( hBitmap );
	}

	inline BOOL CDuiImage::Draw( HDC hDestDC, const RECT& rectDest, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, 
			m_nHeight, bAlpha ) );
	}

	inline BOOL CDuiImage::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, m_nWidth, m_nHeight, bAlpha ) );
	}

	inline BOOL CDuiImage::Draw( HDC hDestDC, const POINT& pointDest, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, bAlpha ) );
	}

	inline BOOL CDuiImage::Draw( HDC hDestDC, int xDest, int yDest, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, bAlpha ) );
	}

	inline BOOL CDuiImage::Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
			rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, bAlpha ) );
	}

	inline BOOL CDuiImage::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth,
		int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, byte bAlpha ) const throw()
	{
		BOOL bResult;

		if( NULL == m_hBitmap )
			return FALSE;

		if( nDestWidth <= 0 )	return TRUE;
		if( nDestHeight <= 0 )	return TRUE;
		if( nSrcWidth <= 0 )	return TRUE;
		if( nSrcHeight <= 0 )	return TRUE;

		assert( m_hBitmap != NULL );
		assert( hDestDC != NULL );

		GetDC();

#if WINVER >= 0x0500
		if( (m_iTransparentColor != -1) && IsTransparencySupported() )
		{
			bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
				m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, GetTransparentRGB() );
		}
		else if( m_bHasAlphaChannel && IsTransparencySupported() )
		{
			BLENDFUNCTION bf;

			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = bAlpha;
			bf.AlphaFormat = AC_SRC_ALPHA;

			bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 
				m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, bf );
		}
		else
#endif  // WINVER >= 0x0500
		{
			bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 
				m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY );
		}

		ReleaseDC();

		return( bResult );
	}

	/* <BEGIN bo.li ADD 20101202 增加9宫绘制 */
	inline BOOL CDuiImage::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
		Image9Region* pImage9Region, bool bDrawCenter, byte bAlpha ) const throw()
	{
		if (NULL == pImage9Region)
			return this->Draw(hDestDC, xDest, yDest, nDestWidth, nDestHeight,
				xSrc, ySrc, nSrcWidth, nSrcHeight, bAlpha);

		// 1. topleft

		this->Draw( hDestDC, 

			xDest,
			yDest,
			pImage9Region->topleft,
			pImage9Region->top,
			
			xSrc, 
			ySrc,
			pImage9Region->topleft,
			pImage9Region->top 
			, bAlpha
			);

		// 2. top

		this->Draw( hDestDC, 

			xDest + pImage9Region->topleft,                                     // xDest
			yDest,                                                              // yDest
			nDestWidth - pImage9Region->topleft - pImage9Region->topright ,     // nDestWidth
			pImage9Region->top,                                                 // nDestHeight

			xSrc + pImage9Region->topleft,                                         // xSrc
			ySrc,                                                               // ySrc
			nSrcWidth - pImage9Region->topleft - pImage9Region->topright,       // nSrcWidth
			pImage9Region->top                                                  // nSrcHeight
			, bAlpha
			);                                                 

		// 3. topright

		this->Draw( hDestDC, 
			
			xDest + nDestWidth - pImage9Region->topright,
			yDest,
			pImage9Region->topright,
			pImage9Region->top, 
			
			xSrc + nSrcWidth - pImage9Region->topright,
			ySrc, 
			pImage9Region->topright,
			pImage9Region->top
			, bAlpha
			);

		// 4. left

		this->Draw( hDestDC, 

			xDest,                                                        // xDest
			yDest + pImage9Region->top,                                          // yDest
			pImage9Region->left,                                         // nDestWidth
			nDestHeight - pImage9Region->top- pImage9Region->bottom,               // nDestHeight

			xSrc ,                                                        // xSrc
			ySrc + pImage9Region->top,                                          // ySrc
			pImage9Region->left,                                         // nSrcWidth
			nSrcHeight - pImage9Region->top - pImage9Region->bottom      // nSrcHeight
			, bAlpha
			);

		// 5. center
		if (bDrawCenter)
		{
			this->Draw( hDestDC, 

				xDest + pImage9Region->left,                                         // xDest
				yDest + pImage9Region->top,                                          // yDest
				nDestWidth  - pImage9Region->left - pImage9Region->right,                // nDestWidth
				nDestHeight - pImage9Region->top  - pImage9Region->bottom,               // nDestHeight

				xSrc + pImage9Region->left,                                         // xSrc
				ySrc + pImage9Region->top,                                          // ySrc
				nSrcWidth  - pImage9Region->left - pImage9Region->right,           // nSrcWidth
				nSrcHeight - pImage9Region->top  - pImage9Region->bottom        // nSrcHeight
				, bAlpha
				); 
		}
		// 6. right

		this->Draw( hDestDC, 

			xDest + nDestWidth - pImage9Region->right,                                 // xDest
			yDest + pImage9Region->top,                                          // yDest
			pImage9Region->right,                                        // nDestWidth
			nDestHeight - pImage9Region->top - pImage9Region->bottom,               // nDestHeight

			xSrc + nSrcWidth - pImage9Region->right,                            // xSrc
			ySrc + pImage9Region->top,                                          // ySrc
			pImage9Region->right,                                        // nSrcWidth
			nSrcHeight - pImage9Region->top - pImage9Region->bottom        // nSrcHeight
			, bAlpha
			);

		// 7. bottomleft

		this->Draw( hDestDC,
			
			xDest,
			yDest + nDestHeight - pImage9Region->bottom,
			pImage9Region->bottomleft, 
			pImage9Region->bottom, 
			
			xSrc, 
			ySrc + nSrcHeight - pImage9Region->bottom,
			pImage9Region->bottomleft, 
			pImage9Region->bottom
			, bAlpha
			);

		// 8. bottom

		this->Draw( hDestDC,

			xDest + pImage9Region->bottomleft,                                      // xDest
			yDest + nDestHeight - pImage9Region->bottom,                               // yDest
			nDestWidth - pImage9Region->bottomleft - pImage9Region->bottomright,// nDestWidth
			pImage9Region->bottom,                                       // nDestHeight

			xSrc + pImage9Region->bottomleft,                                   // xSrc
			ySrc + nSrcHeight - pImage9Region->bottom,                          // ySrc
			nSrcWidth - pImage9Region->bottomleft - pImage9Region->bottomright, // nSrcWidth
			pImage9Region->bottom                                         // nSrcHeight
			, bAlpha
			);

		// 9. bottomright
		this->Draw( hDestDC,

			xDest + nDestWidth - pImage9Region->bottomright,
			yDest + nDestHeight - pImage9Region->bottom,
			pImage9Region->bottomright,
			pImage9Region->bottom,
			
			xSrc + nSrcWidth  - pImage9Region->bottomright,
			ySrc + nSrcHeight - pImage9Region->bottom, 
			pImage9Region->bottomright,
			pImage9Region->bottom
			, bAlpha
			);

		return true;
	}
	/* <BEGIN bo.li ADD 20101202 增加9宫绘制 */

	inline const void* CDuiImage::GetBits() const throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );

		return( m_pBits );
	}

	inline void* CDuiImage::GetBits() throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );

		return( m_pBits );
	}

	inline int CDuiImage::GetBPP() const throw()
	{
		assert( m_hBitmap != NULL );

		return( m_nBPP );
	}

	inline void CDuiImage::GetColorTable( UINT iFirstColor, UINT nColors, 
		RGBQUAD* prgbColors ) const throw()
	{
		assert( m_hBitmap != NULL );
		assert( m_pBits != NULL );
		assert( IsIndexed() );

		GetDC();

		::GetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

		ReleaseDC();
	}

	inline HDC CDuiImage::GetDC() const throw()
	{
		assert( m_hBitmap != NULL );

		m_nDCRefCount++;
		if( m_hDC == NULL )
		{
			m_hDC = s_cache.GetDC();
			m_hOldBitmap = HBITMAP( ::SelectObject( m_hDC, m_hBitmap ) );
		}

		return( m_hDC );
	}

	inline bool CDuiImage::ShouldExcludeFormat( REFGUID guidFileType, DWORD dwExclude ) throw()
	{
		static const GUID* apguidFormats[] =
		{
			&Gdiplus::ImageFormatGIF,
			&Gdiplus::ImageFormatBMP,
			&Gdiplus::ImageFormatEMF,
			&Gdiplus::ImageFormatWMF,
			&Gdiplus::ImageFormatJPEG,
			&Gdiplus::ImageFormatPNG,
			&Gdiplus::ImageFormatTIFF,
			&Gdiplus::ImageFormatIcon,
			NULL
		};

		assert( (dwExclude|excludeValid) == excludeValid );
		for( int iFormat = 0; apguidFormats[iFormat] != NULL; iFormat++ )
		{
			if( guidFileType == *apguidFormats[iFormat] )
			{
				return( (dwExclude&(1<<iFormat)) != 0 );
			}
		}

		return( (dwExclude&excludeOther) != 0 );
	}

	inline void CDuiImage::BuildCodecFilterString( const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs,
		CDuiString& strFilter, vector< GUID >& aguidFileTypes, LPCTSTR pszAllFilesDescription, 
		DWORD dwExclude, TCHAR chSeparator )
	{
#ifdef _UNICODE // libo 20101117 remove 使用于多字节编码
		if( pszAllFilesDescription != NULL )
		{
			aguidFileTypes.push_back( GUID_NULL );
		}

		CDuiString strAllExtensions;
		CDuiString strTempFilter;
		for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
		{
			const Gdiplus::ImageCodecInfo* pCodec = &pCodecs[iCodec];

			if( !ShouldExcludeFormat( pCodec->FormatID, dwExclude ) )
			{
				LPCWSTR pwszFilenameExtension( pCodec->FilenameExtension );
				strTempFilter +=  pCodec->FormatDescription ;
				strTempFilter += _T( " (" );
				strTempFilter += pwszFilenameExtension;
				strTempFilter += _T( ")" );
				strTempFilter += chSeparator;
				strTempFilter += pwszFilenameExtension;
				strTempFilter += chSeparator;

				aguidFileTypes.push_back( pCodec->FormatID );

				if( strAllExtensions != _T("") )
				{
					strAllExtensions += _T( ";" );
				}
				strAllExtensions += pwszFilenameExtension;
			}
		}

		if( pszAllFilesDescription != NULL )
		{
			strFilter += pszAllFilesDescription;
			strFilter += chSeparator;
			strFilter += strAllExtensions;
			strFilter += chSeparator;
		}
		strFilter += strTempFilter;

		strFilter += chSeparator;
		if( aguidFileTypes.size() == 0 )
		{
			strFilter += chSeparator;
		}
#endif
	}

	inline HRESULT CDuiImage::GetImporterFilterString( CDuiString& strImporters, 
		vector< GUID >& aguidFileTypes, LPCTSTR pszAllFilesDescription /* = NULL */,
		DWORD dwExclude /* = excludeDefaultLoad */, TCHAR chSeparator /* = '|' */ )
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		UINT nCodecs;
		UINT nSize;
		Gdiplus::Status status;
		Gdiplus::ImageCodecInfo* pCodecs;

		status = Gdiplus::GetImageDecodersSize( &nCodecs, &nSize );
		//USES_ATL_SAFE_ALLOCA;
		pCodecs = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nSize) );

		if( pCodecs == NULL )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageDecoders( nCodecs, nSize, pCodecs );
		BuildCodecFilterString( pCodecs, nCodecs, strImporters, aguidFileTypes, pszAllFilesDescription, dwExclude, chSeparator );

		return( S_OK );
	}

	inline HRESULT CDuiImage::GetExporterFilterString( CDuiString& strExporters, 
		vector< GUID >& aguidFileTypes, LPCTSTR pszAllFilesDescription /* = NULL */,
		DWORD dwExclude /* = excludeDefaultSave */, TCHAR chSeparator /* = '|' */ )
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		UINT nCodecs;
		UINT nSize;
		Gdiplus::Status status;
		Gdiplus::ImageCodecInfo* pCodecs;

		status = Gdiplus::GetImageDecodersSize( &nCodecs, &nSize );
		//USES_ATL_SAFE_ALLOCA;
		pCodecs = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nSize) );

		if( pCodecs == NULL )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageDecoders( nCodecs, nSize, pCodecs );
		BuildCodecFilterString( pCodecs, nCodecs, strExporters, aguidFileTypes, pszAllFilesDescription, dwExclude, chSeparator );

		return( S_OK );
	}

	inline int CDuiImage::GetHeight() const throw()
	{
		assert( m_hBitmap != NULL );

		return( m_nHeight );
	}

	inline int CDuiImage::GetMaxColorTableEntries() const throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );

		if( IsIndexed() )
		{
			return( 1<<m_nBPP );
		}
		else
		{
			return( 0 );
		}
	}

	inline int CDuiImage::GetPitch() const throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );

		return( m_nPitch );
	}

	inline COLORREF CDuiImage::GetPixel( int x, int y ) const throw()
	{
		assert( m_hBitmap != NULL );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		GetDC();

		COLORREF clr = ::GetPixel( m_hDC, x, y );

		ReleaseDC();

		return( clr );
	}

	inline const void* CDuiImage::GetPixelAddress( int x, int y ) const throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
	}

	inline void* CDuiImage::GetPixelAddress( int x, int y ) throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
	}

	inline LONG CDuiImage::GetTransparentColor() const throw()
	{
		assert( m_hBitmap != NULL );
		assert( (m_nBPP == 4) || (m_nBPP == 8) );

		return( m_iTransparentColor );
	}

	inline int CDuiImage::GetWidth() const throw()
	{
		assert( m_hBitmap != NULL );

		return( m_nWidth );
	}

	inline bool CDuiImage::IsDIBSection() const throw()
	{
		return( m_bIsDIBSection );
	}

	inline bool CDuiImage::IsIndexed() const throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );

		return( m_nBPP <= 8 );
	}

	inline bool CDuiImage::IsNull() const throw()
	{
		return( m_hBitmap == NULL );
	}

	inline HRESULT CDuiImage::Load( IStream* pStream ) throw()
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		Gdiplus::Bitmap bmSrc( pStream );
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		return( CreateFromGdiplusBitmap( bmSrc ) );
	}

	inline HRESULT CDuiImage::Load( LPCTSTR pszFileName ) throw()
	{
#ifdef _UNICODE // libo 20101117 remove 只适用用于多字节编码
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		Gdiplus::Bitmap bmSrc( pszFileName );
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		return( CreateFromGdiplusBitmap( bmSrc ) );
#else
		return -1;
#endif
	}

	// 2012.11.28 增加强制创建一个32位带alpha通道的功能，使得24位格式图片也能在分层窗口上显示
	inline HRESULT CDuiImage::CreateFromGdiplusBitmap(Gdiplus::Bitmap& bmSrc) throw()
	{
		UINT  nBPP = 32;
		DWORD dwFlags = 0;

		Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
		Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppRGB;
		if( eSrcPixelFormat&PixelFormatGDI )
		{
			nBPP = Gdiplus::GetPixelFormatSize( eSrcPixelFormat );
			eDestPixelFormat = eSrcPixelFormat;
		}
		if( m_bHasAlphaChannel || Gdiplus::IsAlphaPixelFormat( eSrcPixelFormat ) )  // Add by libo 如果需要强制创建一个带alpha channel的位图
		{
			nBPP = 32;
			dwFlags |= createAlphaChannel;
			eDestPixelFormat = PixelFormat32bppARGB;
		}

		BOOL bSuccess = Create( bmSrc.GetWidth(), bmSrc.GetHeight(), nBPP, dwFlags );
		if( !bSuccess )
		{
			return( E_FAIL );
		}
		//USES_ATL_SAFE_ALLOCA;
		Gdiplus::ColorPalette* pPalette = NULL;
		if (false == m_bHasAlphaChannel && Gdiplus::IsIndexedPixelFormat(eSrcPixelFormat))  // 有可能外部强制指定了创建alpha通道的32位图片，这时候没有color table
		{
			UINT nPaletteSize = bmSrc.GetPaletteSize();
			pPalette = static_cast< Gdiplus::ColorPalette* >( _alloca(nPaletteSize) );

			if( pPalette == NULL )
				return E_OUTOFMEMORY;

			bmSrc.GetPalette( pPalette, nPaletteSize );

			RGBQUAD argbPalette[256];
			assert( (pPalette->Count > 0) && (pPalette->Count <= 256) );
			for( UINT iColor = 0; iColor < pPalette->Count; iColor++ )
			{
				Gdiplus::ARGB color = pPalette->Entries[iColor];
				argbPalette[iColor].rgbRed = (BYTE)( (color>>RED_SHIFT) & 0xff );
				argbPalette[iColor].rgbGreen = (BYTE)( (color>>GREEN_SHIFT) & 0xff );
				argbPalette[iColor].rgbBlue = (BYTE)( (color>>BLUE_SHIFT) & 0xff );
				argbPalette[iColor].rgbReserved = 0;
			}

			SetColorTable( 0, pPalette->Count, argbPalette );
		}

		if (eDestPixelFormat == eSrcPixelFormat)
		{
			// The pixel formats are identical, so just memcpy the rows.
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect( 0, 0, GetWidth(), GetHeight() );
			if(bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data )!=Gdiplus::Ok)
			{
				return E_OUTOFMEMORY;
			}

			size_t nBytesPerRow = ( (nBPP*GetWidth()+8-1)&~(8 -1) )/ 8;//AtlAlignUp( nBPP*GetWidth(), 8 )/8;
			
			BYTE* pbDestRow = static_cast< BYTE* >( GetBits() );
			BYTE* pbSrcRow = static_cast< BYTE* >( data.Scan0 );
	
			for( int y = 0; y < GetHeight(); y++ )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//                         实现预乘。
				//
				// First you need to create a pre-multiplied 32-bits-per-pixel (bpp) bitmap using a
				// blue-green-red-alpha (BGRA) color channel byte order. Pre-multiplied just means
				// that the color channel values have already been multiplied by the alpha value.
				// This tends to provide better performance for alpha blending images, but it means
				// you need to reverse the process by dividing the color values by the alpha value
				// to get their true color values.
				//
				//////////////////////////////////////////////////////////////////////////
				if (m_bHasAlphaChannel)
				{
					for (int i = 0; i < (int)nBytesPerRow; i+=4)
					{
						int nAlpha = pbSrcRow[i+3];
						pbDestRow[i]   = pbSrcRow[i]  *nAlpha/255;
						pbDestRow[i+1] = pbSrcRow[i+1]*nAlpha/255;
						pbDestRow[i+2] = pbSrcRow[i+2]*nAlpha/255;
						pbDestRow[i+3] = nAlpha;
					}
				}
				else 
				{
					/* < BEGIN 20100623 libo MODIFY，　在VS2005以下，没有memcpy_s函数 */
					//memcpy_s(pbDestRow, nBytesPerRow, pbSrcRow, nBytesPerRow);
					memcpy( pbDestRow,  pbSrcRow, nBytesPerRow );
					/* 20100623 libo MODIFY END > */
				}
				pbDestRow += GetPitch();
				pbSrcRow += data.Stride;
			}

			bmSrc.UnlockBits( &data );
		}
		else
		{
			// Let GDI+ work its magic
			Gdiplus::Bitmap bmDest( GetWidth(), GetHeight(), GetPitch(), eDestPixelFormat, static_cast< BYTE* >( GetBits() ) );
			Gdiplus::Graphics gDest( &bmDest );
			gDest.DrawImage( &bmSrc, 0,0);
		}

		return( S_OK );
	}


	// 指定创建32位格式的Image。为了避免24位的图片有没有alpha值，导致在分层窗口上显示不出来，在这里将alpha填充为255
	inline HRESULT CDuiImage::Create32BPPFromGdiplusBitmap( Gdiplus::Bitmap& bmSrc ) throw()
	{
		Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
		UINT nBPP = 32;
		DWORD dwFlags = 0;
		Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppRGB;
		if( eSrcPixelFormat&PixelFormatGDI )
		{
			nBPP = Gdiplus::GetPixelFormatSize( eSrcPixelFormat );
			eDestPixelFormat = eSrcPixelFormat;
		}
		if( Gdiplus::IsAlphaPixelFormat( eSrcPixelFormat ) )
		{
			nBPP = 32;
			dwFlags |= createAlphaChannel;
			eDestPixelFormat = PixelFormat32bppARGB;
		}

		BOOL bSuccess = Create( bmSrc.GetWidth(), bmSrc.GetHeight(), nBPP, dwFlags );
		if( !bSuccess )
		{
			return( E_FAIL );
		}
		//USES_ATL_SAFE_ALLOCA;
		Gdiplus::ColorPalette* pPalette = NULL;
		if( Gdiplus::IsIndexedPixelFormat( eSrcPixelFormat ) )
		{
			UINT nPaletteSize = bmSrc.GetPaletteSize();
			pPalette = static_cast< Gdiplus::ColorPalette* >( _alloca(nPaletteSize) );

			if( pPalette == NULL )
				return E_OUTOFMEMORY;

			bmSrc.GetPalette( pPalette, nPaletteSize );

			RGBQUAD argbPalette[256];
			assert( (pPalette->Count > 0) && (pPalette->Count <= 256) );
			for( UINT iColor = 0; iColor < pPalette->Count; iColor++ )
			{
				Gdiplus::ARGB color = pPalette->Entries[iColor];
				argbPalette[iColor].rgbRed = (BYTE)( (color>>RED_SHIFT) & 0xff );
				argbPalette[iColor].rgbGreen = (BYTE)( (color>>GREEN_SHIFT) & 0xff );
				argbPalette[iColor].rgbBlue = (BYTE)( (color>>BLUE_SHIFT) & 0xff );
				argbPalette[iColor].rgbReserved = 0;
			}

			SetColorTable( 0, pPalette->Count, argbPalette );
		}

		if( eDestPixelFormat == eSrcPixelFormat  )
		{
			// The pixel formats are identical, so just memcpy the rows.
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect( 0, 0, GetWidth(), GetHeight() );
			if(bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data )!=Gdiplus::Ok)
			{
				return E_OUTOFMEMORY;
			}

			size_t nBytesPerRow = ( (nBPP*GetWidth()+8-1)&~(8 -1) )/ 8;//AtlAlignUp( nBPP*GetWidth(), 8 )/8;

			BYTE* pbDestRow = static_cast< BYTE* >( GetBits() );
			BYTE* pbSrcRow = static_cast< BYTE* >( data.Scan0 );

			for( int y = 0; y < GetHeight(); y++ )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//                         实现预乘。
				//
				// First you need to create a pre-multiplied 32-bits-per-pixel (bpp) bitmap using a
				// blue-green-red-alpha (BGRA) color channel byte order. Pre-multiplied just means
				// that the color channel values have already been multiplied by the alpha value.
				// This tends to provide better performance for alpha blending images, but it means
				// you need to reverse the process by dividing the color values by the alpha value
				// to get their true color values.
				//
				//////////////////////////////////////////////////////////////////////////
				if (m_bHasAlphaChannel)
				{
					for (int i = 0; i < (int)nBytesPerRow; i+=4)
					{
						int nAlpha = pbSrcRow[i+3];
						pbDestRow[i]   = pbSrcRow[i]  *nAlpha/255;
						pbDestRow[i+1] = pbSrcRow[i+1]*nAlpha/255;
						pbDestRow[i+2] = pbSrcRow[i+2]*nAlpha/255;
						pbDestRow[i+3] = nAlpha;
					}
				}
				else 
				{
					/* < BEGIN 20100623 libo MODIFY，　在VS2005以下，没有memcpy_s函数 */
					//memcpy_s(pbDestRow, nBytesPerRow, pbSrcRow, nBytesPerRow);
					memcpy( pbDestRow,  pbSrcRow, nBytesPerRow );
					/* 20100623 libo MODIFY END > */
				}
				pbDestRow += GetPitch();
				pbSrcRow += data.Stride;
			}

			bmSrc.UnlockBits( &data );
		}
		else
		{
			// Let GDI+ work its magic
			Gdiplus::Bitmap bmDest( GetWidth(), GetHeight(), GetPitch(), eDestPixelFormat, static_cast< BYTE* >( GetBits() ) );
			Gdiplus::Graphics gDest( &bmDest );

			gDest.DrawImage( &bmSrc, 0, 0 );
		}

		return( S_OK );
	}

	/* <BEGIN  ADD libo 20101116 添加从资源中加载png图片的方法 */
	inline void CDuiImage::LoadFromResource( HINSTANCE hInstance, UINT nIDResource, TCHAR* szResourceType ) throw()
	{
		// 如果没有指定资源类型，那么可能就是默认的bmp图片
		if( NULL == szResourceType )
		{
			this->LoadFromResource( hInstance, MAKEINTRESOURCE(nIDResource) );
			return;
		}

		// 查找资源
		HRSRC hRsrc = ::FindResource( hInstance, MAKEINTRESOURCE(nIDResource), szResourceType);
		if (hRsrc == NULL) 
			return;

		// 加载资源
		HGLOBAL hImgData = ::LoadResource( hInstance, hRsrc);
		if (hImgData == NULL)
			return;

		// 锁定内存中的指定资源
		LPVOID   lpVoid  = ::LockResource(hImgData);
		DWORD    dwSize  = ::SizeofResource(hInstance, hRsrc);

		this->LoadFromData(lpVoid,dwSize);

		// 释放资源
		::FreeResource(hImgData);
	}

	//
	//	从文件数据中加载图片
	//		pImageData
	//			[in]	文件流数据
	//		nSize
	//			[in]	pImageData大小
	//
	inline bool CDuiImage::LoadFromData( void* pImageData, int nSize  )
	{
		LPSTREAM pStream = NULL;
		HGLOBAL  hNew    = ::GlobalAlloc(GHND, nSize);

		LPBYTE lpByte    = (LPBYTE)::GlobalLock(hNew);
		::memcpy(lpByte, pImageData, nSize);
		::GlobalUnlock(hNew);

		// 从指定内存创建流对象
		HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
		if ( ht != S_OK )
		{
			GlobalFree(hNew);
			return false;
		}
		else
		{
			// 加载图片
			HRESULT hr = this->Load(pStream);
			GlobalFree(hNew);
			if( FAILED(hr) )
				return false;
		}
		return true;;
	}
	/* ADD END> */

	inline void CDuiImage::LoadFromResource( HINSTANCE hInstance, LPCTSTR pszResourceName ) throw()
	{
		HBITMAP hBitmap;

		hBitmap = HBITMAP( ::LoadImage( hInstance, pszResourceName, IMAGE_BITMAP, 0, 
			0, LR_CREATEDIBSECTION ) );

		Attach( hBitmap );
	}

	inline void CDuiImage::LoadFromResource( HINSTANCE hInstance, UINT nIDResource ) throw()
	{
		LoadFromResource( hInstance, MAKEINTRESOURCE( nIDResource ) );
	}

	inline BOOL CDuiImage::MaskBlt( HDC hDestDC, int xDest, int yDest, int nWidth, 
		int nHeight, int xSrc, int ySrc, HBITMAP hbmMask, int xMask, int yMask,
		DWORD dwROP ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != NULL );
		assert( hDestDC != NULL );

		GetDC();

		bResult = ::MaskBlt( hDestDC, xDest, yDest, nWidth, nHeight, m_hDC, xSrc, 
			ySrc, hbmMask, xMask, yMask, dwROP );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL CDuiImage::MaskBlt( HDC hDestDC, const RECT& rectDest, 
		const POINT& pointSrc, HBITMAP hbmMask, const POINT& pointMask, 
		DWORD dwROP ) const throw()
	{
		return( MaskBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y, 
			hbmMask, pointMask.x, pointMask.y, dwROP ) );
	}

	inline BOOL CDuiImage::MaskBlt( HDC hDestDC, int xDest, int yDest, HBITMAP hbmMask, 
		DWORD dwROP ) const throw()
	{
		return( MaskBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, hbmMask, 
			0, 0, dwROP ) );
	}

	inline BOOL CDuiImage::MaskBlt( HDC hDestDC, const POINT& pointDest, HBITMAP hbmMask,
		DWORD dwROP ) const throw()
	{
		return( MaskBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 
			0, hbmMask, 0, 0, dwROP ) );
	}

	inline BOOL CDuiImage::PlgBlt( HDC hDestDC, const POINT* pPoints, int xSrc, 
		int ySrc, int nSrcWidth, int nSrcHeight, HBITMAP hbmMask, int xMask, 
		int yMask ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != NULL );
		assert( hDestDC != NULL );

		GetDC();

		bResult = ::PlgBlt( hDestDC, pPoints, m_hDC, xSrc, ySrc, nSrcWidth, 
			nSrcHeight, hbmMask, xMask, yMask );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL CDuiImage::PlgBlt( HDC hDestDC, const POINT* pPoints, 
		const RECT& rectSrc, const POINT& pointMask, HBITMAP hbmMask  ) const throw()
	{
		return( PlgBlt( hDestDC, pPoints, rectSrc.left, rectSrc.top, rectSrc.right-
			rectSrc.left, rectSrc.bottom-rectSrc.top, hbmMask, pointMask.x, 
			pointMask.y ) );
	}

	inline BOOL CDuiImage::PlgBlt( HDC hDestDC, const POINT* pPoints, 
		HBITMAP hbmMask ) const throw()
	{
		return( PlgBlt( hDestDC, pPoints, 0, 0, m_nWidth, m_nHeight, hbmMask, 0, 
			0 ) );
	}

	inline void CDuiImage::ReleaseDC() const throw()
	{
		HBITMAP hBitmap;

		assert( m_hDC != NULL );

		m_nDCRefCount--;
		if( m_nDCRefCount == 0 )
		{
			hBitmap = HBITMAP( ::SelectObject( m_hDC, m_hOldBitmap ) );
			assert( hBitmap == m_hBitmap );
			s_cache.ReleaseDC( m_hDC );
			m_hDC = NULL;
		}
	}

	inline CLSID CDuiImage::FindCodecForExtension( LPCTSTR pszExtension, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs )
	{
#ifdef _UNICODE // libo 20101117 remove 只适用用于多字节编码
		LPCWSTR pszExtensionW( pszExtension );

		for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
		{
			CDuiString strExtensions( pCodecs[iCodec].FilenameExtension );

			int iStart = 0;
			do
			{
				CDuiString strExtension;
				TCHAR* pszPlace = (TCHAR*)(strExtensions.GetData()+iStart);
				TCHAR* pszEnd = (TCHAR*)(strExtensions.GetData()+strExtensions.GetLength());
				if( pszPlace < pszEnd )
				{
					int nIncluding = (int)wcsspn( pszPlace,	L";" );

					if( (pszPlace+nIncluding) < pszEnd )
					{
						pszPlace += nIncluding;
						int nExcluding = (int)wcsspn( pszPlace, L";" );

						int iFrom = iStart+nIncluding;
						int nUntil = nExcluding;
						iStart = iFrom+nUntil+1;

						strExtension = ( strExtensions.Mid( iFrom, nUntil-iFrom ) );
					}
				}
				else
					iStart = -1;

				//String strExtension = ::PathFindExtensionW( strExtensions.Tokenize( L";", iStart ) );
				if( iStart != -1 )
				{
					if( strExtension.Compare( pszExtensionW ) == 0 )
					{
						return( pCodecs[iCodec].Clsid );
					}
				}
			} while( iStart != -1 );
		}
#endif
		return( CLSID_NULL );

	}

	inline CLSID CDuiImage::FindCodecForFileType( REFGUID guidFileType, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs )
	{
		for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
		{
			if( pCodecs[iCodec].FormatID == guidFileType )
			{
				return( pCodecs[iCodec].Clsid );
			}
		}

		return( CLSID_NULL );
	}

	inline HRESULT CDuiImage::Save( IStream* pStream, REFGUID guidFileType ) const throw()
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		UINT nEncoders;
		UINT nBytes;
		Gdiplus::Status status;

		status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		//USES_ATL_SAFE_ALLOCA;
		Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nBytes) );

		if( pEncoders == NULL )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		CLSID clsidEncoder = FindCodecForFileType( guidFileType, pEncoders, nEncoders );
		if( clsidEncoder == CLSID_NULL )
		{
			return( E_FAIL );
		}

		if( m_bHasAlphaChannel )
		{
			assert( m_nBPP == 32 );
			Gdiplus::Bitmap bm( m_nWidth, m_nHeight, m_nPitch, PixelFormat32bppARGB, static_cast< BYTE* >( m_pBits ) );
			status = bm.Save( pStream, &clsidEncoder, NULL );
			if( status != Gdiplus::Ok )
			{
				return( E_FAIL );
			}
		}
		else
		{
			Gdiplus::Bitmap bm( m_hBitmap, NULL );
			status = bm.Save( pStream, &clsidEncoder, NULL );
			if( status != Gdiplus::Ok )
			{
				return( E_FAIL );
			}
		}

		return( S_OK );
	}

	inline HRESULT CDuiImage::Save( LPCTSTR pszFileName, REFGUID guidFileType ) const throw()
	{
#ifdef _UNICODE // libo 20101117 remove 只适用用于多字节编码
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		UINT nEncoders;
		UINT nBytes;
		Gdiplus::Status status;

		status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		//USES_CONVERSION_EX;
		Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nBytes) );

		if( pEncoders == NULL )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		CLSID clsidEncoder = CLSID_NULL;
		if( guidFileType == GUID_NULL )
		{
			// Determine clsid from extension
			clsidEncoder = FindCodecForExtension( ::PathFindExtension( pszFileName ), pEncoders, nEncoders );
		}
		else
		{
			// Determine clsid from file type
			clsidEncoder = FindCodecForFileType( guidFileType, pEncoders, nEncoders );
		}
		if( clsidEncoder == CLSID_NULL )
		{
			return( E_FAIL );
		}

		LPCWSTR pwszFileName =  pszFileName;
#ifndef _UNICODE
		if( pwszFileName == NULL )
			return E_OUTOFMEMORY;
#endif // _UNICODE
		if (m_bHasAlphaChannel)
		{
			assert(m_nBPP == 32);
			// libo modified 20130121 PixelFormat32bppARGB->PixelFormat32bppPARGB
			// 使用PixelFormat32bppARGB保存时，alpha值不知道为什么总是和原始值不一样。
			Gdiplus::Bitmap bm( m_nWidth, m_nHeight, m_nPitch, PixelFormat32bppPARGB, static_cast< BYTE* >( m_pBits ) );
			status = bm.Save( pwszFileName, &clsidEncoder, NULL );
			if (status != Gdiplus::Ok)
			{
				return( E_FAIL );
			}
		}
		else
		{
			Gdiplus::Bitmap bm( m_hBitmap, NULL );
			status = bm.Save( pwszFileName, &clsidEncoder, NULL );
			if (status != Gdiplus::Ok)
			{
				return( E_FAIL );
			}
		}

#endif
		return( S_OK );
	}

	inline void CDuiImage::SetColorTable( UINT iFirstColor, UINT nColors, 
		const RGBQUAD* prgbColors ) throw()
	{
		assert( m_hBitmap != NULL );
		assert( IsDIBSection() );
		assert( IsIndexed() );

		GetDC();

		::SetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

		ReleaseDC();
	}

	inline void CDuiImage::SetPixel( int x, int y, COLORREF color ) throw()
	{
		assert( m_hBitmap != NULL );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		GetDC();

		::SetPixel( m_hDC, x, y, color );

		ReleaseDC();
	}

	inline void CDuiImage::SetPixelIndexed( int x, int y, int iIndex ) throw()
	{
		SetPixel( x, y, PALETTEINDEX( iIndex ) );
	}

	inline void CDuiImage::SetPixelRGB( int x, int y, BYTE r, BYTE g, BYTE b ) throw()
	{
		SetPixel( x, y, RGB( r, g, b ) );
	}

	inline LONG CDuiImage::SetTransparentColor( LONG iTransparentColor ) throw()
	{
		LONG iOldTransparentColor;

		assert( m_hBitmap != NULL );
		assert( (m_nBPP == 4) || (m_nBPP == 8) );
		assert( iTransparentColor < GetMaxColorTableEntries() );
		assert( iTransparentColor >= -1 );

		iOldTransparentColor = m_iTransparentColor;
		m_iTransparentColor = iTransparentColor;

		return( iOldTransparentColor );
	}

	inline COLORREF CDuiImage::SetTransparentColor( COLORREF colorTransparent ) throw()
	{
		COLORREF oldValue = m_colorTransparent;

		m_iTransparentColor = CLR_USE_COLOR_DIRECT;
		m_colorTransparent = colorTransparent;	

		return oldValue;
	}

	inline BOOL CDuiImage::StretchBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, DWORD dwROP ) const throw()
	{
		return( StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, 
			m_nWidth, m_nHeight, dwROP ) );
	}

	inline BOOL CDuiImage::StretchBlt( HDC hDestDC, const RECT& rectDest, 
		DWORD dwROP ) const throw()
	{
		return( StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, m_nHeight, 
			dwROP ) );
	}

	inline BOOL CDuiImage::StretchBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
		int nSrcHeight, DWORD dwROP ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != NULL );
		assert( hDestDC != NULL );

		GetDC();

		bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC,
			xSrc, ySrc, nSrcWidth, nSrcHeight, dwROP );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL CDuiImage::StretchBlt( HDC hDestDC, const RECT& rectDest, 
		const RECT& rectSrc, DWORD dwROP ) const throw()
	{
		return( StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
			rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, dwROP ) );
	}

#if WINVER >= 0x0500
	inline BOOL CDuiImage::TransparentBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, UINT crTransparent ) const throw()
	{
		return( TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 
			0, m_nWidth, m_nHeight, crTransparent ) );
	}

	inline BOOL CDuiImage::TransparentBlt( HDC hDestDC, const RECT& rectDest, 
		UINT crTransparent ) const throw()
	{
		return( TransparentBlt( hDestDC, rectDest.left, rectDest.top, 
			rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, 
			crTransparent ) );
	}

	inline BOOL CDuiImage::TransparentBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
		int nSrcHeight, UINT crTransparent ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != NULL );
		assert( hDestDC != NULL );

		GetDC();

		if( crTransparent == CLR_INVALID )
		{
			crTransparent = GetTransparentRGB();
		}
		else if( crTransparent == CLR_USE_COLOR_DIRECT )
		{
			crTransparent = m_colorTransparent;
		}

		bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL CDuiImage::TransparentBlt( HDC hDestDC, const RECT& rectDest, 
		const RECT& rectSrc, UINT crTransparent ) const throw()
	{
		return( TransparentBlt( hDestDC, rectDest.left, rectDest.top, 
			rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, 
			rectSrc.top, rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, 
			crTransparent ) );
	}
#endif  // WINVER >= 0x0500

	inline BOOL CDuiImage::IsTransparencySupported() throw()
	{
#if WINVER >= 0x0500
		//return( _AtlBaseModule.m_bNT5orWin98 );
		return TRUE;
#else  // WINVER < 0x0500
		return( FALSE );
#endif  // WINVER >= 0x0500
	}

	inline void CDuiImage::UpdateBitmapInfo( DIBOrientation eOrientation )
	{
		DIBSECTION dibsection;
		int nBytes;

        // http://support.microsoft.com/kb/186586/zh-cn
        // When you use GetObject() to obtain information about a DIB section, GetObject() 
        // always returns a DIBSECTION containing a BITMAPINFOHEADER with a positive height 
        // value, regardless of the sign of the height it was created with. 
        // If you need to track the sign of the height in an application, you will need to
        // do this programmatically. 
        // 当使用 GetObject() 以获得有关 DIB 部分信息时，GetObject() 将始终返回包含正高度值，
        // 而不考虑与创建它的高度的符号 BITMAPINFOHEADER DIBSECTION。如果您需要跟踪应用程序
        // 中的高度的符号，您需要以编程方式执行此操作。

		nBytes = ::GetObject( m_hBitmap, sizeof( DIBSECTION ), &dibsection );
		if( nBytes == sizeof( DIBSECTION ) )
		{
			m_bIsDIBSection = true;
			m_nWidth = dibsection.dsBmih.biWidth;
			m_nHeight = abs( dibsection.dsBmih.biHeight );
			m_nBPP = dibsection.dsBmih.biBitCount;
			m_nPitch = ComputePitch( m_nWidth, m_nBPP );
			m_pBits = dibsection.dsBm.bmBits;
			if( eOrientation == DIBOR_DEFAULT )
			{
				eOrientation = (dibsection.dsBmih.biHeight > 0) ? DIBOR_BOTTOMUP : DIBOR_TOPDOWN;
			}
			if( eOrientation == DIBOR_BOTTOMUP )
			{
				m_pBits = LPBYTE( m_pBits )+((m_nHeight-1)*m_nPitch);
				m_nPitch = -m_nPitch;
			}
		}
		else
		{
			// Non-DIBSection
			assert( nBytes == sizeof( BITMAP ) );
			m_bIsDIBSection = false;
			m_nWidth = dibsection.dsBm.bmWidth;
			m_nHeight = dibsection.dsBm.bmHeight;
			m_nBPP = dibsection.dsBm.bmBitsPixel;
			m_nPitch = 0;
			m_pBits = 0;
		}
		m_iTransparentColor = -1;
		m_bHasAlphaChannel = (m_nBPP==32);
	}

	inline void CDuiImage::GenerateHalftonePalette( LPRGBQUAD prgbPalette )
	{
		int r;
		int g;
		int b;
		int gray;
		LPRGBQUAD prgbEntry;

		prgbEntry = prgbPalette;
		for( r = 0; r < 6; r++ )
		{
			for( g = 0; g < 6; g++ )
			{
				for( b = 0; b < 6; b++ )
				{
					prgbEntry->rgbBlue = BYTE( b*255/5 );
					prgbEntry->rgbGreen = BYTE( g*255/5 );
					prgbEntry->rgbRed = BYTE( r*255/5 );
					prgbEntry->rgbReserved = 0;

					prgbEntry++;
				}
			}
		}

		for( gray = 0; gray < 20; gray++ )
		{
			prgbEntry->rgbBlue = BYTE( gray*255/20 );
			prgbEntry->rgbGreen = BYTE( gray*255/20 );
			prgbEntry->rgbRed = BYTE( gray*255/20 );
			prgbEntry->rgbReserved = 0;

			prgbEntry++;
		}
	}

	inline COLORREF CDuiImage::GetTransparentRGB() const
	{
		if( m_iTransparentColor == CLR_USE_COLOR_DIRECT )
			return m_colorTransparent;

		RGBQUAD rgb;

		bool bHaveDC = (m_hDC != NULL);   // libo add 用于外部调用获取颜色值
		if( !bHaveDC )
			GetDC();

		assert( m_hDC != NULL );  // Must have a DC
		assert( m_iTransparentColor != -1 );

		::GetDIBColorTable( m_hDC, m_iTransparentColor, 1, &rgb );

		if( !bHaveDC )
			ReleaseDC();

		return( RGB( rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue ) );
	}

	//kk:add填充位图矩形颜色
	inline bool CDuiImage::DrawColor(HDC hDC, const RECT& rc, DWORD color)
	{
		::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		return true;
	}

	inline bool CDuiImage::DrawColor(HDC hDC, DWORD color,int nx,int ny,int nwidth,int nheight)
	{
		if(nwidth == 0) nwidth = GetWidth();
		if(nheight == 0) nheight = GetHeight();
		RECT rc;
		rc.left = nx;
		rc.top = ny;
		rc.right = rc.left + nwidth;
		rc.bottom = rc.top + nheight;
		::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		return true;
	}

	// libo 20120321 add
	//	创建pImage的一张去色图片
	//
	// modify 20120927 将浮点运算修改为整数运算 
	//	参考:http://blog.csdn.net/housisong/article/details/3884368 图形图像处理－之－彩色转化到灰度的速度优化. 
	//	HouSisong@GMail.com  2009.02.08
	// 
	inline bool CDuiImage::CopyGrayImageFrom( const CDuiImage* pImage )
	{
		if( NULL == pImage || (pImage->GetBPP()!= 32 && pImage->GetBPP()!= 24))  // 暂时不支持其它格式的图片，不了解其格式
			return false;
	
		this->Destroy();
		this->Create(pImage->GetWidth(), pImage->GetHeight(), pImage->GetBPP(), (pImage->HasAlphaChannel()==true)?createAlphaChannel:0 );
		if (this->IsNull())
			return false;

		// Gray = R*0.299 + G*0.587  + B*0.114;
		//灰度转换系数   
		const double gray_r_coeff = 0.299;  
		const double gray_g_coeff = 0.587;  
		const double gray_b_coeff = 0.114;

		const long bit=16;  
		const int gray_r_coeff_int = (int)( gray_r_coeff*(1<<bit)+0.4999999 );  
		const int gray_g_coeff_int = (int)( gray_g_coeff*(1<<bit)+0.4999999 );  
		const int gray_b_coeff_int = (1<<bit)-gray_r_coeff_int-gray_g_coeff_int; 

		const BYTE* pImageBits = (const BYTE*)pImage->GetBits();
		BYTE* pNewImageBits = (BYTE*)m_pBits;
		int   bytesperpx    = m_nBPP>>3;
		int   bytesperrow   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/;   // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch

		for (int row = 0; row < m_nHeight; row ++ )
		{
			for (int i = 0; i < bytesperrow; i += bytesperpx)
			{
				//int rgb = (int)(pImageBits[i]*0.11 + pImageBits[i+1]*0.59 + pImageBits[i+2]*0.3);  // 去色算法，可优化非浮点运算
				int rgb = (pImageBits[i]*gray_b_coeff_int   + 
					       pImageBits[i+1]*gray_g_coeff_int + 
						   pImageBits[i+2]*gray_r_coeff_int) >> bit;  
				
				pNewImageBits[i] = pNewImageBits[i+1] = pNewImageBits[i+2] = rgb;

				if (m_bHasAlphaChannel)
					pNewImageBits[i+3] = pImageBits[i+3];
			}

			pNewImageBits += m_nPitch;
			pImageBits += m_nPitch;
		}

		return true;
	}
	inline bool CDuiImage::DrawGray(HDC hDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
	{
		CDuiImage  grayImage;
		if (false == this->CopyGrayImageFrom(&grayImage))
			return false;

		BOOL bRet = grayImage.Draw(hDC, xDest, yDest, wDest, hDest, xSrc, ySrc, wDest, hDest);
		grayImage.Destroy();
		return bRet?true:false;
	}

	inline void CDuiImage::GetSystemPaletteEntries(RGBQUAD *pRGBQuad,int nBitsCount)
	{
		PALETTEENTRY* paletteEntry = new PALETTEENTRY[256];
		//获取系统调色盘的信息.系统调色盘的改变由系统来完成, 这里不能改变系统调色盘.我们希望这个类不要干涉系统事务
		::GetSystemPaletteEntries(GetDC(), 0, 256, paletteEntry); 
		ReleaseDC();

		for(int index = 0; index < 256; index++)
		{
			pRGBQuad[index].rgbRed = paletteEntry[index].peRed; 
			pRGBQuad[index].rgbGreen = paletteEntry[index].peGreen; 
			pRGBQuad[index].rgbBlue = paletteEntry[index].peBlue; 
			pRGBQuad[index].rgbReserved = 0; 
		}

		delete[] paletteEntry; 
	}

	inline bool CDuiImage::CreateEmptyImage(int nWidth,int nHeight,int nBitsCount,DWORD dwMode,CDuiImage* pImg)
	{
		if(pImg == NULL) pImg = this;
		if (!pImg->IsNull()) pImg->Destroy();
		if(pImg->Create(nWidth,nHeight,nBitsCount,dwMode) == false)  return false;
		if(nBitsCount <= 8 && pImg->IsIndexed())//需要调色板
		{
			int nColors = pImg->GetMaxColorTableEntries();
			RGBQUAD pal[256];
			pImg->GetSystemPaletteEntries(pal,nBitsCount);
			pImg->SetColorTable(0,nColors,pal);
		}
		return true;
	}

	inline PBYTE CDuiImage::GetBitmapBits()
	{
		PBYTE pBitmapData = NULL;

		if (IsNull())
		{
			return pBitmapData;
		}

		if (GetPitch() < 0 )
		{
			pBitmapData = (PBYTE)GetPixelAddress(0,GetHeight()-1);
		}
		else
		{
			pBitmapData = (PBYTE)GetPixelAddress(0,0);
		}	

		return pBitmapData;
	}

	inline bool CDuiImage::CopyImageFromDC(HDC hDC,int nx,int ny,int nWidth,int nHeight,int nBitsCount)
	{
		if(CreateEmptyImage(nWidth,nHeight,nBitsCount,CDuiImage::excludePNG,this) == false) return false;

		BLENDFUNCTION blend;
		BOOL bResult;

		blend.SourceConstantAlpha = 0xff;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		if( m_bHasAlphaChannel )
		{
			blend.AlphaFormat = AC_SRC_ALPHA;
		}
		else
		{
			blend.AlphaFormat = 0;
		}

		GetDC();

		bResult = ::AlphaBlend( m_hDC, 0, 0, nWidth, nHeight, hDC, 
			nx, ny, nWidth, nHeight, blend );

		ReleaseDC();

		GdiFlush();
		if(bResult) return true;
		return false;
	}
	inline bool CDuiImage::CopyImage(CDuiImage* psrcImage,int nx,int ny,int nWidth,int nHeight,int nBitsCount)
	{
		if (psrcImage == NULL) return false;

		if (psrcImage->IsNull()) return false;

		if (nWidth == 0) nWidth = psrcImage ->GetWidth() - nx;

		if (nHeight == 0) nHeight = psrcImage->GetHeight() - ny;

		if(CreateEmptyImage(nWidth,nHeight,nBitsCount,CDuiImage::excludePNG,this) == false) return false;

		
		::BitBlt(GetDC(),0,0,nWidth,nHeight,psrcImage->GetDC(),nx,ny,SRCCOPY);

		psrcImage->ReleaseDC();
		ReleaseDC();

		GdiFlush();
		return true;
	}


	// 为了解决内存在另一个模块分配，却在其它模块释放，在这里修改为由外部自己分配内存
	inline bool CDuiImage::SaveBits(ImageData* pImageData, int* pNeedSize)
	{
		if (NULL == pImageData || NULL == pNeedSize)
			return false;

		if (m_nBPP != 24 && m_nBPP != 32)  // TODO: 暂不支持其它格式的（8位的是基于调色板的）
			return false;

		int   bytesperline = abs(m_nPitch);
		int nSize = bytesperline*m_nHeight;
		if (*pNeedSize != nSize)
		{
			*pNeedSize = nSize;
			return false;
		}

		if (NULL == pImageData->m_ptr)
			return false;

		BYTE* pThisBits = (BYTE*)m_pBits;
		
		pImageData->m_nWidth = m_nWidth;
		pImageData->m_nHeight = m_nHeight;
		pImageData->m_nStride = m_nPitch;
		pImageData->m_nbpp = m_nBPP;

		// 创建内存
//		pImageData->m_ptr = new BYTE[nSize];
		pImageData->m_pScan0 = pImageData->m_ptr;

		if (m_nPitch < 0)
			pImageData->m_pScan0 += ((m_nHeight-1)*bytesperline);

		// 内存拷贝
		BYTE* pTemp = pImageData->m_pScan0;
		for (int row = 0; row < m_nHeight; row ++ )
		{
			memcpy(pTemp, pThisBits, bytesperline);
			pThisBits += m_nPitch;
			pTemp += m_nPitch;
		}

		return true;
	}
	inline void CDuiImage::RestoreBits( ImageData* pImageData )
	{
		// TODO
	}

	//
	//	根据参数中提供的原始数据pSaveBits，将自己偏移wNewH色调
	//
	//  色调调节方法：
	//    1)正常模式：将所有像素的H值偏移h，
	//    2)着色模式：将所有像素的H值设置为h
	//
	//	  H的取值范围是0 - 240的环形值，到达240之后又回到0起点
	//
	//	饱和度调节方法：
	//   饱和度的变化规律是： dS = s/100;
	//	  当dS > 0, newS = S / (1-dS)
	//	  当dS < 0, newS = S * (1+dS)
	//
	//	  S的取值范围为 -100 ~ +100
	//
	//	亮度调节方法：
	//    不需要转换为HLS模式，采用的方法是将一张白色图片或者一张黑色图片盖上原图上的方法
	//    R = 255*d + R*(1-d) = 255*d + R - R*d = R + (255-R)*d
	//    R = 0*d + R*(1-d) = R - R*d
	//    
	//    L的取值范围为 -100 ~ +100
	//	

	/*  附表（HSL/V/B在各种环境下的取值范围）：
		Applications       Space       H Range      S Range      L/V/B Range

		Paint Shop Pro     HSL         0 - 255      0 - 255      L  0 - 255
		Gimp               HSV         0 - 360°     0 - 100      V  0 - 100
		Photoshop          HSV         0 - 360°     0 - 100%     B  0 - 100%
		Windows            HSL         0 - 240      0 - 240      L  0 - 240
		Linux / KDE        HSV         0 - 360°     0 - 255      V  0 - 255
		GTK                HSV         0 - 360°     0 - 1.0      V  0 - 1.0
		Java (awt.Color)   HSV         0 - 1.0      0 - 1.0      B  0 - 1.0
		Apple              HSV         0 - 360°     0 - 100%     L  0 - 100%
	*/

	inline bool ChangeColorHue(BYTE& R, BYTE& G, BYTE& B, short h, bool bOffsetOrReplace)
	{
		if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
			return false;
		
		WORD hTemp=0,lTemp=0,sTemp=0;
		::ColorRGBToHLS(RGB(R,G,B), &hTemp,&lTemp,&sTemp);

		short h2 = h;
		if (bOffsetOrReplace)
			h2 = hTemp + h;
		else
			h2 = h;

		while(h2 < MIN_HUE_VALUE)
			h2 += MAX_HUE_VALUE;
		while (h2 >= MAX_HUE_VALUE)
			h2 -= MAX_HUE_VALUE;
		hTemp = h2;

		COLORREF color = ::ColorHLSToRGB(hTemp,lTemp,sTemp);
		R = GetRValue(color);
		G = GetGValue(color);
		B = GetBValue(color);

		return true;
	}
	inline void ChangeColorLuminance(BYTE& R, BYTE& G, BYTE& B, short l, float dL)  // dL = l/100;
	{
		if (l > 0)  // 相当于是在背景图上面盖了一张全白的图片（alpha为dL)
		{  
			R = R + (BYTE)((255 - R) * dL);  
			G = G + (BYTE)((255 - G) * dL);  
			B = B + (BYTE)((255 - B) * dL);  
		}  
		else if (l < 0)  // 相当于是在背景图上面盖了一张全黑的图片(alpha为dL)
		{  
			R = R + (BYTE)(R * dL);  
			G = G + (BYTE)(G * dL);   
			B = B + (BYTE)(B * dL);  
		}  
#define CHECK_RGB_RANGE(x)  \
		if (x>255) x = 255; \
		if (x<0)   x = 0;

		CHECK_RGB_RANGE(R);
		CHECK_RGB_RANGE(G);
		CHECK_RGB_RANGE(B);
	}
	inline bool ChangeColorHueAndSaturation(BYTE& R, BYTE& G, BYTE& B, short h, bool bOffsetOrReplace, short s, float dS)
	{
		if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
			return false;

		WORD hTemp=0,lTemp=0,sTemp=0;
		::ColorRGBToHLS(RGB(R,G,B), &hTemp,&lTemp,&sTemp);

		// hue
		short h2 = h;
		if (bOffsetOrReplace)
			h2 = hTemp + h;
		else
			h2 = h;

		while(h2 < MIN_HUE_VALUE)
			h2 += MAX_HUE_VALUE;
		while (h2 >= MAX_HUE_VALUE)
			h2 -= MAX_HUE_VALUE;
		hTemp = h2;

		// saturation
		short s2 = sTemp;
		if (s > 0)
			s2 = (short)(s2 / (1-dS));
		else
			s2 = (short)(s2 * (1+dS));

		if(s2 <= MIN_SATURATION_VALUE)
			s2 = 0, hTemp = 160;  // 灰色系了

		if (s2 > MAX_SATURATION_VALUE)
			s2 = MAX_SATURATION_VALUE;

		sTemp = s2;

		COLORREF color = ::ColorHLSToRGB(hTemp,lTemp,sTemp);
		R = GetRValue(color);
		G = GetGValue(color);
		B = GetBValue(color);

		return true;
	}
	inline bool ChangeColorSaturation(BYTE& R, BYTE& G, BYTE& B, short s, float dS)
	{
		if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
			return false;

		WORD hTemp=0,lTemp=0,sTemp=0;
		::ColorRGBToHLS(RGB(R,G,B), &hTemp,&lTemp,&sTemp);

		short s2 = sTemp;
		if (s > 0)
			s2 = (short)(s2 / (1-dS));
		else
			s2 = (short)(s2 * (1+dS));

		if(s2 <= MIN_SATURATION_VALUE)
			s2 = 0, hTemp = 160;  // 灰色系了

		if (s2 > MAX_SATURATION_VALUE)
			s2 = MAX_SATURATION_VALUE;

		sTemp = s2;

		COLORREF color = ::ColorHLSToRGB(hTemp,lTemp,sTemp);
		R = GetRValue(color);
		G = GetGValue(color);
		B = GetBValue(color);

		return true;
	}

	inline bool ChangeColorHLS(BYTE& R, BYTE& G, BYTE& B, short h, short l , short s, int nFlag )
	{
		bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
		bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
		bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
		bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false:true;
		if (l == 0)
			bChangeL = false;
		if (s == 0)
			bChangeS = false;

		if(false == bChangeH && false == bChangeL && false == bChangeS)
			return false;

		float dL = 0, ds = 0;
		if (bChangeL)
			dL = (float)(l/100.0); 
		if (bChangeS)
			ds = (float)(s/100.0);

		if (bChangeL)
			ChangeColorLuminance(R,G,B,l,dL);

		if (bChangeH && bChangeS)
		{
			ChangeColorHueAndSaturation(R,G,B,h,bSetHueMode,s,ds);
		}
		else
		{
			if (bChangeH)
				ChangeColorHue(R,G,B,h,bSetHueMode);
			if (bChangeS)
				ChangeColorSaturation(R,G,B,s,ds);
		}
		return true;
	}
	inline bool CDuiImage::ChangeHLS(const ImageData* pOriginImageData, short h, short l , short s, int nFlag)
	{
		if (NULL == pOriginImageData)
			return false;

		BYTE* pTemp = pOriginImageData->m_pScan0;
		if (NULL == pTemp)
			return false;

		if (m_nBPP != pOriginImageData->m_nbpp)
		{
			assert(0);
			return false;
		}

		bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
		bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
		bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
		bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false:true;

		if (l == 0)
			bChangeL = false;
		if (s == 0)
			bChangeS = false;
		if (h == 0)
			bChangeH = false;

// 		if(false == bChangeH && false == bChangeL && false == bChangeS) // 有可能是还原操作，因此不能直接退出 
// 			return false;

		BYTE* pNewImageBits = (BYTE*)m_pBits;
		int   bytesperpx    = m_nBPP>>3;
		int   bytesperline   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
		bool  bHaveAlphaChannel = GetBPP() == 32;

		float dL = 0, ds = 0;
		if (bChangeL)
			dL = (float)(l/100.0);   // 避免在循环中重复计算该值
		if (bChangeS)
			ds = (float)(s/100.0);

		for (int row = 0; row < m_nHeight; row ++)
		{
			for (int i = 0; i < bytesperline; i += bytesperpx)
			{
				BYTE B = pTemp[i];
				BYTE G = pTemp[i+1];
				BYTE R = pTemp[i+2];

				if (bChangeL)
					ChangeColorLuminance(R,G,B,l,dL);

				if (bChangeH && bChangeS)
				{
					ChangeColorHueAndSaturation(R,G,B,h,bSetHueMode,s,ds);
				}
				else
				{
					if (bChangeH)
						ChangeColorHue(R,G,B,h,bSetHueMode);
					if (bChangeS)
						ChangeColorSaturation(R,G,B,s,ds);
				}

				pNewImageBits[i]   = B;
				pNewImageBits[i+1] = G;
				pNewImageBits[i+2] = R;

				if (bHaveAlphaChannel)
				{
					pNewImageBits[i+3] = pTemp[i+3];
				}
			}

			pNewImageBits += m_nPitch;
			pTemp += pOriginImageData->m_nStride;
		}

		return true;
	}
	// libo 20120318 将DIB转换成DDB
	// hMemDC
	//		[in]	要用于兼容DDB的DC
	//
	inline bool CDuiImage::ChangeDIB2DDB(HDC hMemDC)
	{
		assert( NULL != m_hBitmap );
		assert( NULL != hMemDC );

		if( false == m_bIsDIBSection )
			return true;

		HBITMAP hDDB = ::CreateCompatibleBitmap(hMemDC,this->GetWidth(),this->GetHeight());
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hDDB);

		this->BitBlt(hMemDC, 0,0 );
		::SelectObject(hMemDC, hOldBitmap);

		this->Destroy();
		this->Attach(hDDB);

		return true;
	}
	
	// libo add 20120401 增加图像列表绘制方法
	inline bool CDuiImage::ImageList_Draw(HDC hDestDC, int x, int y, int col, int row, int cx, int cy )
	{
		assert( col >= 0 );
		assert( row >= 0 );
		assert( cx >= 0 );
		assert( cy >= 0 );

		int xSrc = col * cx;
		int ySrc = row * cy;

		if( xSrc > m_nWidth )
			return false;
		if( ySrc > m_nHeight )
			return false;

		this->Draw(hDestDC, x, y, cx, cy, xSrc, ySrc, cx, cy );

		return true;
	}


	
	// 修改图片的透明度
	inline bool CDuiImage::ModifyAlpha(ImageData* pOriginImageData, int nAlphaPercent)
	{
		if (m_nBPP != 32)
			return false;

		if (NULL == pOriginImageData)
			return false;

		BYTE* pTemp = pOriginImageData->m_pScan0;
		if (NULL == pTemp)
			return false;
		
		BYTE* pNewImageBits = (BYTE*)m_pBits;
		int   bytesperpx    = m_nBPP>>3;
		int   bytesperline   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
		bool  bHaveAlphaChannel = GetBPP() == 32;

		for (int row = 0; row < m_nHeight; row ++)
		{
			for (int i = 0; i < bytesperline; i += bytesperpx)
			{
				BYTE B = pTemp[i];
				BYTE G = pTemp[i+1];
				BYTE R = pTemp[i+2];

				BYTE  a = pTemp[i+3];
				a = a * nAlphaPercent / 100;

				pNewImageBits[i+3] = a;
				pNewImageBits[i]   = pTemp[i]  *a/255;
				pNewImageBits[i+1] = pTemp[i+1]*a/255;
				pNewImageBits[i+2] = pTemp[i+2]*a/255;
			}

			pNewImageBits += m_nPitch;
			pTemp += pOriginImageData->m_nStride;
		}
		return true;
	}

	// 设置该图片中所有位置的alpha值。该图一般是一个临时图片，用于在分层窗口上使用HDC进行绘制
	inline bool  CDuiImage::SetAlpha(int nAlpha)
	{
		if (m_nBPP != 32)
			return false;

		BYTE* pImageBits = (BYTE*)m_pBits;
		for (int row = 0; row < m_nHeight; row ++)
		{
			for (int i = 0; i < m_nWidth; i ++)
			{
				pImageBits[4*i+3]	= nAlpha;
			}
			pImageBits+= m_nPitch;
		}

		return true;
	}

};  // namespace 

#endif //Image_h__

#ifndef Color_h__
#define Color_h__
namespace DuiLib
{
	class UILIB_API DuiColor
	{
	public:
		DuiColor();
		DuiColor(int r, int g, int b, int a);
		DuiColor(DWORD rgba);
		static DuiColor* CreateInstance(COLORREF color);

		operator COLORREF();
		union
		{
			COLORREF   m_col;
			struct  
			{
				byte   r;
				byte   g;
				byte   b;
				byte   a;
			};
		};

		int   GetR();
		int   GetG();
		int   GetB();
		int   GetA();

		void  SetR(byte n);
		void  SetG(byte n);
		void  SetB(byte n);
		void  SetA(byte n);

		// 保留ALPHAW值颜色拷贝
		void  ReplaceRGB(COLORREF c);
		void  ReplaceRGB(BYTE r1, BYTE g1, BYTE b1);
		void  SetColor(COLORREF c);
		COLORREF  GetGDICompatibleValue();

		// 在外部使用Color对象时，既有可能是通过GetColor获取到的Color*指针，也有可能外部自己直接设置
		// 一个COLORREF值，这个时候就得new一个Color*，使得逻辑与GetColor保持一致。为了达到释放逻辑一
		// 致的目的，将释放函数都写成Release()
		long  AddRef();
		long  Release();

	private:
		long  lRef;
	};

	typedef DuiColor* DuiColorPtr;
	//
	// 9宫拉伸区域定义
	//
	class UILIB_API Image9Region
	{
	public:
		Image9Region(){ Set(0); }
		void Set( WORD w );
		void Set( WORD wLeftRight, WORD wTopBottom );
		void Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom );

		WORD topleft;
		WORD top;
		WORD topright;
		WORD left;
		WORD right;
		WORD bottomleft;
		WORD bottom;
		WORD bottomright;
	};
}
#endif //Color_h__
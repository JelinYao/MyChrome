#pragma once
#include "UIOptioneEx.h"
#include <vector>
using std::vector;


namespace DuiLib
{
	class UILIB_API COptionLayoutUI :
		public CVerticalLayoutUI,
		public COptionLayoutCallback
	{
	public:
		COptionLayoutUI(void);
		~COptionLayoutUI(void);
		LPVOID GetInterface(LPCTSTR pstrName);
		virtual void ItemSelect(CControlUI* pItem);
		virtual bool Add(CControlUI* pControl);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		int	GetOptionIndex(CControlUI* pItem);
		void SelectItem(const int nIndex);
	private:
		int	m_nSelectIndex;
		vector<CControlUI*>	m_pOptionItems;
		vector<CControlUI*> m_pHorizItems;
	};
}

#pragma once

namespace DuiLib
{
	class COptionLayoutCallback
	{
	public:
		virtual void ItemSelect(CControlUI* pItem)=0;
	};


	class UILIB_API COptioneExUI :
		public COptionUI
	{
	public:
		COptioneExUI(void);
		~COptioneExUI(void);
		LPVOID GetInterface(LPCTSTR pstrName);
		void DoEvent(TEventUI& event);
		void SetOptionLayoutCallback(COptionLayoutCallback* pCallback);
	private:
		COptionLayoutCallback*	m_pCallback;
	};
}

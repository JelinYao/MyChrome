#pragma once
#include "ChromePage.h"




class CControlCallback
	: public IDialogBuilderCallback
{
public:
	virtual CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if ( _tcscmp(pstrClass, _T("CefWebkit")) == 0 )
			return new CCefWebkitUI;
// 		if ( _tcscmp(pstrClass, _T("ChromePage")) == 0 ) 
// 			return new CChromePage(&m_PaintManager);
		return NULL;
	}
};
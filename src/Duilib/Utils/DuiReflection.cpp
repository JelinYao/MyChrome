#include "StdAfx.h"
#include "DuiReflection.h"

namespace DuiLib
{
	//************************************
	// 函数名称: RegistDuiClass
	// 返回类型: void
	// 参数信息: string _duiClassName					类名
	// 参数信息: DuiInstanceFun _createDuiInstanceFun	注册的函数指针
	// 函数说明：类反射注册，若已经注册过，则直接返回
	//************************************
	void DuiObjectMap::RegistDuiClass( string _duiClassName, DuiInstanceFun _createDuiInstanceFun )
	{
		if(GetDuiClassMaps().size() && GetDuiClassMaps().find(_duiClassName) != GetDuiClassMaps().end())
			return;

		GetDuiClassMaps()[_duiClassName] = _createDuiInstanceFun;
	}

	//************************************
	// 函数名称: createInstance
	// 返回类型: void*
	// 参数信息: string _duiClassName
	// 函数说明：通过类名构造实例
	//************************************
	void* DuiObjectMap::createInstance( string _duiClassName )
	{
		if(GetDuiClassMaps().size() && GetDuiClassMaps().find(_duiClassName) == GetDuiClassMaps().end())
		{
			std::map<string, DuiInstanceFun>& mDuiClassMaps = GetDuiClassMaps();
			DUITRACE(_T("没有找到类对象：%S "),_duiClassName.c_str());
			return NULL;
		}

		return (GetDuiClassMaps()[_duiClassName])();
	}

	//************************************
	// 函数名称: createInstance
	// 返回类型: void*
	// 参数信息: LPCTSTR _duiClassName
	// 函数说明：
	//************************************
	void* DuiObjectMap::createInstance( LPCTSTR _duiClassName )
	{
		char duiClassName[64];
		sprintf(duiClassName,"%S",_duiClassName);
		return createInstance(duiClassName);
	}

	//************************************
	// 函数名称: GetDuiClassMaps
	// 返回类型: std::map<string, DuiInstanceFun>&
	// 函数说明: 
	//************************************
	std::map<string, DuiInstanceFun>& DuiObjectMap::GetDuiClassMaps()
	{
		static std::map<string, DuiInstanceFun> m_duiClassMaps;
		return m_duiClassMaps;
	}

}


#ifndef DuiReflection_h__
#define DuiReflection_h__

#include<string>
#include<map>
using namespace std;

namespace DuiLib
{
	typedef void* (*DuiInstanceFun)();
	class UILIB_API DuiObjectMap
	{
	private:
		DuiObjectMap(){};
	public:

		static void RegistDuiClass(string _duiClassName, DuiInstanceFun _createDuiInstanceFun);
		static void* createInstance(string _duiClassName);
		static void* createInstance(LPCTSTR _duiClassName);
		static std::map<string, DuiInstanceFun>& GetDuiClassMaps();
	};

	template<typename T>
	class UILIB_API DelegatingObject 
	{
	public:
		DelegatingObject(string _duiClassName)
		{
			DuiObjectMap::RegistDuiClass(_duiClassName,(DuiInstanceFun)&(DelegatingObject::CreateClassInstance));
		}

		static T* CreateClassInstance()
		{
			return static_cast<T*>(new T);
		}
	};

#ifndef REGIST_DUICLASS
#define REGIST_DUICLASS(DuiObject) DelegatingObject<DuiObject> __class_##DuiObject( #DuiObject );
#endif

#ifdef _UNICODE
	template<class TObj>
	TObj CreateDuiInstance(LPCSTR _DuiClassName){
		return static_cast<TObj>(DuiObjectMap::createInstance(_DuiClassName));
	}
#endif // !_UNICODE

	template<class TObj>
	TObj CreateDuiInstance(LPCTSTR _DuiClassName){
		return static_cast<TObj>(DuiObjectMap::createInstance(_DuiClassName));
	}
}

#endif // DuiReflection_h__
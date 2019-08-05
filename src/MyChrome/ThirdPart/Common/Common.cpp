#include "stdafx.h"
#include "Common.h"



HANDLE GetTokenByPid( DWORD dwPid )
{
	HANDLE hRetToken  = NULL;
	HANDLE hProcess	  = NULL;
	HANDLE hToken     = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	try 
	{	
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if ( NULL == hProcess )
			throw L"";
		if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) 
			throw L"";
		pSD = (PSECURITY_DESCRIPTOR) GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (pSD == NULL) 
			throw L"";
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
			throw L"";
		if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE)) 
			throw L"";
		SECURITY_ATTRIBUTES sa;
		sa.nLength              = sizeof(sa);
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle       = TRUE;
		if ( !DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, &sa, \
			SecurityImpersonation, TokenPrimary, &hRetToken) ) 
			throw L"";
	} 
	catch(const TCHAR* pMsg)
	{

	}
	catch(...) 
	{

	}
	if ( hToken )
		CloseHandle(hToken);
	if ( hProcess )
		CloseHandle(hProcess);
	if ( pSD )
		GlobalFree(pSD);
	return hRetToken;
}


BOOL IsRunasAdmin( HANDLE hProcess )
{
	BOOL bElevated = FALSE;  
	HANDLE hToken = NULL;  

	if ( !OpenProcessToken( hProcess, TOKEN_QUERY, &hToken ) )
	{
		return FALSE;
	}
	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;  
	// Retrieve token elevation information
	if ( GetTokenInformation( hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen ) )
	{  
		if ( dwRetLen == sizeof(tokenEle) )
		{
			bElevated = tokenEle.TokenIsElevated;  
		}
	} 
	CloseHandle( hToken );  
	return bElevated;  
}

string U2A(const wstring& str)
{
	string strDes;
	if ( str.empty() )
		goto __end;
	int nLen=::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
	if ( 0==nLen )
		goto __end;
	char* pBuffer=new char[nLen+1];
	memset(pBuffer, 0, nLen+1);
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen, NULL, NULL);
	pBuffer[nLen]='\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

wstring A2U(const string& str)
{
	wstring strDes;
	if ( str.empty() )
		goto __end;
	int nLen=::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	if ( 0==nLen )
		goto __end;
	wchar_t* pBuffer=new wchar_t[nLen+1];
	memset(pBuffer, 0, nLen+1);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen);
	pBuffer[nLen]='\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

string U2Utf(const wstring& wstrUnicode)  
{  
	string strRet;
	if( wstrUnicode.empty() )
		return strRet;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, NULL, 0, NULL, NULL);  
	char* pBuffer=new char[nLen+1];
	pBuffer[nLen] = '\0';
	nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, pBuffer, nLen, NULL, NULL); 
	strRet.append(pBuffer);
	delete[] pBuffer;
	return strRet;  
}

wstring Utf2U(const string &str)
{
	int u16Len = ::MultiByteToWideChar(CP_UTF8, NULL,str.c_str(),(int)str.size(), NULL, 0);
	wchar_t* wstrBuf = new wchar_t[u16Len + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(),(int)str.size(), wstrBuf, u16Len);
	wstrBuf[u16Len] = L'\0';
	wstring wStr;
	wStr.assign(wstrBuf, u16Len);
	delete [] wstrBuf;
	return wStr;
}

bool IsUrl(const wstring& strUrl)
{
	string s = U2A(strUrl);
	for ( size_t i=0; i<s.size(); ++i )
	{
		char ch = s[i];
		if ( ch<0 )
			return false;
		if ( isalnum(ch) || (ch=='_') || (ch=='/') || (ch=='.') || (ch=='%') )
			continue;
		return false;
	}
	return true;
}

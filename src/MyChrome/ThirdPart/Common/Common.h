#pragma once


HANDLE	GetTokenByPid( DWORD dwPid );

BOOL	IsRunasAdmin( HANDLE hProcess );


wstring A2U(const string& str);

string	U2Utf(const wstring& wstrUnicode) ;

wstring Utf2U(const string &str);

bool	IsUrl(const wstring& strUrl);
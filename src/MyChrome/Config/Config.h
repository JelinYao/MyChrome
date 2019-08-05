#pragma once



class CConfig
{
public:
	static CConfig*	Instance()
	{
		static CConfig config;
		return &config;
	}
	const wstring&	GetHomePage()const		{ return m_strHomePage; }
protected:
	CConfig();
	~CConfig();
private:
	wstring	m_strHomePage;
};
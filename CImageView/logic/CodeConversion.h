#pragma once

#include <Windows.h>
#include <string>
using namespace std;

#pragma warning(disable : 4996)

class CCodeConversion
{
public:
	CCodeConversion(void);
	~CCodeConversion(void);

public:
	static CCodeConversion& Instance()
	{
		static CCodeConversion m_ccc;
		return m_ccc;
	}

	string UrlGB2312(char * str);								//urlgb2312编码
	string UrlUTF8(char * str);									//urlutf8 编码
	string UrlUTF8Decode(string str);							//urlutf8解码
	string UrlGB2312Decode(string str);							//urlgb2312解码

	std::string U2GEx(std::string strUtf8);						//utf-8转gb2312
	std::string G2UEx(std::string strGB2312);					//gb2312转utf-8

	wstring MultCHarToWideChar(string str);						//多字节字符集转宽字符集
	string WideCharToMultiChar(wstring str);					//宽字符集转多字节字符集

private:	//暂不使用
	void UTF_8ToGB2312(string &pOut, char *pText, int pLen);	//utf_8转为gb2312
	void GB2312ToUTF_8(string& pOut,char *pText, int pLen);		//gb2312 转utf_8

private:
	void Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer);
	void UTF_8ToUnicode(WCHAR* pOut,char *pText);
	void UnicodeToUTF_8(char* pOut,WCHAR* pText);
	void UnicodeToGB2312(char* pOut,WCHAR uData);
	char CharToInt(char ch);
	char StrToBin(char *str);

	char* U2G(const char* utf8);
	char* G2U(const char* gb2312);
};

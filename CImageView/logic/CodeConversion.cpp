#include "stdafx.h"
#include "CodeConversion.h"

CCodeConversion::CCodeConversion(void)
{
}

CCodeConversion::~CCodeConversion(void)
{
}

void CCodeConversion::Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return;
}

void CCodeConversion::UTF_8ToUnicode(WCHAR* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

void CCodeConversion::UnicodeToUTF_8(char* pOut,WCHAR* pText)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

void CCodeConversion::UnicodeToGB2312(char* pOut,WCHAR uData)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
	return;
}

//做为解Url使用
char CCodeConversion:: CharToInt(char ch){
	if(ch>='0' && ch<='9')return (char)(ch-'0');
	if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
	if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
	return -1;
}

char CCodeConversion::StrToBin(char *str){
	char tempWord[2];
	char chn;

	tempWord[0] = CharToInt(str[0]);                         //make the B to 11 -- 00001011
	tempWord[1] = CharToInt(str[1]);                         //make the 0 to 0 -- 00000000

	chn = (tempWord[0] << 4) | tempWord[1];                //to change the BO to 10110000

	return chn;
}

//UTF_8 转gb2312
void CCodeConversion::UTF_8ToGB2312(string &pOut, char *pText, int pLen)
{
	char buf[4];
	char* rst = new char[pLen + (pLen >> 2) + 2];
	memset(buf,0,4);
	memset(rst,0,pLen + (pLen >> 2) + 2);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(*(pText + i) >= 0)
		{

			rst[j++] = pText[i++];
		}
		else                
		{
			WCHAR Wtemp;


			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(buf,Wtemp);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];

			//newBuf[j] = Ctemp[0];
			//newBuf[j + 1] = Ctemp[1];

			i += 3;   
			j += 2;  
		}

	}
	rst[j]='\0';
	pOut = rst;
	delete []rst;
}

//GB2312 转为 UTF-8
void CCodeConversion::GB2312ToUTF_8(string& pOut,char *pText, int pLen)
{
	char buf[4];
	memset(buf,0,4);

	pOut.clear();

	int i = 0;
	while(i < pLen)
	{
		//如果是英文直接复制就可以
		if( pText[i] >= 0)
		{
			char asciistr[2]={0};
			asciistr[0] = (pText[i++]);
			pOut.append(asciistr);
		}
		else
		{
			WCHAR pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);

			UnicodeToUTF_8(buf,&pbuffer);

			pOut.append(buf);

			i += 2;
		}
	}

	return;
}

//把str编码为网页中的 GB2312 url encode ,英文不变，汉字双字节 如%3D%AE%88
string CCodeConversion::UrlGB2312(char * str)
{
	string dd;
	size_t len = strlen(str);
	for (size_t i=0;i<len;i++)
	{
		if(isalnum((BYTE)str[i]))
		{
			char tempbuff[2];
			sprintf_s(tempbuff,"%c",str[i]);
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)str[i]))
		{
			dd.append("+");
		}
		else
		{
			char tempbuff[4];
			sprintf_s(tempbuff,"%%%X%X",((BYTE*)str)[i] >>4,((BYTE*)str)[i] %16);
			dd.append(tempbuff);
		}

	}
	return dd;
}

//把str编码为网页中的 UTF-8 url encode ,英文不变，汉字三字节 如%3D%AE%88
string CCodeConversion::UrlUTF8(char * str)
{
	string tt;
	string dd;
	GB2312ToUTF_8(tt,str,(int)strlen(str));

	size_t len=tt.length();
	for (size_t i=0;i<len;i++)
	{
		if(isalnum((BYTE)tt.at(i)))
		{
			char tempbuff[2]={0};
			sprintf_s(tempbuff,"%c",(BYTE)tt.at(i));
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)tt.at(i)))
		{
			dd.append("+");
		}
		else
		{
			char tempbuff[4];
			sprintf_s(tempbuff,"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
			dd.append(tempbuff);
		}

	}
	return dd;
}

//把url GB2312解码
string CCodeConversion::UrlGB2312Decode(string str)
{
	string output="";
	char tmp[2];
	int i=0,idx=0,len=str.length();

	while(i<len){
		if(str[i]=='%'){
			tmp[0]=str[i+1];
			tmp[1]=str[i+2];
			output += StrToBin(tmp);
			i=i+3;
		}
		else if(str[i]=='+'){
			output+=' ';
			i++;
		}
		else{
			output+=str[i];
			i++;
		}
	}

	return output;
}

//把url utf8解码
string CCodeConversion::UrlUTF8Decode(string str)
{
	string output="";

	string temp =UrlGB2312Decode(str);//

	UTF_8ToGB2312(output,(char *)temp.data(),strlen(temp.data()));

	return output;

}

//UTF-8到utf8的轉換
char* CCodeConversion::U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}

//utf8到UTF-8的轉換
char* CCodeConversion::G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}

//UTF-8到utf8的轉換
std::string CCodeConversion::U2GEx(std::string strUtf8)
{
	std::string strBuf;
	char* pgb2312 = U2G(strUtf8.c_str());
	strBuf = pgb2312;
	delete pgb2312;
	return strBuf;
}

std::string CCodeConversion::G2UEx(std::string strGB2312)
{
	std::string strBuf;
	char* pUTF8 = G2U(strGB2312.c_str());
	strBuf = pUTF8;
	delete pUTF8;
	return strBuf;
}

wstring CCodeConversion::MultCHarToWideChar(string str)
{
	//获取缓冲区的大小，并申请空间，缓冲区大小是按字符计算的
	int len=MultiByteToWideChar(CP_ACP,0,str.c_str(),str.size(),NULL,0);
	TCHAR *buffer=new TCHAR[len+1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP,0,str.c_str(),str.size(),buffer,len);
	buffer[len]='\0';//添加字符串结尾
	//删除缓冲区并返回值
	wstring return_value;
	return_value.append(buffer);
	delete []buffer;
	return return_value;
}

string CCodeConversion::WideCharToMultiChar(wstring str)
{
	string return_value;
	//获取缓冲区的大小，并申请空间，缓冲区大小是按字节计算的
	int len=WideCharToMultiByte(CP_ACP,0,str.c_str(),str.size(),NULL,0,NULL,NULL);
	char *buffer=new char[len+1];
	WideCharToMultiByte(CP_ACP,0,str.c_str(),str.size(),buffer,len,NULL,NULL);
	buffer[len]='\0';
	//删除缓冲区并返回值
	return_value.append(buffer);
	delete []buffer;
	return return_value;
}
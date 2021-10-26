// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT



#include <atlimage.h>
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

//UpdateLayeredWindow
typedef BOOL (WINAPI *UDLWindow)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);

#include <iostream>
#include <vector>
#include <list>
#include <map>
using namespace std;

#ifdef DEBUG
	#ifdef ___EXE_D
		#include "vld.h"
		#pragma comment(lib, "vld.lib")
	#endif
#endif

//CImage��ת
void ImageHorizontalFlip(ATL::CImage* &dest, ATL::CImage* &src);
//Cimage��ת
void ImageRotate(ATL::CImage* &dest, ATL::CImage* &src);
//Cimage����
void ImageCopy(CImage* const &srcImage, CImage* &destImage, int x = 0, int y = 0, int w = 0, int h = 0);
//���·����Ŀ¼�����ļ���
bool SplitePath(IN const wchar_t* pSource, OUT wstring& strPath, OUT wstring& strFile);
//�����Ϣ
void OutMsg(wchar_t* pStr, ...);

#include "TagInfo.h"
#include "logic/ThreadLockEx.h"
#include "logic/LoadThread.h"
#include "logic/CodeConversion.h"



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
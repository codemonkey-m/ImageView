// CImageView.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCImageViewApp:
// �йش����ʵ�֣������ CImageView.cpp
//

class CCImageViewApp : public CWinApp
{
public:
	CCImageViewApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCImageViewApp theApp;
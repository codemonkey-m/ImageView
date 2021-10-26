#pragma once

#include "CImageViewDlg.h"

bool ShowWin(void *p, LoadType type)
{
	if (!p) return false;

	static CCImageViewDlg *pDlg = NULL;

	INT_PTR nState = IDOK;
	if (pDlg)
	{
		pDlg->SetLoadTypeAndPath(type, p);
		pDlg->Init();
		pDlg->SetFocus();
		pDlg->TrunPage();
	}
	else
	{
		CCImageViewDlg dlg;
		pDlg = &dlg;
		pDlg->SetLoadTypeAndPath(type, p);
		nState = pDlg->DoModal();

		pDlg = NULL;
	}

	/*INT_PTR nState = IDOK;
	dlg.SetLoadTypeAndPath(type, p);
	if (dlg.GetIsRuning())
	{
		dlg.Init();
		dlg.SetFocus();
		dlg.TrunPage();
	}
	else
		nState = dlg.DoModal();*/
	
	return nState == IDOK ? true : false;
}

extern "C"
{
	//显示列表图片
	_declspec(dllexport) bool ShowPicFile (const vector<string>* vtFiles)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		return ShowWin((void*)vtFiles, emLoadList);


		//if (!vtFiles)
		//	return false;

		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
		//CCImageViewDlg dlg;
		//dlg.SetLoadTypeAndPath(emLoadList, (void*)vtFiles);

		//INT_PTR nState = dlg.DoModal();

		////Gdiplus::GdiplusShutdown(m_ldiplusToken);

		//return nState == IDOK ? true : false;
	}

	//显示目录图片
	_declspec(dllexport) bool ShowDirPicFile (const char * pDir)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		return ShowWin((void*)pDir, emLoadDir);

		

		//不初始化关闭的时候会卡死
		/*ULONG_PTR		m_ldiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_ldiplusToken, &gdiplusStartupInput, NULL);*/

		//CCImageViewDlg dlg;
		//dlg.SetLoadTypeAndPath(emLoadDir, (void*)pDir);

		//INT_PTR nState = dlg.DoModal();

		////Gdiplus::GdiplusShutdown(m_ldiplusToken);

		//return nState == IDOK ? true : false;
	}  
}
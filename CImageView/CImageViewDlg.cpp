// CImageViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CImageView.h"
#include "CImageViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	THUMBNAIL_WIDTH		100
#define	THUMBNAIL_HEIGHT	90

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCImageViewDlg 对话框




CCImageViewDlg::CCImageViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCImageViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hCurNormal = AfxGetApp()->LoadCursor(IDC_CURSOR1);

	HWND hWnd = ::FindWindow(NULL, L"简单看图");
	if (hWnd)
		::PostMessage(hWnd, WM_CLOSE, 0, 0);

	//获取命令行
	wstring strCommand = AfxGetApp()->m_lpCmdLine;
	//AfxMessageBox(L"1111111");
	//如果文件名有特殊字符,系统会在两边加上双引号,所以要去掉
	if (wstring::npos != strCommand.find(L"\""))
		strCommand = strCommand.substr(1, strCommand.length() - 2);

	//AfxMessageBox(strCommand.data());

	//先调用,保证先析构,必须在GDI+释放之前析构GDI+对象
	//CLoadThread::Instance();

	/*Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_ldiplusToken, &gdiplusStartupInput, NULL);*/

	m_emLoadType = emLoadList;
	//m_strLoadPath = L".\\";
	//m_strLoadPath = L"G:\\Users\\Ming\\Desktop\\pics";
	//m_pLoadPath = &m_strLoadPath;

	m_vtLoadFileList.push_back(strCommand);
	m_pLoadPath = &m_vtLoadFileList;

	SetLoadTypeAndPath(emLoadDir, (const void*)strCommand.c_str());
	
	m_bIsRuning = false;

	//屏幕里面不包括任务栏
	RECT rt;  
	SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);  
	m_stCurScreenSize.cx = rt.right;
	m_stCurScreenSize.cy = rt.bottom;
}

void CCImageViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCImageViewDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_CREATE()
//	ON_WM_KEYDOWN()
ON_WM_TIMER()
ON_COMMAND(ID_32772, &CCImageViewDlg::OnClickCopy)
ON_COMMAND(ID_32773, &CCImageViewDlg::OnClickSaveToFile)
END_MESSAGE_MAP()


// CCImageViewDlg 消息处理程序

BOOL CCImageViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowText(L"简单看图");

	// TODO: 在此添加额外的初始化代码
	//LoadAllFileFromDir(L"E:\\project\\Project\\CImageView\\Debug\\pics");
	//LoadAllFileFromDir(L"G:\\Users\\Ming\\Desktop");

	//不初始化关闭的时候会卡死
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_ldiplusToken, &gdiplusStartupInput, NULL);

	m_cLoadThread.StartThread(m_stCurScreenSize);

	//CLoadThread::Instance().LoadFile(emLoadDir, L"E:\\备份\\相机备份\\8297w");

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^WS_EX_LAYERED);

	//隐藏任务栏
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);

	//窗口置顶
	//::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION );

	//获取UpdateLayeredWindow
	HMODULE hFuncInst=LoadLibrary(L"User32.DLL");
	m_hUpdateLayeredWindow=(UDLWindow)GetProcAddress(hFuncInst,"UpdateLayeredWindow");
	FreeLibrary(hFuncInst);

	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//菜单初始化
	m_cTip.Create(this);
	//m_cTip.AddTool(this, L"");
	//延迟时间
	//m_cTip.SetDelayTime(1000);
	//设置文本和背景颜色
	m_cTip.SetTipBkColor(RGB(255, 255, 255));
	m_cTip.SetTipTextColor(RGB(255, 0, 0));
	//设置是否启用
	m_cTip.Activate(TRUE);

	////绘制内存位图
	//m_hdcTmp=GetDC()->m_hDC;
	//m_hdcMemory=CreateCompatibleDC(m_hdcTmp);
	//HBITMAP hBitMap=CreateCompatibleBitmap(m_hdcTmp, m_stCurScreenSize.cx, m_stCurScreenSize.cy);
	//SelectObject(m_hdcMemory,hBitMap);
	//m_pGh = Graphics::FromHDC(m_hdcMemory);

	Init();
	m_bShowTrunPage = false;
	m_bCurImageChange = false;
	
	//m_pCurImageFull = NULL;
	m_emLastClickButton = emBottomMax;
	m_bLeftButtonState = false;
	m_bIsFullScreen = false;
	
	m_bUserCloseThumb = false;
	
	m_bIsDraging = false;
	m_bIsDragThumb = false;
	
	m_cRotateNum = 0;
	m_bOriginalSize = false;
	m_bProportionState = false;
	m_fThunmbClockX = 0;
	m_fThunmbClockY = 0;
	m_fThunmbClockW = 0;
	m_fThunmbClockH = 0;
	m_nCurThumbPosX = 0;
	m_nCurThumbPosY = 0;
	m_nCurThumbPosW = 0;
	m_nCurThumbPosH = 0;
	m_bShowTips = false;
	m_bSizeFirst = true;
	m_bIsRuning = true;

	//加载资源
	for (short w = 0; w < (sizeof(g_nOnlyRes) / sizeof(g_nOnlyRes[0])); ++w)
	{
		CImage* pImage = new CImage();
		LoadPngForRes(pImage, g_nOnlyRes[w]);
		if (!pImage->IsNull())
			m_mapIcons.insert(make_pair(g_nOnlyRes[w], pImage));
		else
			OnCancel();
	}

	//将资源与工具们关联
	for (short w = 0; w < emMax; ++w)
	{
		map<UINT, CImage*>::iterator itera = m_mapIcons.find(g_nFileNames[w]);
		if (itera == m_mapIcons.end())
		{
			OnCancel();
			return FALSE;
		}

		m_TooleBtn[w].m_pImage = itera->second;
	}

	//画画
	TrunPage(0, true);

	//定时器检测鼠标移出窗口
	SetTimer(emTimerOutOfWimCheck, 1000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

bool CCImageViewDlg::Init()
{
	m_bLoadSucess = true;
	m_nCurPicIndex = -1;
	m_nAllPicCount = 0;
	m_pCurImage = NULL;
	m_fCurPicProportion = 1.0;
	m_fUserProportion = 0.0;
	m_bShowThumbnail = false;
	m_fUserDragW = 0;
	m_fUserDragH = 0;
	m_bUseChangeProportion = false;
	m_bSizeFirst = true;

	return true;
}


void CCImageViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);

		if( (nID & 0xFFF0) == SC_MOVE )
			PostMessage(WM_NCLBUTTONUP, HTCAPTION, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCImageViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCImageViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CCImageViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性	
	return hbr;
}

LRESULT CCImageViewDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	UINT uRet = CDialog::OnNcHitTest(point);

	/*if (emBgImage != CheckItemPoint())
		return uRet;*/

	//如果拖动的是窗体,则返回标题栏
	return (HTCLIENT == uRet) && !m_bIsFullScreen && !m_bShowThumbnail ? HTCAPTION : uRet;
}

//显示缩略图
void CCImageViewDlg::ShowThumbnail(float fPosX, float fPosY, float fWidth, float fHeight)
{
	if (!m_bShowThumbnail || m_bUserCloseThumb)
	{
		m_TooleBtn[emThumbnail].m_bDisplayState = false;
		return;
	}

	//显示在右下角,固定大小
	if (m_TooleBtn[emThumbnail].m_pImage->IsNull())
		return;

	int nX = m_stLastDrawSize.cx - DEFAULT_THUMBNAUL_W - 3;
	int nY = m_stLastDrawSize.cy - DEFAULT_THUMBNAUL_H - 3;

	//先画上背景图
	DrawImage(m_TooleBtn[emThumbnail].m_pImage, nX, nY, DEFAULT_THUMBNAUL_W, DEFAULT_THUMBNAUL_H);

	//然后再画上缩略图,还得再缩小一遍.....
	if (m_pCurImage && !m_pCurImage->IsNull())
	{
		m_nCurThumbPosW = m_pCurImage->GetWidth();
		m_nCurThumbPosH = m_pCurImage->GetHeight();
		float shear = (float)m_nCurThumbPosW/(float)m_nCurThumbPosH;

		//按比较大的来
		if ((float(m_nCurThumbPosW) / DEFAULT_THUMBNAUL_W) >= (float(m_nCurThumbPosH) / DEFAULT_THUMBNAUL_H))
		{
			m_nCurThumbPosW = DEFAULT_THUMBNAUL_W;
			m_nCurThumbPosH = int(m_nCurThumbPosW / shear);
		}
		else
		{
			m_nCurThumbPosH = DEFAULT_THUMBNAUL_H;
			m_nCurThumbPosW = int(m_nCurThumbPosH * shear);
		}

		//计算位置
		m_nCurThumbPosX = (DEFAULT_THUMBNAUL_W - m_nCurThumbPosW) / 2 + nX;
		m_nCurThumbPosY = (DEFAULT_THUMBNAUL_H - m_nCurThumbPosH) / 2 + nY;

		//DWORD dwTTT = GetTickCount();

		/*m_TooleBtn[emThumbnail].m_curPoint.x = nThumbnailPosX;
		m_TooleBtn[emThumbnail].m_curPoint.y = nThumbnailPosY;
		m_TooleBtn[emThumbnail].m_wShowH = nH;
		m_TooleBtn[emThumbnail].m_wShowW = nW;*/

		m_TooleBtn[emThumbnail].m_curPoint.x = nX;
		m_TooleBtn[emThumbnail].m_curPoint.y = nY;
		m_TooleBtn[emThumbnail].m_wShowH = DEFAULT_THUMBNAUL_H;
		m_TooleBtn[emThumbnail].m_wShowW = DEFAULT_THUMBNAUL_W;

		DrawImage(m_pCurImage, m_nCurThumbPosX + 2, m_nCurThumbPosY + 2, m_nCurThumbPosW - 4, m_nCurThumbPosH - 4);

		m_TooleBtn[emThumbnail].m_bDisplayState = true;

		//缩略图上还有个小的框, 根据用户操作偏移量来确定位置, 根据缩放比确定大小
		//Image* pImage = m_bUseFullPic && m_pCurImageFull ? m_pCurImageFull : m_pCurImage;

		/*char szMsg[64] = {0};
		_snprintf_s(szMsg, sizeof(szMsg), "fWidth = %f, fHeight = %f\n", fWidth, fHeight);
		OutputDebugStringA(szMsg);*/

		m_fThunmbClockW = m_nCurThumbPosW * fWidth;
		m_fThunmbClockH = m_nCurThumbPosH * fHeight;

		if (!m_fThunmbClockX)
			//m_nThunmbClockX = (m_nCurThumbPosW - nRectangleW) / 2 + m_nCurThumbPosX;
			SetThunmbClockX((m_nCurThumbPosW - m_fThunmbClockW) / 2 + m_nCurThumbPosX);
		if (!m_fThunmbClockY)
			//m_nThunmbClockY = (m_nCurThumbPosH - nRectangleH) / 2 + m_nCurThumbPosY;
			SetThunmbClockY((m_nCurThumbPosH - m_fThunmbClockH) / 2 + m_nCurThumbPosY);

		//int nRectangleX = m_nThunmbClockX;//int(nThumbnailPosX + nW * fPosX);
		//int nRectangleY = m_nThunmbClockY;//int(nThumbnailPosY + nH * fPosY);

		//先贴一个底层
		m_TooleBtn[emThumRect].m_curPoint.x = (LONG)m_fThunmbClockX;
		m_TooleBtn[emThumRect].m_curPoint.y = (LONG)m_fThunmbClockY;
		m_TooleBtn[emThumRect].m_wShowW = (short)m_fThunmbClockW;
		m_TooleBtn[emThumRect].m_wShowH = (short)m_fThunmbClockH;
		m_TooleBtn[emThumRect].m_bDisplayState = true;
		DrawImage(m_TooleBtn[emThumRect].m_pImage, m_TooleBtn[emThumRect].m_curPoint.x, m_TooleBtn[emThumRect].m_curPoint.y, m_TooleBtn[emThumRect].w(), m_TooleBtn[emThumRect].h());

		m_pGh = Graphics::FromHDC(m_hdcMemory);
		Pen pen(Color(0, 0, 0), 0.5);
		//外层黑色
		m_pGh->DrawRectangle(&pen, m_fThunmbClockX, m_fThunmbClockY, m_fThunmbClockW, m_fThunmbClockH);
		//内层白色
		pen.SetColor(Color(255, 255, 255));
		m_pGh->DrawRectangle(&pen, m_fThunmbClockX + 1, m_fThunmbClockY + 1, m_fThunmbClockW - 2, m_fThunmbClockH - 2);

		//右上角有关闭按钮
		m_TooleBtn[emThumClose].m_bDisplayState = true;
		m_TooleBtn[emThumClose].m_curPoint.x = nX + DEFAULT_THUMBNAUL_W - 20;
		m_TooleBtn[emThumClose].m_curPoint.y = nY + 10;

		//画关闭按钮背景
		if (m_TooleBtn[emThumCloseBG].m_bDisplayState)
			DrawImage(m_TooleBtn[emThumCloseBG].m_pImage, m_TooleBtn[emThumClose].m_curPoint.x - 4, m_TooleBtn[emThumClose].m_curPoint.y - 2,\
			m_TooleBtn[emThumCloseBG].m_pImage->GetWidth() - 2, m_TooleBtn[emThumCloseBG].m_pImage->GetHeight() - 2);

		DrawImage(m_TooleBtn[emThumClose].m_pImage, m_TooleBtn[emThumClose].m_curPoint.x, m_TooleBtn[emThumClose].m_curPoint.y, \
			m_TooleBtn[emThumClose].m_pImage->GetWidth() * DEFAULT_THUM_CLOSE_PRO, m_TooleBtn[emThumClose].m_pImage->GetHeight() * DEFAULT_THUM_CLOSE_PRO);

	}
}
void CCImageViewDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnNcLButtonDblClk(nHitTest, point);
}

void CCImageViewDlg::OnCancel()
{
	m_cLoadThread.StopThread();

	m_bIsRuning = false;

	ClearRotateTemp();

	map<UINT, CImage*>::iterator itera = m_mapIcons.begin();
	for (;itera != m_mapIcons.end(); ++itera)
	{
		if (!itera->second->IsNull())
			itera->second->Destroy();

		delete itera->second;
		itera->second = NULL;
	}

	CDialog::OnCancel();

	Gdiplus::GdiplusShutdown(m_ldiplusToken);

	OutputDebugStringA("显示线程退出\n");
}

int CCImageViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}

void CCImageViewDlg::RePaintDialog(int nWidth /*= 0*/, int nHeight /*= 0*/, int nPosX /*= -1*/, int nPosY /*= -1*/)
{
	static POINT ptSrc={0,0};
	
	//更新位置
	GetWindowRect(&m_stRct);
	if (m_bIsFullScreen)
	{
		m_stLastDrawPoint.x = 0;
		m_stLastDrawPoint.y = 0;
	}
	else
	{
		m_stLastDrawPoint.x = nPosX != -1 ? nPosX : m_stRct.left;
		m_stLastDrawPoint.y = nPosY != -1 ? nPosY : m_stRct.top;		
	}

	m_hUpdateLayeredWindow( m_hWnd,::GetDC(m_hWnd),&m_stLastDrawPoint,&m_stLastDrawSize,m_hdcMemory,&ptSrc,255,&m_Blend,ULW_ALPHA);
}

//单纯的画图
bool CCImageViewDlg::DrawImage(CImage* pImage, int nPosX /*= 0*/, int nPosY /*= 0*/, int nWidth /*= 0*/, int nHeight /*= 0*/, int nXScr /*= 0*/, int nYSrc /*= 0*/, int nSrcWidth /*= 0*/, int nSrcHeight /*= 0*/)
{
	if (!pImage)
		return false;
	/*if (m_pGh->GetLastStatus())
	return false;*/

	/*m_pGh->DrawImage(pImage, nPosX, nPosY, nWidth, nHeight);
	if (m_pGh->GetLastStatus())
		return false;*/

	//GetDC()->SetStretchBltMode(COLORONCOLOR);

	nWidth = nWidth ? nWidth : pImage->GetWidth();
	nHeight = nHeight ? nHeight : pImage->GetHeight();

	nSrcWidth = nSrcWidth ? nSrcWidth : pImage->GetWidth();
	nSrcHeight = nSrcHeight ? nSrcHeight : pImage->GetHeight();

	pImage->AlphaBlend(m_hdcMemory, nPosX, nPosY, nWidth, nHeight, nXScr, nYSrc, nSrcWidth, nSrcHeight, 254);

	return true;
}

bool CCImageViewDlg::DrawAllImage()
{
	if (!m_bIsRuning)
		return false;

	//m_pGh->Clear(0);

	//CDC* pDC = GetDC();   

	//CPoint cp;
	//GetCursorPos( &cp );
	////////////////////////////////////////////////////////////////////////////
	//CDC BufferDC;
	//BufferDC.CreateCompatibleDC( pDC );
	//CBitmap cBitMap;
	//cBitMap.CreateCompatibleBitmap( pDC, m_stCurScreenSize.cx,m_stCurScreenSize.cy); 
	//BufferDC.SelectObject(&cBitMap);

	////pDC->SetBkMode(TRANSPARENT);

	///*CImage image;
	//image.Load(L"res\\BgImage.png");
	//image.Draw(m_hdcMemory, 0, 0, m_stLastDrawSize.cx, m_stLastDrawSize.cy);*/

	//CImage image1;
	//image1.Load(L"res\\BgImage.png");
	////image1.Draw(m_hdcMemory, 0, 0, image1.GetWidth(), image1.GetHeight());
	//image1.AlphaBlend(m_hdcMemory, 0, 0, image1.GetWidth(), image1.GetHeight(), 0, 0, image1.GetWidth(), image1.GetHeight(), SRCCOPY);
	////image1.BitBlt(m_hdcMemory, 0, 0, image1.GetWidth(), image1.GetHeight(), 0, 0, SRCAND);

	////pDC->BitBlt(0,0,m_stCurScreenSize.cx,m_stCurScreenSize.cy,&BufferDC,0,0,SRCCOPY);

	////////////////////////////////////////////////////////////////////////////
	////清理资源
	//BufferDC.DeleteDC(); 
	//cBitMap.DeleteObject();
	//ReleaseDC(pDC);  
	//ReleaseDC(&BufferDC);  

	//return true;

	//绘制内存位图
	DeleteDC(m_hdcMemory);
	DeleteDC(m_hdcTmp);

	m_hdcTmp=GetDC()->m_hDC;
	m_hdcMemory=CreateCompatibleDC(m_hdcTmp);
	HBITMAP hBitMap=CreateCompatibleBitmap(m_hdcTmp, m_stCurScreenSize.cx, m_stCurScreenSize.cy);
	SelectObject(m_hdcMemory,hBitMap);
	//m_pGh = Graphics::FromHDC(m_hdcMemory);

	//先画背景
	DrawBackGroundImage();

	//画缩略图
	if (m_pCurImage && !m_pCurImage->IsNull())
	{
		if (m_bIsFullScreen)
		{
			m_stLastDrawSize.cx = m_stCurScreenSize.cx;
			m_stLastDrawSize.cy = m_stCurScreenSize.cy;
		}

		if (m_stLastDrawSize.cx <= 0 || m_stLastDrawSize.cy <= 0)
			return false;

		CImage *pImage = m_pCurImage;

		//计算尺寸
		int nW = int(pImage->GetWidth());
		int nH = int(pImage->GetHeight());

		//计算位置
		int nThumbnailPosX = (m_stLastDrawSize.cx - nW) / 2;
		int nThumbnailPosY = (m_stLastDrawSize.cy - nH) / 2;

		//重新调整大小,如果用户调整了比例就不重新计算了
		if (m_bUseChangeProportion)
		{
			//尺寸和位置都要重新调整
			nW = int(nW * (m_fCurPicProportion + m_fUserProportion));
			nH = int(nH * (m_fCurPicProportion + m_fUserProportion));

			nThumbnailPosX = (m_stLastDrawSize.cx - nW) / 2;
			nThumbnailPosY = (m_stLastDrawSize.cy - nH) / 2;
		}
		else if ((nW > m_stLastDrawSize.cx || nH > m_stLastDrawSize.cy))
		{
			float shear = (float)nW/(float)nH;

			//按比较大的来
			if ((float(nW) / m_stLastDrawSize.cx) >= (float(nH) / m_stLastDrawSize.cy))
			{
				m_fCurPicProportion = (float)m_stLastDrawSize.cx / (float)nW;

				nW = m_stLastDrawSize.cx;
				nH = int(nW / shear);
			}
			else
			{
				m_fCurPicProportion = (float)m_stLastDrawSize.cy / (float)nH;

				nH = m_stLastDrawSize.cy;
				nW = int(nH * shear);
			}

			nThumbnailPosX = (m_stLastDrawSize.cx - nW) / 2;
			nThumbnailPosY = (m_stLastDrawSize.cy - nH) / 2;
		}
		else
		{
			nThumbnailPosX = (m_stLastDrawSize.cx - nW) / 2;
			nThumbnailPosY = (m_stLastDrawSize.cy - nH) / 2;

			m_fCurPicProportion = 1.0;
		}

		if (nW > m_stLastDrawSize.cx || nH > m_stLastDrawSize.cy)
			m_bShowThumbnail = true;
		else
			m_bShowThumbnail = false;

		//DWORD dwA = GetTickCount();

		if (m_bShowThumbnail)
		{

			//这个是画布范围
			int nDrawX = (nThumbnailPosX > m_stLastDrawSize.cx || nThumbnailPosX < 0) ? 0 : nThumbnailPosX;
			int nDrawY = (nThumbnailPosY > m_stLastDrawSize.cy || nThumbnailPosY < 0) ? 0 : nThumbnailPosY;
			int nDrawW = nW > m_stLastDrawSize.cx ? m_stLastDrawSize.cx : nW;
			int nDrawH = nH > m_stLastDrawSize.cy ? m_stLastDrawSize.cy : nH;

			//Rect stDrawRect(nDrawX, nDrawY, nDrawW, nDrawH);

			//拉回处理, 不要问我为什么这么算....我已经晕了....
			//float nLimitW = (nW - nDrawW) / 2 * (m_fCurPicProportion + m_fUserProportion);
			//if (m_fUserDragW < -nLimitW)
			//{
			//	//m_fUserDragW = -nLimitW;
			//	//SetUserDragW(-nLimitW);
			//}
			//else if (m_fUserDragW > (nW - nDrawW - nLimitW))
			//{
			//	//m_fUserDragW = (nW - nDrawW - nLimitW);
			//	//SetUserDragW(nW - nDrawW - nLimitW);
			//}

			//float nLimitH = (nH - nDrawH) / 2 * (m_fCurPicProportion + m_fUserProportion);
			//if (m_fUserDragH < -nLimitH)
			//{
			//	//m_fUserDragH = -nLimitH;
			//	//SetUserDragH(-nLimitH);
			//}
			//else if (m_fUserDragH > (nH - nDrawH - nLimitH))
			//{
			//	//m_fUserDragH = (nH - nDrawH - nLimitH);
			//	//SetUserDragH(nH - nDrawH - nLimitH);
			//}

			/*char sz[64] = {0};
			_snprintf_s(sz, sizeof(sz), "m_nUserDragW = %d, nDrawW = %d, nW = %d \n", m_nUserDragW, nDrawW, nW);
			OutputDebugStringA(sz);*/

			//这个是截图范围
			int nInterceptX = int(m_fUserDragW / (m_fCurPicProportion + m_fUserProportion));// + float(nW - m_stLastDrawSize.cx) / 2 + nDrawX);
			int nInterceptY = int(m_fUserDragH / (m_fCurPicProportion + m_fUserProportion));// + float(nH - m_stLastDrawSize.cy) / 2 + nDrawY);
			int nInterceptW = int(nDrawW / (m_fCurPicProportion + m_fUserProportion));
			int nInterceptH = int(nDrawH / (m_fCurPicProportion + m_fUserProportion));

			//把图画在st 范围内! 截取原图的nInterceptX, nInterceptY, nInterceptW, nInterceptH 部分去画!
			//m_pGh->DrawImage(pImage, stDrawRect, nInterceptX, nInterceptY, nInterceptW, nInterceptH, UnitPixel);
			//pImage->BitBlt(m_hdcMemory, nDrawX, nDrawY, nDrawW, nDrawH, nInterceptX, nInterceptY, SRCPAINT);

			//CImage* imageE = new CImage;
			//imageE->Create(nInterceptW, nInterceptH, pImage->GetBPP());

			//CPoint points[3];
			//points[0] = CPoint(nInterceptX, nInterceptY);				//左上角
			//points[1] = CPoint(nInterceptX + nInterceptW, nInterceptY);	//右上角
			//points[2] = CPoint(nInterceptX, nInterceptY + nInterceptH);	//左下角
			//imageE.PlgBlt(pImage->GetDC(), points);

			//imageE.BitBlt(pImage->GetDC(),nInterceptX, nInterceptY, nInterceptW, nInterceptH, 0, 0); 

			//ImageCopy(pImage, imageE, nInterceptX, nInterceptY, nInterceptW, nInterceptH);

			//imageE.BitBlt(m_hdcMemory, nDrawX, nDrawY, nDrawW, nDrawH, 0, 0);
			
			//SetStretchBltMode(m_hdcMemory,HALFTONE);

			//pImage->StretchBlt(m_hdcMemory, nDrawX, nDrawY, nDrawW, nDrawH, nInterceptX, nInterceptY, nInterceptW, nInterceptH, SRCCOPY);

			DrawImage(pImage, nDrawX + 2, nDrawY + 2, nDrawW - 4, nDrawH - 4, nInterceptX, nInterceptY, nInterceptW, nInterceptH);

			/*imageE->Destroy();

			delete imageE;
			imageE = NULL;*/

			//缩略图
			ShowThumbnail(((float)nInterceptX / (float)pImage->GetWidth()), (float)nInterceptY / (float)pImage->GetHeight(), (float)nInterceptW / (float)pImage->GetWidth(), (float)nInterceptH / (float)pImage->GetHeight());
		}
		else
		{
			//把整幅图画到nThumbnailPosX, nThumbnailPosY, nW, nH 范围内!
			DrawImage(pImage, nThumbnailPosX + 2, nThumbnailPosY + 2, nW - 4, nH - 4);

			/*SetStretchBltMode(m_hdcTmp,COLORONCOLOR);
			pImage->StretchBlt(m_hdcTmp, nThumbnailPosX, nThumbnailPosY, nW, nH, \
			0, 0, pImage->GetWidth(), pImage->GetHeight(), SRCCOPY);*/

			//pImage->Draw()

			//DrawImage(m_TooleBtn[emBgImage].m_pImage, 0, 0, m_stLastDrawSize.cx, m_stLastDrawSize.cy);
		}

		/*char sz[64] = {0};
		_snprintf_s(sz, sizeof(sz), "DrawImage time = %d\n", GetTickCount() - dwA);
		OutputDebugStringA(sz);*/

		//恢复标志位
		m_bCurImageChange = false;
	}

	//画工具栏
	DrawToole();

	//画翻页
	DrawTurnPageBtn();

	//画比例图
	ShowThumnProportion();

	//关闭按钮位置
	m_TooleBtn[emCloseButton].m_curPoint.x = m_stLastDrawSize.cx - m_TooleBtn[emCloseButton].m_pImage->GetWidth() - 2;
	m_TooleBtn[emCloseButton].m_curPoint.y = 0 + 6;
	m_TooleBtn[emCloseButton].m_bDisplayState = true;

	//关闭按钮背景
	if (m_TooleBtn[emCloseBG].m_bDisplayState)
		DrawImage(m_TooleBtn[emCloseBG].m_pImage, m_TooleBtn[emCloseButton].m_curPoint.x - 4, m_TooleBtn[emCloseButton].m_curPoint.y - 3,\
		m_TooleBtn[emCloseBG].m_pImage->GetWidth(), m_TooleBtn[emCloseBG].m_pImage->GetHeight());

	//关闭按钮
	DrawImage(m_TooleBtn[emCloseButton].m_pImage, m_TooleBtn[emCloseButton].m_curPoint.x, m_TooleBtn[emCloseButton].m_curPoint.y,\
		m_TooleBtn[emCloseButton].m_pImage->GetWidth(), m_TooleBtn[emCloseButton].m_pImage->GetHeight());

	//增加关闭按钮的检测范围
	m_TooleBtn[emCloseButton].m_curPoint.x -= 10;
	m_TooleBtn[emCloseButton].m_curPoint.y += 10;
	m_TooleBtn[emCloseButton].m_wShowW = m_TooleBtn[emCloseButton].m_pImage->GetWidth() + 10;
	m_TooleBtn[emCloseButton].m_wShowH = m_TooleBtn[emCloseButton].m_pImage->GetHeight() + 10;

	//最后画这个估计没问题
	//ShowPicCountTips(NULL);

	//最后的右键菜单
	ShowLBtnMenu();

	DeleteObject(hBitMap);

	return true;
}

//画背景
bool CCImageViewDlg::DrawBackGroundImage(int nWidth /*= 0*/, int nHeige /*= 0*/)
{
	//画背景前先清空
	//m_pGh->Clear(0);
	//Invalidate();

	//SetStretchBltMode(m_hdcTmp,HALFTONE);

	//背景
	if (m_TooleBtn[emBgImage].m_pImage->IsNull())
		return false;

	DrawImage(m_TooleBtn[emBgImage].m_pImage, 0, 0, m_stLastDrawSize.cx, m_stLastDrawSize.cy);

	//m_TooleBtn[emBgImage].m_pImage->AlphaBlend(m_hdcMemory, 0,0,m_stLastDrawSize.cx, m_stLastDrawSize.cy, \
	//	0,0,m_TooleBtn[emBgImage].m_pImage->GetWidth(), m_TooleBtn[emBgImage].m_pImage->GetHeight());//, AC_SRC_ALPHA);

	return true;
}

BOOL CCImageViewDlg::PreTranslateMessage(MSG* pMsg)
{
	static DWORD dwBeginTime;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		OnRBtnKeyDown(pMsg);
		break;
	case WM_NCLBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			SetCur();

			//鼠标按下计时
			dwBeginTime = GetTickCount();
			m_bLeftButtonState = true;

			//更新鼠标位置
			m_cDragTmp.x = GET_X_LPARAM(pMsg->lParam);
			m_cDragTmp.y = GET_Y_LPARAM(pMsg->lParam);
		}
		break;
	case WM_NCLBUTTONUP:
	case WM_LBUTTONUP:
		{
			SetCur();

			ToolButtunID emID = OnClickItem();

			m_bLeftButtonState = false;

			if (emBgImage == emID && GetTickCount() - dwBeginTime < DEFAULT_LBUTTON_CLICK)
				OnCancel();

			m_TooleBtn[emRBtnMenu].m_bDisplayState = false;
			//立刻重刷
			if(DrawAllImage())
				RePaintDialog();

			m_bIsDraging = false;
			m_bIsDragThumb = false;
			//OutMsg(L"鼠标抬起,取消拖动");
		}
		break;
	case WM_NCMOUSEMOVE:
		{
			SetCur();

			CheckRePaintDialog();

			ShowPicCountTips(pMsg);
		}
		break;
	//case WM_LBUTTONUP:
	//	{
	//		SetCur();

	//		m_bLeftButtonState = false;

	//		m_bIsDraging = false;
	//		m_bIsDragThumb = false;

	//		m_TooleBtn[emRBtnMenu].m_bDisplayState = false;
	//		//立刻重刷
	//		if(DrawAllImage())
	//			RePaintDialog();
	//	}
	//	break;
	//case WM_LBUTTONDOWN:
	//	{
	//		SetCur();

	//		OnClickItem();

	//		m_bLeftButtonState = true;
	//		//更新鼠标位置
	//		m_cDragTmp.x = GET_X_LPARAM(pMsg->lParam);
	//		m_cDragTmp.y = GET_Y_LPARAM(pMsg->lParam);
	//	}
	//	break;
	case WM_RBUTTONDOWN:
		{
			m_stLastRBtnPos.x = GET_X_LPARAM(pMsg->lParam);
			m_stLastRBtnPos.y = GET_Y_LPARAM(pMsg->lParam);

			m_TooleBtn[emRBtnMenu].m_bDisplayState = true;

			//立刻重刷
			if(DrawAllImage())
				RePaintDialog();
		}
		break;
	case WM_NCRBUTTONDOWN:
		{
			m_stLastRBtnPos.x = GET_X_LPARAM(pMsg->lParam) - m_stLastDrawPoint.x;
			m_stLastRBtnPos.y = GET_Y_LPARAM(pMsg->lParam) - m_stLastDrawPoint.y;

			m_TooleBtn[emRBtnMenu].m_bDisplayState = true;

			//立刻重刷
			if(DrawAllImage())
				RePaintDialog();
		}
		break;
	case WM_MOUSEMOVE:
		{
			SetCur();

			CheckRePaintDialog();
			ShowPicCountTips(pMsg);

			//全屏或者放大超过窗口尺寸后,非客户区收不到任何消息了
			//现在内容转变为客户区了,所以全屏后的拖动就在这里
			if ((int)(pMsg->wParam << 31) >= 0 || !m_bShowThumbnail)
				break;

			if (m_bIsDragThumb)
				OutMsg(L"正在拖动缩略图");

			if (m_bIsDraging)
				OutMsg(L"正在拖动图片");

			if (!m_bIsDragThumb && !m_bIsDraging)
			{
				switch (CheckItemPoint())
				{
				case emBgImage:
					m_bIsDraging = true;	
					break;
				case emThumRect:
					m_bIsDragThumb = true;
					break;
				default:
					break;
				}
			}

			if (m_bIsDragThumb)
				DragThumb(pMsg->lParam);

			if (m_bIsDraging)
				DragImage(pMsg->lParam);

			//更新鼠标位置
			m_cDragTmp.x = GET_X_LPARAM(pMsg->lParam);
			m_cDragTmp.y = GET_Y_LPARAM(pMsg->lParam);

			//立刻重刷
			if(DrawAllImage())
				RePaintDialog();

			/*static DWORD dwBTime = GetTickCount();
			static DWORD dwCount = 0;
			dwCount++;

			char szMsg[64] = {0};
			_snprintf_s(szMsg, sizeof(szMsg), "X= %d, Y = %d \n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OutputDebugStringA(szMsg);

			if (GetTickCount() - dwBTime >= 1000)
			{
			char szMsg[64] = {0};
			_snprintf_s(szMsg, sizeof(szMsg), "每秒触发次数 = %d \n", dwCount);
			OutputDebugStringA(szMsg);

			dwCount = 0;
			dwBTime = GetTickCount();
			}

			break;*/


			

			//保存当前鼠标的相对位置
			/*m_cPoint.x = LOWORD(lParam);
			m_cPoint.y = HIWORD(lParam);*/

			//增加定时器
			//SetTimer(2, 20, NULL);

			/*CPoint cPoint;
			GetCursorPos(&cPoint);*/

			

			//更新数据
			//m_cCurPoint = cPoint;

			/*char szMsg[64] = {0};
			_snprintf_s(szMsg, sizeof(szMsg), "偏移量X = %d, 偏移量Y = %d\n", m_nUserDragH, m_nUserDragW);
			OutputDebugStringA(szMsg);*/
		}
		break;
	case WM_MOUSEWHEEL:
		{
			short wState = HIWORD(pMsg->wParam);
			if (wState > 0)
				ChangeArea(1);
			else
				ChangeArea(-1);

			m_bOriginalSize = false;
		}
		break;
	default:
		{
			/*char sz[64] = {0};
			_snprintf_s(sz, sizeof(sz), "command = %d\n", pMsg->message);
			OutputDebugStringA(sz);*/
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//检查需要重绘的组件
bool CCImageViewDlg::CheckReDraw()
{
	//检查是不是有变化
	bool bTurnState = false;
	ToolButtunID emID = CheckItemPoint();

	//判断翻页按钮是不是要显示,左侧右侧都要显示
	if (m_cCurPoint.x >= 0 && m_cCurPoint.y >= 0)
	{
		//左侧
		if (((UINT)m_cCurPoint.x <= m_TooleBtn[emBtnTurnPage].w()) && (m_cCurPoint.y > 0 && m_cCurPoint.y <= m_stLastDrawSize.cy))
		{
			bTurnState = true;
			m_bIsLeft = true;
		}
		//右侧
		else if ((m_cCurPoint.x <= m_stLastDrawSize.cx) && ((UINT)m_cCurPoint.x > (m_stLastDrawSize.cx - m_TooleBtn[emBtnTurnPage].w()))
			&& (m_cCurPoint.y > 0 && m_cCurPoint.y <= m_stLastDrawSize.cy))
		{
			bTurnState = true;
			m_bIsLeft = false;
		}
	}
	
	if (bTurnState != m_TooleBtn[emBtnTurnPage].m_bDisplayState)
	{
		//有变化,重绘
		m_TooleBtn[emBtnTurnPage].m_bDisplayState = bTurnState;

		bTurnState = true;
	}
	else
		bTurnState = false;

	//工具栏状态
	bool bToolState = false;
	//检查工具栏,只要鼠标在窗口内就显示
	if (m_cCurPoint.x >= 0 && m_cCurPoint.x <= m_stLastDrawSize.cx && m_cCurPoint.y >= 0 && m_cCurPoint.y <= m_stLastDrawSize.cy)
		bToolState = true;
	
	if (bToolState != m_TooleBtn[emToolButtons].m_bDisplayState)
	{
		m_TooleBtn[emToolButtons].m_bDisplayState = bToolState;

		bToolState = true;
	}
	else
		bToolState = false;

	//如果在工具栏范围内还要检查按钮
	bool bBottomState = false;
	if (m_TooleBtn[emToolButtons].m_bDisplayState)
	{
		BottomBtnID emBtnID = CheckBottomItem();

		if (m_emLastClickButton != emBtnID)
		{
			bBottomState = true;
			m_emLastClickButton = emBtnID;
		}
	}

	//检测关闭按钮
	bool bCloseBG = false;
	if (emCloseButton == emID)
	{
		if (!m_TooleBtn[emCloseBG].m_bDisplayState)
		{
			bCloseBG = true;
			m_TooleBtn[emCloseBG].m_bDisplayState = true;
		}
	}
	else
	{
		if (m_TooleBtn[emCloseBG].m_bDisplayState)
		{
			bCloseBG = true;
			m_TooleBtn[emCloseBG].m_bDisplayState = false;
		}
	}

	//检测缩略图关闭按钮   
	bool bThumCloseBG = false;
	if (emThumClose == emID)
	{
		if (!m_TooleBtn[emThumCloseBG].m_bDisplayState)
		{
			bThumCloseBG = true;
			m_TooleBtn[emThumCloseBG].m_bDisplayState = true;
		}
	}
	else
	{
		if (m_TooleBtn[emThumCloseBG].m_bDisplayState)
		{
			bThumCloseBG = true;
			m_TooleBtn[emThumCloseBG].m_bDisplayState = false;
		}
	}

	//检测数量提示
	bool bPicCountTips = false;
	if (emBtnTurnPage == emID){
		if (!m_bShowTips){
			bPicCountTips = true;
			m_bShowTips = true;
		}
	}
	else
	{
		if (m_bShowTips)
		{
			bPicCountTips = true;
			m_bShowTips = false;
		}
	}

	//检查右键菜单
	bool bRBtnMenu = false;
	if (emRBtnMenu == emID && CheckRBtnMenuItem() != m_emLastMRtnIndex)
		bRBtnMenu = true;
	else if (emRBtnMenu != emID && emRBtnMax != m_emLastMRtnIndex)
		bRBtnMenu = true;

	return bToolState || bTurnState || bBottomState || bCloseBG || bThumCloseBG || bPicCountTips || bRBtnMenu;
}

//画工具栏
bool CCImageViewDlg::DrawToole()
{
	if (!m_TooleBtn[emToolButtons].m_bDisplayState)
		return true;

	m_TooleBtn[emToolButtons].m_curPoint.x = (m_stLastDrawSize.cx - m_TooleBtn[emToolButtons].m_pImage->GetWidth()) / 2;
	m_TooleBtn[emToolButtons].m_curPoint.y = m_stLastDrawSize.cy - m_TooleBtn[emToolButtons].m_pImage->GetHeight();
	m_TooleBtn[emToolButtons].m_bDisplayState = true;

	DrawImage(m_TooleBtn[emToolButtons].m_pImage, m_TooleBtn[emToolButtons].m_curPoint.x, m_TooleBtn[emToolButtons].m_curPoint.y,\
		m_TooleBtn[emToolButtons].m_pImage->GetWidth(), m_TooleBtn[emToolButtons].m_pImage->GetHeight());

	if ((m_emLastClickButton != emBottomMax) && !m_TooleBtn[emBgImage].m_pImage->IsNull() && (m_emLastClickButton != emReduction || !m_bOriginalSize))
	{
		//高亮
		DrawImage(m_TooleBtn[emToolButtonTips].m_pImage,
			m_TooleBtn[emToolButtons].m_curPoint.x + g_stTooltBtnPoints[m_emLastClickButton].left,\
			m_TooleBtn[emToolButtons].m_curPoint.y + g_stTooltBtnPoints[m_emLastClickButton].top,\
			g_stTooltBtnPoints[m_emLastClickButton].right - g_stTooltBtnPoints[m_emLastClickButton].left,\
			g_stTooltBtnPoints[m_emLastClickButton].bottom - g_stTooltBtnPoints[m_emLastClickButton].top,\
			g_stTooltBtnPoints[m_emLastClickButton].left, g_stTooltBtnPoints[m_emLastClickButton].top,\
			g_stTooltBtnPoints[m_emLastClickButton].right - g_stTooltBtnPoints[m_emLastClickButton].left,\
			g_stTooltBtnPoints[m_emLastClickButton].bottom - g_stTooltBtnPoints[m_emLastClickButton].top);
	}
	
	if (m_bOriginalSize)
	{
		//1:1特殊灰化
		DrawImage(m_TooleBtn[emToolButtonTips].m_pImage,
			m_TooleBtn[emToolButtons].m_curPoint.x + g_stTooltBtnPoints[emReduction].left,\
			m_TooleBtn[emToolButtons].m_curPoint.y + g_stTooltBtnPoints[emReduction].top,\
			g_stTooltBtnPoints[emReduction].right - g_stTooltBtnPoints[emReduction].left,\
			g_stTooltBtnPoints[emReduction].bottom - g_stTooltBtnPoints[emReduction].top,\
			g_stTooltBtnPoints[emBottomMax].left, g_stTooltBtnPoints[emBottomMax].top,\
			g_stTooltBtnPoints[emBottomMax].right - g_stTooltBtnPoints[emBottomMax].left,\
			g_stTooltBtnPoints[emBottomMax].bottom - g_stTooltBtnPoints[emBottomMax].top);
	}

	return true;
}

//画翻页按钮
bool CCImageViewDlg::DrawTurnPageBtn()
{
	if (!m_TooleBtn[emBtnTurnPage].m_bDisplayState)
		return true;

	//不需要显示的时候并且也没显示
	if (!m_bShowTrunPage && emDontShow == m_TooleBtn[emBtnTurnPage].m_bDisplayState)
		return true;

	//在右边的时候要计算X
	m_TooleBtn[emBtnTurnPage].m_curPoint.x = m_bIsLeft ? 0 : m_stLastDrawSize.cx - m_TooleBtn[emBtnTurnPage].w();
	m_TooleBtn[emBtnTurnPage].m_curPoint.y = m_stLastDrawSize.cy / 2 - m_TooleBtn[emBtnTurnPage].m_pImage->GetHeight() / 2;

	CImage* pImage = m_TooleBtn[emBtnTurnPage].m_pImage;
	if (!m_bIsLeft)
	{
		//翻转图像
		pImage = new CImage;
		ImageHorizontalFlip(pImage, m_TooleBtn[emBtnTurnPage].m_pImage);
	}

	DrawImage(pImage, m_TooleBtn[emBtnTurnPage].m_curPoint.x, m_TooleBtn[emBtnTurnPage].m_curPoint.y, m_TooleBtn[emBtnTurnPage].m_pImage->GetWidth(), m_TooleBtn[emBtnTurnPage].m_pImage->GetHeight());

	m_TooleBtn[emBtnTurnPage].m_bDisplayState = emNormal;

	if (!m_bIsLeft)
	{
		if (!pImage->IsNull())
		{
			pImage->ReleaseDC();
			pImage->Destroy();
		}

		delete pImage;
		pImage = NULL;
	}

	return true;
}

//切换图片
bool CCImageViewDlg::TrunPage(char cIsAdd/* = 0*/, bool bInit/* = false*/)
{
	/*delete m_pCurImage;
	m_pCurImage = NULL;*/
	if (!cIsAdd)
		m_nCurPicIndex = -1;
	else if (m_nCurPicIndex + cIsAdd < 0)
		return false;
	else if (0 != m_nAllPicCount && m_nCurPicIndex + cIsAdd >= m_nAllPicCount)
		return false;

	int nTmpIndex = m_nCurPicIndex;

	m_nCurPicIndex += cIsAdd;
	m_bLoadSucess = true;

	ClearRotateTemp();
	
	SIZE stSize;
	SIZE *pSize = m_bSizeFirst ? &stSize : NULL;
	int *pAllCount = m_nAllPicCount ? NULL : &m_nAllPicCount;
	if (!m_cLoadThread.GetImageFromList(m_pCurImage, m_nCurPicIndex, pSize, pAllCount))
	{
		m_pCurImage = m_TooleBtn[emTipsImage].m_pImage;

		if (m_bSizeFirst)
		{
			m_stLastDrawSize.cx = DEFAULT_WINDOW_SIZE_W;
			m_stLastDrawSize.cy = DEFAULT_WINDOW_SIZE_H;

			if (DrawAllImage())
				RePaintDialog(0, 0, (m_stCurScreenSize.cx / 2) - (m_stLastDrawSize.cx / 2), (m_stCurScreenSize.cy / 2) - (m_stLastDrawSize.cy / 2));
		}

		m_bLoadSucess = false;
		bInit = false;
	}
	else if (!cIsAdd || nTmpIndex != m_nCurPicIndex)
	{
		if (!m_nAllPicCount)
			OnCancel();

		//标记图片已经切换需重绘
		m_bCurImageChange = true;
		//重置缩放比
		m_fUserProportion = 0.0;
		//m_fDIsplayProportion = 1.0;
		m_bUseChangeProportion = false;

		//清空上一张原图
		//m_pCurImageFull = NULL;
		//m_bUseFullPic = false;

		m_bOriginalSize = false;
		m_bUserCloseThumb = false;

		//m_bShowTips = true;

		m_cRotateNum = 0;

		m_fUserDragW = 0;
		m_fUserDragH = 0;

		m_fThunmbClockX = 0;
		m_fThunmbClockY = 0;
		m_fThunmbClockW = 0;
		m_fThunmbClockH = 0;

		m_nCurThumbPosX = 0;
		m_nCurThumbPosY = 0;
		m_nCurThumbPosW = 0;
		m_nCurThumbPosH = 0;


		if (m_bSizeFirst && pSize)
		{
			m_stLastDrawSize.cx = pSize->cx;
			m_stLastDrawSize.cy = pSize->cy;

			m_bSizeFirst = false;

			::MoveWindow(m_hWnd, (m_stCurScreenSize.cx / 2) - (m_stLastDrawSize.cx / 2), (m_stCurScreenSize.cy / 2) - (m_stLastDrawSize.cy / 2), m_stLastDrawSize.cx, m_stLastDrawSize.cy, FALSE);
		}

		if (DrawAllImage())
			RePaintDialog(/*0, 0, (m_stCurScreenSize.cx / 2) - (m_stLastDrawSize.cx / 2), (m_stCurScreenSize.cy / 2) - (m_stLastDrawSize.cy / 2)*/);

		//更新翻页显示
		m_cTip.DelTool(this);
		ShowPicCountTips(NULL);
	}

	return true;
}
void CCImageViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case emTimerOutOfWimCheck:
		{
			//检测是否在其他窗口之下,如果是在其他窗口之下,就主动关闭
			

			//检测重绘
			CheckRePaintDialog();
		}
		break;
	case emTimerProportionBG:
		{
			m_bProportionState = false;
			if (DrawAllImage())
				RePaintDialog();

			//删除定时器
			KillTimer(emTimerProportionBG);
		}
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//检测当前位置	
ToolButtunID CCImageViewDlg::CheckItemPoint()
{
	//更新鼠标位置
	UpdateCurPoint();

	for (short w = 0; w < emMax; ++w)
	{
		/*if (w == emThumbnail)
		{
			OutMsg(L"鼠标坐标x=%d,y=%d", m_cCurPoint.x, m_cCurPoint.y);
			OutMsg(L"缩略图范围x=[%d-%d],y=[%d-%d]", m_TooleBtn[w].m_curPoint.x, m_TooleBtn[w].m_curPoint.x + m_TooleBtn[w].w(), m_TooleBtn[w].m_curPoint.y, m_TooleBtn[w].m_curPoint.y + m_TooleBtn[w].h());
		}*/

		if (m_TooleBtn[w].m_curPoint.x < m_cCurPoint.x && (UINT)m_cCurPoint.x < (m_TooleBtn[w].m_curPoint.x + m_TooleBtn[w].w())\
			&&m_TooleBtn[w].m_curPoint.y < m_cCurPoint.y && (UINT)m_cCurPoint.y < (m_TooleBtn[w].m_curPoint.y + m_TooleBtn[w].h())
			&&m_TooleBtn[w].m_bDisplayState)
		{
			return (ToolButtunID)w;
		}
	}

	return emBgImage;
}

BottomBtnID CCImageViewDlg::CheckBottomItem()
{
	//更新鼠标位置
	UpdateCurPoint();

	for (short w = 0; w < emBottomMax; ++w)
	{
		if ((m_TooleBtn[emToolButtons].m_curPoint.x + g_stTooltBtnPoints[w].left) <= m_cCurPoint.x\
			&& m_cCurPoint.x <= (m_TooleBtn[emToolButtons].m_curPoint.x + g_stTooltBtnPoints[w].right)\
			&& (m_TooleBtn[emToolButtons].m_curPoint.y + g_stTooltBtnPoints[w].top) <= m_cCurPoint.y\
			&& m_cCurPoint.y <= (m_TooleBtn[emToolButtons].m_curPoint.y + g_stTooltBtnPoints[w].bottom))
		{
			return (BottomBtnID)w;
		}
	}

	return emBottomMax;
}

RMenuBtnID CCImageViewDlg::CheckRBtnMenuItem()
{
	//更新鼠标位置
	UpdateCurPoint();

	for (short w = 0; w < emRBtnMax; ++w)
	{
		if ((m_TooleBtn[emRBtnMenu].m_curPoint.x + g_stLBtnMenuPoints[w].left) <= m_cCurPoint.x\
			&& m_cCurPoint.x <= (m_TooleBtn[emRBtnMenu].m_curPoint.x + g_stLBtnMenuPoints[w].right)\
			&& (m_TooleBtn[emRBtnMenu].m_curPoint.y + g_stLBtnMenuPoints[w].top) <= m_cCurPoint.y\
			&& m_cCurPoint.y <= (m_TooleBtn[emRBtnMenu].m_curPoint.y + g_stLBtnMenuPoints[w].bottom))
		{
			return (RMenuBtnID)w;
		}
	}

	return emRBtnMax;
}

//单击响应
ToolButtunID CCImageViewDlg::OnClickItem()
{
	ToolButtunID emID = CheckItemPoint();

	/*char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "id = %d, time = %d\n", emID, dwTickCount);
	OutputDebugStringA(szMsg);*/

	switch (emID)
	{
	case emBgImage:		//背景图片
		break;
	case emBtnTurnPage:	//翻页按钮
		{
			char cIsAdd = m_TooleBtn[emBtnTurnPage].m_curPoint.x ? 1: -1;
			TrunPage(cIsAdd);
		}
		break;
	case emToolButtons:
		OnClickBottom();
		break;
	case emCloseButton:
		OnCancel();
		break;
	case emThumbnail:
		//OutputDebugStringA("点击缩略图\n");
		break;
	case emThumClose:
		{
			m_bUserCloseThumb = true;

			if (DrawAllImage())
				RePaintDialog();
		}
		break;
	case emRBtnMenu:
		OnClickRBtnMenuItem();
		break;
	default:
		break;
	}

	return emID;
}

BottomBtnID CCImageViewDlg::OnClickBottom()
{
	/*char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "buttonID = %d \n", (int)CheckBottomItem());
	OutputDebugStringA(szMsg);*/

	switch (m_emLastClickButton)
	{
	case emReduction:			//1:1
		{
			if (m_pCurImage->IsNull() || m_bOriginalSize)
				break;

			int nW = m_pCurImage->GetWidth();
			int nH = m_pCurImage->GetHeight();

			m_fCurPicProportion = 1.0;
			m_fUserProportion = 0.0;
			m_bUseChangeProportion = true;

			//加载成功了,确定窗口大小...
			m_stLastDrawSize.cx = nW < m_stCurScreenSize.cx ? nW : m_stCurScreenSize.cx;
			m_stLastDrawSize.cy = nH < m_stCurScreenSize.cy ? nH : m_stCurScreenSize.cy;

			m_stLastDrawSize.cx = m_stLastDrawSize.cx > DEFAULT_WINDOW_SIZE_W ? m_stLastDrawSize.cx : DEFAULT_WINDOW_SIZE_W;
			m_stLastDrawSize.cy = m_stLastDrawSize.cy > DEFAULT_WINDOW_SIZE_H ? m_stLastDrawSize.cy : DEFAULT_WINDOW_SIZE_H;

			//修改窗口大小
			//::MoveWindow(m_hWnd, 0, 0, m_stCurScreenSize.cx, m_stCurScreenSize.cy, FALSE);

			//重新计算位置...并且刷新窗口
			int nX = (m_stCurScreenSize.cx - m_stLastDrawSize.cx) / 2;
			int nY = (m_stCurScreenSize.cy - m_stLastDrawSize.cy) / 2;

			//重新计算图片位置
			SetUserDragH(0);
			SetUserDragW(0);
			if (nW > m_stLastDrawSize.cx)
				SetUserDragW(float(nW - m_stLastDrawSize.cx) / 2);
			if (nH > m_stLastDrawSize.cy)
				SetUserDragH(float(nH - m_stLastDrawSize.cy) / 2);

			m_fThunmbClockX = 0;
			m_fThunmbClockY = 0;

			m_bIsFullScreen = false;

			//清除拖动标记
			/*SetUserDragH(0);
			SetUserDragW(0);*/

			if(DrawAllImage())
				RePaintDialog(0, 0, nX, nY);

			m_bOriginalSize = true;

			return emReduction;

			//这个可以进行一比一操作
			//如果这个图片的现在尺寸达到了屏幕,那么就可能现在加载的是缩略图
			//if (nW >= m_stCurScreenSize.cx || nH >= m_stCurScreenSize.cy)
			//{
			//	//可能现在是缩略图,现在去获取原图
			//	/*if (CLoadThread::Instance().GetCurFullPic(m_pCurImageFull))
			//	{*/
			//		nW = m_pCurImage->GetWidth();
			//		nH = m_pCurImage->GetHeight();

			//		//加载成功了,确定窗口大小...
			//		m_stLastDrawSize.cx = nW < m_stCurScreenSize.cx ? nW : m_stCurScreenSize.cx;
			//		m_stLastDrawSize.cy = nH < m_stCurScreenSize.cy ? nH : m_stCurScreenSize.cy;

			//		//重新计算位置...并且刷新窗口
			//		int nX = (m_stCurScreenSize.cx - m_stLastDrawSize.cx) / 2;
			//		int nY = (m_stCurScreenSize.cy - m_stLastDrawSize.cy) / 2;

			//		//进行放大操作!!!
			//		ChangeArea();
			//		/*m_bUseFullPic = true;
			//		m_fDIsplayProportion = 1.0;*/
			//		m_bUseChangeProportion = true;

			//		m_bCurImageChange = true;

			//		if(DrawAllImage())
			//			RePaintDialog(0, 0, nX, nY);
			//	//}
			//}
			//else
			//{
			//	//这个就直接1:1了
			//	//不过尺寸还是不能小于默认值
			//	m_stLastDrawSize.cx = nW > DEFAULT_WINDOW_SIZE_W ? nW : DEFAULT_WINDOW_SIZE_W;
			//	m_stLastDrawSize.cy = nH > DEFAULT_WINDOW_SIZE_H ? nH : DEFAULT_WINDOW_SIZE_H;

			//	//m_fDIsplayProportion = 1.0;
			//	m_bUseChangeProportion = false;

			//	//m_bCurImageChange = true;

			//	if (DrawAllImage())
			//		RePaintDialog(0, 0, (m_stCurScreenSize.cx / 2) - (m_stLastDrawSize.cx / 2), (m_stCurScreenSize.cy / 2) - (m_stLastDrawSize.cy / 2));
			//}
		}
		break;
	case emFullScreen:			//全屏
		{
			if (m_bIsFullScreen)
			{
				m_stLastDrawSize = m_stTempSize;
				m_stLastDrawPoint = m_stTempPoint;

				//修改窗口大小
				::MoveWindow(m_hWnd, 0, 0, m_stCurScreenSize.cx, m_stCurScreenSize.cy, FALSE);
			}
			else
			{
				m_stTempSize = m_stLastDrawSize;
				m_stTempPoint = m_stLastDrawPoint;

				//修改窗口大小
				::MoveWindow(m_hWnd, 0, 0, m_stCurScreenSize.cx, m_stCurScreenSize.cy, FALSE);
			}

			m_bCurImageChange = true;

			m_bIsFullScreen = !m_bIsFullScreen;

			if(DrawAllImage())
				RePaintDialog(0, 0, m_stTempPoint.x, m_stTempPoint.y);

			m_bOriginalSize = false;
		}
		break;
	case emRotation:			//旋转
		{
			if (!m_pCurImage->IsNull())
			{
				//m_pCurImage->RotateFlip(Rotate90FlipNone);
				CImage* pImage = new CImage;
				ImageRotate(pImage, m_pCurImage);
				
				m_cRotateNum++;

				//释放原来的资源
				ClearRotateTemp(1);

				m_pCurImage = pImage;

				m_bCurImageChange = true;

				//换回缩略图
				m_bUseChangeProportion = false;
				m_fUserProportion = 0.0;
				//m_bUseFullPic = false;

				if(DrawAllImage())
					RePaintDialog(/*0, 0, m_stTempPoint.x, m_stTempPoint.y*/);

				m_bOriginalSize = false;
			}
		}
		break;
	case emSave:				//保存
		m_cLoadThread.SaveImageToFile(m_nCurPicIndex);
		break;
	default:
		break;
	}

	return m_emLastClickButton;
}

RMenuBtnID CCImageViewDlg::OnClickRBtnMenuItem()
{
	RMenuBtnID emID =  CheckRBtnMenuItem();
	switch(emID)
	{
	case emRBtnCopy:
		OnClickCopy();
		break;
	case emRBtnSave:
		OnClickSaveToFile();
		break;
	default:
		return emRBtnMax;
	}

	m_TooleBtn[emRBtnMenu].m_bDisplayState = false;

	if (DrawAllImage())
		RePaintDialog();

	return emID;
}

//检查重绘
ToolButtunID CCImageViewDlg::CheckRePaintDialog()
{
	//更新鼠标位置
	UpdateCurPoint();
	
	//先判断是不是正在加载当前文件
	if (!m_bLoadSucess)
		TrunPage();

	//判断要不要重绘
	bool bRePaint = false;

	bRePaint = m_bCurImageChange || bRePaint;
	bRePaint = CheckReDraw() || bRePaint;

	if (bRePaint)
	{
		if(DrawAllImage())
			RePaintDialog();
	}

	return emBgImage;
}

//放大缩小, 默认做1:1处理
bool CCImageViewDlg::ChangeArea(char cIsAdd /*= 0*/)
{
	//直接加载原图
	//if (m_fCurPicProportion < 1.0 && !m_pCurImageFull && CLoadThread::Instance().GetCurFullPic(m_pCurImageFull))
	//{
	//	OutputDebugStringA("完整图加载成功!\n");

	//	//加载成功完整图后,重置缩放比
	//	//m_fCurPicProportion *= m_fDIsplayProportion;

	//	//调整缩放比
	//}

	//用户调整了缩放比例
	if (cIsAdd)
		m_bUseChangeProportion = true;


	//操作前保存中心点
	float fDisplayW = m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion);
	float fDisplayH = m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion);

	//float fCenterX = fDisplayW - m_fUserDragW - min(fDisplayW, m_stLastDrawSize.cx) / 2;
	//float fCenterY = fDisplayH - m_fUserDragH - min(fDisplayH, m_stLastDrawSize.cy) / 2;

	//操作前也保存下原比例
	//float fProportion = m_fUserProportion;

	//缩放操作
	m_fUserProportion += float(cIsAdd * (DEFAULT_PROPORTION + m_fUserProportion * 0.1));

	//char szMsg[64] = {0};
	//_snprintf_s(szMsg, sizeof(szMsg), "m_fUserProportion = %f \n", m_fUserProportion);
	//OutputDebugStringA(szMsg);

	if ((m_fCurPicProportion + m_fUserProportion) > DEFAULT_MAX_ZOOM)
		m_fUserProportion = DEFAULT_MAX_ZOOM - m_fCurPicProportion;
	else if ((m_fCurPicProportion + m_fUserProportion) < DEFAULT_MAX_NARROW)
		m_fUserProportion = DEFAULT_MAX_NARROW - m_fCurPicProportion;


	//缩放后的新数据
	float fNewDisplayW = m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion);
	float fNewDisplayH = m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion);

	if (fNewDisplayW > m_stLastDrawSize.cx || fNewDisplayH > m_stLastDrawSize.cy)
	{
		//float fNewCenterX = fCenterX * (fNewDisplayW / fDisplayW);
		//m_fUserDragW += (fNewDisplayW - max(m_stLastDrawSize.cx, fDisplayW)) / 2;

		SetUserDragW(m_fUserDragW + (fNewDisplayW - max(m_stLastDrawSize.cx, fDisplayW)) / 2);
		SetUserDragH(m_fUserDragH + (fNewDisplayH - max(m_stLastDrawSize.cy, fDisplayH)) / 2);
	}

	//if (fNewDisplayH > m_stLastDrawSize.cy)
	//{
	//	//float fNewCenterY = fCenterY * (fNewDisplayH / fDisplayH);
	//	//m_fUserDragH += (fNewDisplayH - max(m_stLastDrawSize.cy, fDisplayH)) / 2;

	//	SetUserDragH(m_fUserDragH + (fNewDisplayH - max(m_stLastDrawSize.cy, fDisplayH)) / 2);
	//}

	/*if ((m_fCurPicProportion + cIsAdd * DEFAULT_PROPORTION + m_fUserProportion) > DEFAULT_MAX_NARROW &&\
		(m_fCurPicProportion + cIsAdd * DEFAULT_PROPORTION + m_fUserProportion) < DEFAULT_MAX_ZOOM)
	{*/
		//float fX = 0.0;
		//float fY = 0.0;

		////调整下偏移量,保证放大后中心点不会偏移
		//float nX = m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion);
		///*if (nX > m_stLastDrawSize.cx)
		//m_fUserDragW -= (nX - m_stLastDrawSize.cx ) * DEFAULT_PROPORTION / 2;
		//else
		//m_fUserDragW = 0;*/
		//if (nX > m_stLastDrawSize.cx)
		//	fX = m_fUserDragW / nX;

		//float nY = m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion);
		///*if (nY > m_stLastDrawSize.cy)
		//m_fUserDragH -= (nY - m_stLastDrawSize.cy ) * DEFAULT_PROPORTION / 2;
		//else
		//m_fUserDragH = 0;*/
		//if (nY > m_stLastDrawSize.cy)
		//	fY = m_fUserDragH / nY;

		//m_fUserProportion += float(cIsAdd * (DEFAULT_PROPORTION + m_fUserProportion * 0.1));

		//if ((m_fCurPicProportion + m_fUserProportion) > DEFAULT_MAX_ZOOM)
		//	m_fUserProportion = DEFAULT_MAX_ZOOM - m_fCurPicProportion;
		//else if ((m_fCurPicProportion + m_fUserProportion) < DEFAULT_MAX_NARROW)
		//	m_fUserProportion = DEFAULT_MAX_NARROW - m_fCurPicProportion;

		//if (fX > 0.000000)
		//	//m_fUserDragW = fX * m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion);
		//	SetUserDragW(fX * m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion));

		//if (fY > 0.000000)
		//	//m_fUserDragH = fY * m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion);
		//	SetUserDragH(fY * m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion));
	//}

	/*char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "m_nUserDragW = %f, m_nUserDragH = %f \n", m_fUserDragW, m_fUserDragH);
	OutputDebugStringA(szMsg);*/

	m_bProportionState = true;
	//设置消失的定时器,先删除原来的再设置
	KillTimer(emTimerProportionBG);
	SetTimer(emTimerProportionBG, DEFAULT_PRORORTIO_TMIE, NULL);

	if (DrawAllImage())
		RePaintDialog();

	/*if (cIsAdd > 0)
	{
	m_nUserDragH += int(m_nUserDragH * DEFAULT_PROPORTION);
	m_nUserDragW += int(m_nUserDragW * DEFAULT_PROPORTION);
	}

	if (cIsAdd < 0)
	{
	m_nUserDragH -= int(m_nUserDragH * DEFAULT_PROPORTION);
	m_nUserDragW -= int(m_nUserDragW * DEFAULT_PROPORTION);
	}*/

	//放大缩小之前保存当前中心点,以此为基点进行缩放

	//m_fDIsplayProportion += float(DEFAULT_PROPORTION * (float)cIsAdd);

	//使用原图
	//if (!m_bUseFullPic && m_fDIsplayProportion >= 1.0 && m_fCurPicProportion < 1.0 && m_pCurImageFull)
	//{
	//	m_bUseFullPic = true;
	//	OutputDebugStringA("使用原图!!!!\n");

	//	//换为原图的缩放比例
	//	m_fDIsplayProportion = m_fCurPicProportion;
	//}

	//缩放到一定大小换回缩略图
	//if (m_bUseFullPic && m_pCurImageFull && m_fCurPicProportion \
	//	&& m_pCurImageFull->GetWidth() * m_fDIsplayProportion < m_pCurImage->GetWidth()\
	//	&& m_pCurImageFull->GetHeight() * m_fDIsplayProportion < m_pCurImage->GetHeight())
	//{
	//	m_bUseFullPic = false;

	//	//换回缩略图
	//	m_fDIsplayProportion = 1.0;

	//	OutputDebugStringA("换回缩略图!!!!\n");
	//}

	/*m_fUserProportion += float(DEFAULT_PROPORTION * (float)cIsAdd);

	if (m_fUserProportion > DEFAULT_MAX_ZOOM)
		m_fUserProportion = DEFAULT_MAX_ZOOM;
	else if (m_fUserProportion < DEFAULT_MAX_NARROW)
		m_fUserProportion = (float)DEFAULT_MAX_NARROW;*/

	/*m_bCurImageChange = true;

	char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "当前缩放比 : %f \n", m_fCurPicProportion + m_fUserProportion);
	OutputDebugStringA(szMsg);*/

	return true;
}

//更新鼠标坐标
void CCImageViewDlg::UpdateCurPoint()
{
	//获取相应的数据
	GetCursorPos( &m_cPoint );
	GetWindowRect(&m_stRct);

	//当前鼠标的相对位置
	m_cCurPoint.x = m_cPoint.x - m_stRct.left;
	m_cCurPoint.y = m_cPoint.y - m_stRct.top;
}

//设置类型和路径
void CCImageViewDlg::SetLoadTypeAndPath(LoadType emType, const void* pSource)
{
	m_emLoadType = emType;
	
	switch (emType)
	{
	case emLoadList:
		{
			m_vtLoadFileList.clear();
			vector<string>* pSr = (vector<string>*)pSource;
			for (vector<string>::iterator itera = pSr->begin(); itera != pSr->end(); ++itera)
				m_vtLoadFileList.push_back(CCodeConversion::Instance().MultCHarToWideChar(*itera));

			m_pLoadPath = &m_vtLoadFileList;
		}
		break;
	case emLoadDir:
		{
			m_strLoadPath = (wchar_t*)pSource;// CCodeConversion::Instance().MultCHarToWideChar((char*)pSource);

			m_pLoadPath = &m_strLoadPath;
		}
		break;
	default:
		break;
	}

	m_cLoadThread.LoadFile(m_emLoadType, m_pLoadPath);
}

//清理旋转缓存
void CCImageViewDlg::ClearRotateTemp(BYTE bCount/* = 0*/)
{
	if (m_cRotateNum > bCount && m_pCurImage)
	{
		if (!m_pCurImage->IsNull())
		{
			m_pCurImage->ReleaseDC();
			m_pCurImage->Destroy();
		}

		delete m_pCurImage;
		m_pCurImage = NULL;
	}
}

bool CCImageViewDlg::LoadPngForRes(CImage* pCImg, UINT nResID)
{
	if( pCImg ==NULL)
		return false;

	pCImg->Destroy();

	// 查找资源
	HRSRC hRsrc =::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), L"png");
	if(hRsrc == NULL)
		return false;

	// 加载资源
	HGLOBAL hImgData =::LoadResource(AfxGetResourceHandle(), hRsrc);
	if(hImgData == NULL)
	{
		::FreeResource(hImgData);
		return false;
	}

	// 锁定内存中的指定资源
	LPVOID lpVoid =::LockResource(hImgData); 
	LPSTREAM pStream = NULL;
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize); 
	// 解除内存中的指定资源
	::GlobalUnlock(hNew);

	// 从指定内存创建流对象
	HRESULT ht =::CreateStreamOnHGlobal(hNew,TRUE,&pStream);
	if( ht != S_OK )
	{
		GlobalFree(hNew);
	}
	else
	{
		// 加载图片
		pCImg->Load(pStream); 
		GlobalFree(hNew);
	}
	// 释放资源
	::FreeResource(hImgData);
	return true;
}

//显示缩放比例
void CCImageViewDlg::ShowThumnProportion()
{
	if (!m_TooleBtn[emProportionBG].m_pImage || m_TooleBtn[emProportionBG].m_pImage->IsNull())
		return;

	if (!m_bProportionState)
		return;

	//计算位置
	int nX = (m_stLastDrawSize.cx - m_TooleBtn[emProportionBG].m_pImage->GetWidth()) / 2;
	int nY = (m_stLastDrawSize.cy - m_TooleBtn[emProportionBG].m_pImage->GetHeight()) / 2;

	DrawImage(m_TooleBtn[emProportionBG].m_pImage, nX, nY);

	//写字
	CDC* pDc = CDC::FromHandle(m_hdcMemory);
	CFont cFont;
	cFont.CreatePointFont(DEFAULT_PRO_FONT_SIZE * 10, L"微软雅黑");

	//pDc->SetTextCharacterExtra(2);//设置字符间距
	pDc->SetBkMode(TRANSPARENT);  //设置背景透明
	pDc->SetTextColor(RGB(255,255,255));
	pDc->SelectObject(&cFont);

	TEXTMETRIC tm; 
	pDc->GetTextMetrics(&tm);

	//字体平滑
	//m_pGh->SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	
	wchar_t szProp[16] = {0};
	_snwprintf_s(szProp, sizeof(szProp), L"%d%%", int((m_fCurPicProportion + m_fUserProportion) * 100));

	nY += (m_TooleBtn[emProportionBG].m_pImage->GetHeight() - (tm.tmHeight+tm.tmExternalLeading)) / 2;
	nX += (m_TooleBtn[emProportionBG].m_pImage->GetWidth() - ((wcslen(szProp) + 1) * tm.tmAveCharWidth)) / 2;

	//m_pGh->DrawString(szProp, wcslen(szProp), );
	pDc->TextOut(nX, nY, szProp);
}

//设置鼠标指针
void CCImageViewDlg::SetCur()
{
	if (emBgImage == CheckItemPoint())
		SetCursor(m_hCurNormal);
}

//拖动图片
void CCImageViewDlg::DragImage(LPARAM lParam)
{
	/*m_fUserDragW += m_cDragTmp.x - GET_X_LPARAM(lParam);
	m_fUserDragH += m_cDragTmp.y - GET_Y_LPARAM(lParam);

	m_nThunmbClockX += (m_cDragTmp.x - GET_X_LPARAM(lParam)) / ((m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosX);
	m_nThunmbClockY += (m_cDragTmp.y - GET_Y_LPARAM(lParam)) / ((m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosY);*/

	SetUserDragW(m_fUserDragW + m_cDragTmp.x - GET_X_LPARAM(lParam));
	SetUserDragH(m_fUserDragH + m_cDragTmp.y - GET_Y_LPARAM(lParam));
}

//拖动缩略图
void CCImageViewDlg::DragThumb(LPARAM lParam)
{
	//第一版
	/*m_fUserDragW -= (int)(float(m_cDragTmp.x - GET_X_LPARAM(lParam)) / ((float)m_TooleBtn[emThumbnail].w() / (float)m_pCurImage->GetWidth()));
	m_fUserDragH -= (int)(float(m_cDragTmp.y - GET_Y_LPARAM(lParam)) / ((float)m_TooleBtn[emThumbnail].h() / (float)m_pCurImage->GetHeight()));*/

	//第二版
	/*m_fUserDragW -= (int)(float(m_cDragTmp.x - GET_X_LPARAM(lParam)) * (m_fCurPicProportion + m_fUserProportion));
	m_fUserDragH -= (int)(float(m_cDragTmp.y - GET_Y_LPARAM(lParam)) * (m_fCurPicProportion + m_fUserProportion));*/

	//第三版
	/*m_fUserDragW -= ((float)m_pCurImage->GetWidth() / (float)m_TooleBtn[emThumbnail].w()) * m_cDragTmp.x - GET_X_LPARAM(lParam);
	m_fUserDragH -= ((float)m_pCurImage->GetHeight() / (float)m_TooleBtn[emThumbnail].h()) * m_cDragTmp.y - GET_Y_LPARAM(lParam);*/

	/*m_nThunmbClockX -= m_cDragTmp.x - GET_X_LPARAM(lParam);
	m_nThunmbClockY -= m_cDragTmp.y - GET_Y_LPARAM(lParam);

	m_fUserDragW -= (m_cDragTmp.x - GET_X_LPARAM(lParam)) * ((m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosX);
	m_fUserDragH -= (m_cDragTmp.y - GET_Y_LPARAM(lParam)) * ((m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosX);*/

	SetThunmbClockX(m_fThunmbClockX - (m_cDragTmp.x - GET_X_LPARAM(lParam)));
	SetThunmbClockY(m_fThunmbClockY - (m_cDragTmp.y - GET_Y_LPARAM(lParam)));
}

void CCImageViewDlg::SetThunmbClockX(float val)
{
	if (val < (m_nCurThumbPosX))
		val = (float)m_nCurThumbPosX;
	else if (val > (m_nCurThumbPosX + m_nCurThumbPosW - m_fThunmbClockW))
		val = m_nCurThumbPosX + m_nCurThumbPosW - m_fThunmbClockW;

	//m_fUserDragW += (val - m_nThunmbClockX) * ((m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosW);
	m_fUserDragW = (float(val - m_nCurThumbPosX) * (m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion)) / (float)m_nCurThumbPosW);

	m_fThunmbClockX = val;
}
void CCImageViewDlg::SetThunmbClockY(float val)
{
	if (val < (m_nCurThumbPosY))
		val = (float)m_nCurThumbPosY;
	else if (val > (m_nCurThumbPosY + m_nCurThumbPosH - m_fThunmbClockH - 1))
		val = m_nCurThumbPosY + m_nCurThumbPosH - m_fThunmbClockH - 1;

	//m_fUserDragH += (val - m_nThunmbClockY) * ((m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosH);
	m_fUserDragH = (float(val - m_nCurThumbPosY) * (m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion)) / (float)m_nCurThumbPosH);

	m_fThunmbClockY = val;
}
void CCImageViewDlg::SetUserDragW(float val) 
{ 
	float fDisplayW = m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion);

	//拉回
	if (val < 0)
		val = 0;
	else if (val > (fDisplayW - min(fDisplayW, m_stLastDrawSize.cx)))
		val = fDisplayW - min(fDisplayW, m_stLastDrawSize.cx);

	/*char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "xxxx : %f \n", val);
	OutputDebugStringA(szMsg);*/

	if (m_nCurThumbPosW)
		m_fThunmbClockX = val * m_nCurThumbPosW / fDisplayW + m_nCurThumbPosX;

	m_fUserDragW = val; 
}
void CCImageViewDlg::SetUserDragH(float val) 
{ 
	float fDisplayH = m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion);

	/*char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "xxxx : %f \n", (fDisplayH - min(m_stLastDrawSize.cy, fDisplayH)));
	OutputDebugStringA(szMsg);*/

	//拉回
	if (val < 0)
		val = 0;
	else if (val > (fDisplayH - min(m_stLastDrawSize.cy, fDisplayH)))
		val = fDisplayH - min(m_stLastDrawSize.cy, fDisplayH);

	if (m_nCurThumbPosH)
		m_fThunmbClockY = val * m_nCurThumbPosH / fDisplayH + m_nCurThumbPosY;

	m_fUserDragH = val; 
}

//显示翻页处数量
void CCImageViewDlg::ShowPicCountTips(MSG* pMsg)
{
	if (!m_bShowTips)
	{
		m_cTip.DelTool(this);
		return;
	}

	//先....准备好要写的字
	wchar_t szTips[64] = {0};
	_snwprintf_s(szTips, sizeof(szTips), L"%d / %d", m_nCurPicIndex + 1, m_nAllPicCount);
	/*bool bIsNumber = false;
	if (0 == m_nCurPicIndex && m_bIsLeft)
		_snwprintf_s(szTips, sizeof(szTips), L"已经是第一张.");
	else if (m_nAllPicCount == (m_nCurPicIndex + 1) && !m_bIsLeft)
		_snwprintf_s(szTips, sizeof(szTips), L"已经是最后一张了.");
	else
	{
		_snwprintf_s(szTips, sizeof(szTips), L"%d / %d", m_nCurPicIndex + 1, m_nAllPicCount);
		bIsNumber = true;
	}*/

	if (!m_cTip.IsWindowVisible() && pMsg)
	{
		m_cTip.AddTool(this, szTips);
		m_cTip.SetDelayTime(TTDT_INITIAL, 100); 
		m_cTip.SetDelayTime(TTDT_AUTOPOP, 1000000);
		m_cTip.RelayEvent(pMsg);
	}
	else if(m_cTip.IsWindowVisible() && !pMsg)
	{
		m_cTip.UpdateTipText(szTips, this);
		//OutputDebugString(szTips);
	}
}
void CCImageViewDlg::OnClickCopy()
{
	if (!m_pCurImage || m_pCurImage->IsNull())
		return ;
	
	CRect rect;   
	CDC *hdc = GetDC();
	HDC memDC = CreateCompatibleDC(*hdc);    // 绘图内存DC
	/*HBITMAP mybitmap = CreateCompatibleBitmap(*hdc, m_pCurImage->GetWidth(), m_pCurImage->GetHeight());
	SelectObject(memDC,mybitmap);*/

	CDC BufferDC;
	BufferDC.CreateCompatibleDC(hdc);
	/*CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(hdc,m_pCurImage->GetWidth(), m_pCurImage->GetHeight());
	BufferDC.SelectObject(&BufferBmp);*/
	CDC pdc;
	HBITMAP mybitmap = CreateCompatibleBitmap(*hdc, m_pCurImage->GetWidth(), m_pCurImage->GetHeight());
	pdc.CreateCompatibleDC(&BufferDC);
	pdc.SelectObject( mybitmap );

	m_pCurImage->AlphaBlend(pdc, 0, 0,m_pCurImage->GetWidth(), m_pCurImage->GetHeight(), 0, 0, m_pCurImage->GetWidth(), m_pCurImage->GetHeight(), 254);

	BufferDC.BitBlt(0,0, m_pCurImage->GetWidth(), m_pCurImage->GetHeight(), &pdc,0,0,SRCCOPY);   

	if(OpenClipboard())
	{
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, (HBITMAP)mybitmap);  
		CloseClipboard();
	}

}

void CCImageViewDlg::OnClickSaveToFile()
{
	// TODO: 在此添加命令处理程序代码
	m_cLoadThread.SaveImageToFile(m_nCurPicIndex);
}

//右键菜单
void CCImageViewDlg::ShowLBtnMenu()
{
	if (!m_TooleBtn[emRBtnMenu].m_bDisplayState)
		return;

	//保持在窗口内
	if ((m_stLastRBtnPos.x + m_TooleBtn[emRBtnMenu].m_pImage->GetWidth()) > m_stLastDrawSize.cx)
		m_stLastRBtnPos.x = m_stLastDrawSize.cx - m_TooleBtn[emRBtnMenu].m_pImage->GetWidth();
	if ((m_stLastRBtnPos.y + m_TooleBtn[emRBtnMenu].m_pImage->GetHeight()) > m_stLastDrawSize.cy)
		m_stLastRBtnPos.y = m_stLastDrawSize.cy - m_TooleBtn[emRBtnMenu].m_pImage->GetHeight();

	m_TooleBtn[emRBtnMenu].m_curPoint = m_stLastRBtnPos;
	DrawImage(m_TooleBtn[emRBtnMenu].m_pImage, m_stLastRBtnPos.x, m_stLastRBtnPos.y);

	//画高亮
	m_emLastMRtnIndex =  CheckRBtnMenuItem();
	if (m_emLastMRtnIndex != emRBtnMax)
	{
		//高亮
		DrawImage(m_TooleBtn[emRBtnMenuTips].m_pImage,
			m_TooleBtn[emRBtnMenu].m_curPoint.x + g_stLBtnMenuPoints[m_emLastMRtnIndex].left,\
			m_TooleBtn[emRBtnMenu].m_curPoint.y + g_stLBtnMenuPoints[m_emLastMRtnIndex].top,\
			g_stLBtnMenuPoints[m_emLastMRtnIndex].right - g_stLBtnMenuPoints[m_emLastMRtnIndex].left,\
			g_stLBtnMenuPoints[m_emLastMRtnIndex].bottom - g_stLBtnMenuPoints[m_emLastMRtnIndex].top,\
			g_stLBtnMenuPoints[m_emLastMRtnIndex].left, g_stLBtnMenuPoints[m_emLastMRtnIndex].top,\
			g_stLBtnMenuPoints[m_emLastMRtnIndex].right - g_stLBtnMenuPoints[m_emLastMRtnIndex].left,\
			g_stLBtnMenuPoints[m_emLastMRtnIndex].bottom - g_stLBtnMenuPoints[m_emLastMRtnIndex].top);
	}
}

//按键响应
void CCImageViewDlg::OnRBtnKeyDown(MSG* pMsg)
{
	if (!m_TooleBtn[emRBtnMenu].m_bDisplayState)
		return;

	switch(pMsg->wParam)
	{
	case 'C':
	case 'c':
		OnClickCopy();
		break;
	case 's':
	case 'S':
		OnClickSaveToFile();
		break;
	default:
		return;
	}

	m_TooleBtn[emRBtnMenu].m_bDisplayState = false;
	if (DrawAllImage())
		RePaintDialog();
}

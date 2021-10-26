// CImageViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CImageView.h"
#include "CImageViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	THUMBNAIL_WIDTH		100
#define	THUMBNAIL_HEIGHT	90

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CCImageViewDlg �Ի���




CCImageViewDlg::CCImageViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCImageViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hCurNormal = AfxGetApp()->LoadCursor(IDC_CURSOR1);

	HWND hWnd = ::FindWindow(NULL, L"�򵥿�ͼ");
	if (hWnd)
		::PostMessage(hWnd, WM_CLOSE, 0, 0);

	//��ȡ������
	wstring strCommand = AfxGetApp()->m_lpCmdLine;
	//AfxMessageBox(L"1111111");
	//����ļ����������ַ�,ϵͳ�������߼���˫����,����Ҫȥ��
	if (wstring::npos != strCommand.find(L"\""))
		strCommand = strCommand.substr(1, strCommand.length() - 2);

	//AfxMessageBox(strCommand.data());

	//�ȵ���,��֤������,������GDI+�ͷ�֮ǰ����GDI+����
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

	//��Ļ���治����������
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


// CCImageViewDlg ��Ϣ�������

BOOL CCImageViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	SetWindowText(L"�򵥿�ͼ");

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//LoadAllFileFromDir(L"E:\\project\\Project\\CImageView\\Debug\\pics");
	//LoadAllFileFromDir(L"G:\\Users\\Ming\\Desktop");

	//����ʼ���رյ�ʱ��Ῠ��
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_ldiplusToken, &gdiplusStartupInput, NULL);

	m_cLoadThread.StartThread(m_stCurScreenSize);

	//CLoadThread::Instance().LoadFile(emLoadDir, L"E:\\����\\�������\\8297w");

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^WS_EX_LAYERED);

	//����������
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);

	//�����ö�
	//::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION );

	//��ȡUpdateLayeredWindow
	HMODULE hFuncInst=LoadLibrary(L"User32.DLL");
	m_hUpdateLayeredWindow=(UDLWindow)GetProcAddress(hFuncInst,"UpdateLayeredWindow");
	FreeLibrary(hFuncInst);

	m_Blend.BlendOp=AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//�˵���ʼ��
	m_cTip.Create(this);
	//m_cTip.AddTool(this, L"");
	//�ӳ�ʱ��
	//m_cTip.SetDelayTime(1000);
	//�����ı��ͱ�����ɫ
	m_cTip.SetTipBkColor(RGB(255, 255, 255));
	m_cTip.SetTipTextColor(RGB(255, 0, 0));
	//�����Ƿ�����
	m_cTip.Activate(TRUE);

	////�����ڴ�λͼ
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

	//������Դ
	for (short w = 0; w < (sizeof(g_nOnlyRes) / sizeof(g_nOnlyRes[0])); ++w)
	{
		CImage* pImage = new CImage();
		LoadPngForRes(pImage, g_nOnlyRes[w]);
		if (!pImage->IsNull())
			m_mapIcons.insert(make_pair(g_nOnlyRes[w], pImage));
		else
			OnCancel();
	}

	//����Դ�빤���ǹ���
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

	//����
	TrunPage(0, true);

	//��ʱ���������Ƴ�����
	SetTimer(emTimerOutOfWimCheck, 1000, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCImageViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCImageViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CCImageViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����	
	return hbr;
}

LRESULT CCImageViewDlg::OnNcHitTest(CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	UINT uRet = CDialog::OnNcHitTest(point);

	/*if (emBgImage != CheckItemPoint())
		return uRet;*/

	//����϶����Ǵ���,�򷵻ر�����
	return (HTCLIENT == uRet) && !m_bIsFullScreen && !m_bShowThumbnail ? HTCAPTION : uRet;
}

//��ʾ����ͼ
void CCImageViewDlg::ShowThumbnail(float fPosX, float fPosY, float fWidth, float fHeight)
{
	if (!m_bShowThumbnail || m_bUserCloseThumb)
	{
		m_TooleBtn[emThumbnail].m_bDisplayState = false;
		return;
	}

	//��ʾ�����½�,�̶���С
	if (m_TooleBtn[emThumbnail].m_pImage->IsNull())
		return;

	int nX = m_stLastDrawSize.cx - DEFAULT_THUMBNAUL_W - 3;
	int nY = m_stLastDrawSize.cy - DEFAULT_THUMBNAUL_H - 3;

	//�Ȼ��ϱ���ͼ
	DrawImage(m_TooleBtn[emThumbnail].m_pImage, nX, nY, DEFAULT_THUMBNAUL_W, DEFAULT_THUMBNAUL_H);

	//Ȼ���ٻ�������ͼ,��������Сһ��.....
	if (m_pCurImage && !m_pCurImage->IsNull())
	{
		m_nCurThumbPosW = m_pCurImage->GetWidth();
		m_nCurThumbPosH = m_pCurImage->GetHeight();
		float shear = (float)m_nCurThumbPosW/(float)m_nCurThumbPosH;

		//���Ƚϴ����
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

		//����λ��
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

		//����ͼ�ϻ��и�С�Ŀ�, �����û�����ƫ������ȷ��λ��, �������ű�ȷ����С
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

		//����һ���ײ�
		m_TooleBtn[emThumRect].m_curPoint.x = (LONG)m_fThunmbClockX;
		m_TooleBtn[emThumRect].m_curPoint.y = (LONG)m_fThunmbClockY;
		m_TooleBtn[emThumRect].m_wShowW = (short)m_fThunmbClockW;
		m_TooleBtn[emThumRect].m_wShowH = (short)m_fThunmbClockH;
		m_TooleBtn[emThumRect].m_bDisplayState = true;
		DrawImage(m_TooleBtn[emThumRect].m_pImage, m_TooleBtn[emThumRect].m_curPoint.x, m_TooleBtn[emThumRect].m_curPoint.y, m_TooleBtn[emThumRect].w(), m_TooleBtn[emThumRect].h());

		m_pGh = Graphics::FromHDC(m_hdcMemory);
		Pen pen(Color(0, 0, 0), 0.5);
		//����ɫ
		m_pGh->DrawRectangle(&pen, m_fThunmbClockX, m_fThunmbClockY, m_fThunmbClockW, m_fThunmbClockH);
		//�ڲ��ɫ
		pen.SetColor(Color(255, 255, 255));
		m_pGh->DrawRectangle(&pen, m_fThunmbClockX + 1, m_fThunmbClockY + 1, m_fThunmbClockW - 2, m_fThunmbClockH - 2);

		//���Ͻ��йرհ�ť
		m_TooleBtn[emThumClose].m_bDisplayState = true;
		m_TooleBtn[emThumClose].m_curPoint.x = nX + DEFAULT_THUMBNAUL_W - 20;
		m_TooleBtn[emThumClose].m_curPoint.y = nY + 10;

		//���رհ�ť����
		if (m_TooleBtn[emThumCloseBG].m_bDisplayState)
			DrawImage(m_TooleBtn[emThumCloseBG].m_pImage, m_TooleBtn[emThumClose].m_curPoint.x - 4, m_TooleBtn[emThumClose].m_curPoint.y - 2,\
			m_TooleBtn[emThumCloseBG].m_pImage->GetWidth() - 2, m_TooleBtn[emThumCloseBG].m_pImage->GetHeight() - 2);

		DrawImage(m_TooleBtn[emThumClose].m_pImage, m_TooleBtn[emThumClose].m_curPoint.x, m_TooleBtn[emThumClose].m_curPoint.y, \
			m_TooleBtn[emThumClose].m_pImage->GetWidth() * DEFAULT_THUM_CLOSE_PRO, m_TooleBtn[emThumClose].m_pImage->GetHeight() * DEFAULT_THUM_CLOSE_PRO);

	}
}
void CCImageViewDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	OutputDebugStringA("��ʾ�߳��˳�\n");
}

int CCImageViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	return 0;
}

void CCImageViewDlg::RePaintDialog(int nWidth /*= 0*/, int nHeight /*= 0*/, int nPosX /*= -1*/, int nPosY /*= -1*/)
{
	static POINT ptSrc={0,0};
	
	//����λ��
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

//�����Ļ�ͼ
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
	////������Դ
	//BufferDC.DeleteDC(); 
	//cBitMap.DeleteObject();
	//ReleaseDC(pDC);  
	//ReleaseDC(&BufferDC);  

	//return true;

	//�����ڴ�λͼ
	DeleteDC(m_hdcMemory);
	DeleteDC(m_hdcTmp);

	m_hdcTmp=GetDC()->m_hDC;
	m_hdcMemory=CreateCompatibleDC(m_hdcTmp);
	HBITMAP hBitMap=CreateCompatibleBitmap(m_hdcTmp, m_stCurScreenSize.cx, m_stCurScreenSize.cy);
	SelectObject(m_hdcMemory,hBitMap);
	//m_pGh = Graphics::FromHDC(m_hdcMemory);

	//�Ȼ�����
	DrawBackGroundImage();

	//������ͼ
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

		//����ߴ�
		int nW = int(pImage->GetWidth());
		int nH = int(pImage->GetHeight());

		//����λ��
		int nThumbnailPosX = (m_stLastDrawSize.cx - nW) / 2;
		int nThumbnailPosY = (m_stLastDrawSize.cy - nH) / 2;

		//���µ�����С,����û������˱����Ͳ����¼�����
		if (m_bUseChangeProportion)
		{
			//�ߴ��λ�ö�Ҫ���µ���
			nW = int(nW * (m_fCurPicProportion + m_fUserProportion));
			nH = int(nH * (m_fCurPicProportion + m_fUserProportion));

			nThumbnailPosX = (m_stLastDrawSize.cx - nW) / 2;
			nThumbnailPosY = (m_stLastDrawSize.cy - nH) / 2;
		}
		else if ((nW > m_stLastDrawSize.cx || nH > m_stLastDrawSize.cy))
		{
			float shear = (float)nW/(float)nH;

			//���Ƚϴ����
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

			//����ǻ�����Χ
			int nDrawX = (nThumbnailPosX > m_stLastDrawSize.cx || nThumbnailPosX < 0) ? 0 : nThumbnailPosX;
			int nDrawY = (nThumbnailPosY > m_stLastDrawSize.cy || nThumbnailPosY < 0) ? 0 : nThumbnailPosY;
			int nDrawW = nW > m_stLastDrawSize.cx ? m_stLastDrawSize.cx : nW;
			int nDrawH = nH > m_stLastDrawSize.cy ? m_stLastDrawSize.cy : nH;

			//Rect stDrawRect(nDrawX, nDrawY, nDrawW, nDrawH);

			//���ش���, ��Ҫ����Ϊʲô��ô��....���Ѿ�����....
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

			//����ǽ�ͼ��Χ
			int nInterceptX = int(m_fUserDragW / (m_fCurPicProportion + m_fUserProportion));// + float(nW - m_stLastDrawSize.cx) / 2 + nDrawX);
			int nInterceptY = int(m_fUserDragH / (m_fCurPicProportion + m_fUserProportion));// + float(nH - m_stLastDrawSize.cy) / 2 + nDrawY);
			int nInterceptW = int(nDrawW / (m_fCurPicProportion + m_fUserProportion));
			int nInterceptH = int(nDrawH / (m_fCurPicProportion + m_fUserProportion));

			//��ͼ����st ��Χ��! ��ȡԭͼ��nInterceptX, nInterceptY, nInterceptW, nInterceptH ����ȥ��!
			//m_pGh->DrawImage(pImage, stDrawRect, nInterceptX, nInterceptY, nInterceptW, nInterceptH, UnitPixel);
			//pImage->BitBlt(m_hdcMemory, nDrawX, nDrawY, nDrawW, nDrawH, nInterceptX, nInterceptY, SRCPAINT);

			//CImage* imageE = new CImage;
			//imageE->Create(nInterceptW, nInterceptH, pImage->GetBPP());

			//CPoint points[3];
			//points[0] = CPoint(nInterceptX, nInterceptY);				//���Ͻ�
			//points[1] = CPoint(nInterceptX + nInterceptW, nInterceptY);	//���Ͻ�
			//points[2] = CPoint(nInterceptX, nInterceptY + nInterceptH);	//���½�
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

			//����ͼ
			ShowThumbnail(((float)nInterceptX / (float)pImage->GetWidth()), (float)nInterceptY / (float)pImage->GetHeight(), (float)nInterceptW / (float)pImage->GetWidth(), (float)nInterceptH / (float)pImage->GetHeight());
		}
		else
		{
			//������ͼ����nThumbnailPosX, nThumbnailPosY, nW, nH ��Χ��!
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

		//�ָ���־λ
		m_bCurImageChange = false;
	}

	//��������
	DrawToole();

	//����ҳ
	DrawTurnPageBtn();

	//������ͼ
	ShowThumnProportion();

	//�رհ�ťλ��
	m_TooleBtn[emCloseButton].m_curPoint.x = m_stLastDrawSize.cx - m_TooleBtn[emCloseButton].m_pImage->GetWidth() - 2;
	m_TooleBtn[emCloseButton].m_curPoint.y = 0 + 6;
	m_TooleBtn[emCloseButton].m_bDisplayState = true;

	//�رհ�ť����
	if (m_TooleBtn[emCloseBG].m_bDisplayState)
		DrawImage(m_TooleBtn[emCloseBG].m_pImage, m_TooleBtn[emCloseButton].m_curPoint.x - 4, m_TooleBtn[emCloseButton].m_curPoint.y - 3,\
		m_TooleBtn[emCloseBG].m_pImage->GetWidth(), m_TooleBtn[emCloseBG].m_pImage->GetHeight());

	//�رհ�ť
	DrawImage(m_TooleBtn[emCloseButton].m_pImage, m_TooleBtn[emCloseButton].m_curPoint.x, m_TooleBtn[emCloseButton].m_curPoint.y,\
		m_TooleBtn[emCloseButton].m_pImage->GetWidth(), m_TooleBtn[emCloseButton].m_pImage->GetHeight());

	//���ӹرհ�ť�ļ�ⷶΧ
	m_TooleBtn[emCloseButton].m_curPoint.x -= 10;
	m_TooleBtn[emCloseButton].m_curPoint.y += 10;
	m_TooleBtn[emCloseButton].m_wShowW = m_TooleBtn[emCloseButton].m_pImage->GetWidth() + 10;
	m_TooleBtn[emCloseButton].m_wShowH = m_TooleBtn[emCloseButton].m_pImage->GetHeight() + 10;

	//����������û����
	//ShowPicCountTips(NULL);

	//�����Ҽ��˵�
	ShowLBtnMenu();

	DeleteObject(hBitMap);

	return true;
}

//������
bool CCImageViewDlg::DrawBackGroundImage(int nWidth /*= 0*/, int nHeige /*= 0*/)
{
	//������ǰ�����
	//m_pGh->Clear(0);
	//Invalidate();

	//SetStretchBltMode(m_hdcTmp,HALFTONE);

	//����
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

			//��갴�¼�ʱ
			dwBeginTime = GetTickCount();
			m_bLeftButtonState = true;

			//�������λ��
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
			//������ˢ
			if(DrawAllImage())
				RePaintDialog();

			m_bIsDraging = false;
			m_bIsDragThumb = false;
			//OutMsg(L"���̧��,ȡ���϶�");
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
	//		//������ˢ
	//		if(DrawAllImage())
	//			RePaintDialog();
	//	}
	//	break;
	//case WM_LBUTTONDOWN:
	//	{
	//		SetCur();

	//		OnClickItem();

	//		m_bLeftButtonState = true;
	//		//�������λ��
	//		m_cDragTmp.x = GET_X_LPARAM(pMsg->lParam);
	//		m_cDragTmp.y = GET_Y_LPARAM(pMsg->lParam);
	//	}
	//	break;
	case WM_RBUTTONDOWN:
		{
			m_stLastRBtnPos.x = GET_X_LPARAM(pMsg->lParam);
			m_stLastRBtnPos.y = GET_Y_LPARAM(pMsg->lParam);

			m_TooleBtn[emRBtnMenu].m_bDisplayState = true;

			//������ˢ
			if(DrawAllImage())
				RePaintDialog();
		}
		break;
	case WM_NCRBUTTONDOWN:
		{
			m_stLastRBtnPos.x = GET_X_LPARAM(pMsg->lParam) - m_stLastDrawPoint.x;
			m_stLastRBtnPos.y = GET_Y_LPARAM(pMsg->lParam) - m_stLastDrawPoint.y;

			m_TooleBtn[emRBtnMenu].m_bDisplayState = true;

			//������ˢ
			if(DrawAllImage())
				RePaintDialog();
		}
		break;
	case WM_MOUSEMOVE:
		{
			SetCur();

			CheckRePaintDialog();
			ShowPicCountTips(pMsg);

			//ȫ�����߷Ŵ󳬹����ڳߴ��,�ǿͻ����ղ����κ���Ϣ��
			//��������ת��Ϊ�ͻ�����,����ȫ������϶���������
			if ((int)(pMsg->wParam << 31) >= 0 || !m_bShowThumbnail)
				break;

			if (m_bIsDragThumb)
				OutMsg(L"�����϶�����ͼ");

			if (m_bIsDraging)
				OutMsg(L"�����϶�ͼƬ");

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

			//�������λ��
			m_cDragTmp.x = GET_X_LPARAM(pMsg->lParam);
			m_cDragTmp.y = GET_Y_LPARAM(pMsg->lParam);

			//������ˢ
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
			_snprintf_s(szMsg, sizeof(szMsg), "ÿ�봥������ = %d \n", dwCount);
			OutputDebugStringA(szMsg);

			dwCount = 0;
			dwBTime = GetTickCount();
			}

			break;*/


			

			//���浱ǰ�������λ��
			/*m_cPoint.x = LOWORD(lParam);
			m_cPoint.y = HIWORD(lParam);*/

			//���Ӷ�ʱ��
			//SetTimer(2, 20, NULL);

			/*CPoint cPoint;
			GetCursorPos(&cPoint);*/

			

			//��������
			//m_cCurPoint = cPoint;

			/*char szMsg[64] = {0};
			_snprintf_s(szMsg, sizeof(szMsg), "ƫ����X = %d, ƫ����Y = %d\n", m_nUserDragH, m_nUserDragW);
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

//�����Ҫ�ػ�����
bool CCImageViewDlg::CheckReDraw()
{
	//����ǲ����б仯
	bool bTurnState = false;
	ToolButtunID emID = CheckItemPoint();

	//�жϷ�ҳ��ť�ǲ���Ҫ��ʾ,����Ҳ඼Ҫ��ʾ
	if (m_cCurPoint.x >= 0 && m_cCurPoint.y >= 0)
	{
		//���
		if (((UINT)m_cCurPoint.x <= m_TooleBtn[emBtnTurnPage].w()) && (m_cCurPoint.y > 0 && m_cCurPoint.y <= m_stLastDrawSize.cy))
		{
			bTurnState = true;
			m_bIsLeft = true;
		}
		//�Ҳ�
		else if ((m_cCurPoint.x <= m_stLastDrawSize.cx) && ((UINT)m_cCurPoint.x > (m_stLastDrawSize.cx - m_TooleBtn[emBtnTurnPage].w()))
			&& (m_cCurPoint.y > 0 && m_cCurPoint.y <= m_stLastDrawSize.cy))
		{
			bTurnState = true;
			m_bIsLeft = false;
		}
	}
	
	if (bTurnState != m_TooleBtn[emBtnTurnPage].m_bDisplayState)
	{
		//�б仯,�ػ�
		m_TooleBtn[emBtnTurnPage].m_bDisplayState = bTurnState;

		bTurnState = true;
	}
	else
		bTurnState = false;

	//������״̬
	bool bToolState = false;
	//��鹤����,ֻҪ����ڴ����ھ���ʾ
	if (m_cCurPoint.x >= 0 && m_cCurPoint.x <= m_stLastDrawSize.cx && m_cCurPoint.y >= 0 && m_cCurPoint.y <= m_stLastDrawSize.cy)
		bToolState = true;
	
	if (bToolState != m_TooleBtn[emToolButtons].m_bDisplayState)
	{
		m_TooleBtn[emToolButtons].m_bDisplayState = bToolState;

		bToolState = true;
	}
	else
		bToolState = false;

	//����ڹ�������Χ�ڻ�Ҫ��鰴ť
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

	//���رհ�ť
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

	//�������ͼ�رհ�ť   
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

	//���������ʾ
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

	//����Ҽ��˵�
	bool bRBtnMenu = false;
	if (emRBtnMenu == emID && CheckRBtnMenuItem() != m_emLastMRtnIndex)
		bRBtnMenu = true;
	else if (emRBtnMenu != emID && emRBtnMax != m_emLastMRtnIndex)
		bRBtnMenu = true;

	return bToolState || bTurnState || bBottomState || bCloseBG || bThumCloseBG || bPicCountTips || bRBtnMenu;
}

//��������
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
		//����
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
		//1:1����һ�
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

//����ҳ��ť
bool CCImageViewDlg::DrawTurnPageBtn()
{
	if (!m_TooleBtn[emBtnTurnPage].m_bDisplayState)
		return true;

	//����Ҫ��ʾ��ʱ����Ҳû��ʾ
	if (!m_bShowTrunPage && emDontShow == m_TooleBtn[emBtnTurnPage].m_bDisplayState)
		return true;

	//���ұߵ�ʱ��Ҫ����X
	m_TooleBtn[emBtnTurnPage].m_curPoint.x = m_bIsLeft ? 0 : m_stLastDrawSize.cx - m_TooleBtn[emBtnTurnPage].w();
	m_TooleBtn[emBtnTurnPage].m_curPoint.y = m_stLastDrawSize.cy / 2 - m_TooleBtn[emBtnTurnPage].m_pImage->GetHeight() / 2;

	CImage* pImage = m_TooleBtn[emBtnTurnPage].m_pImage;
	if (!m_bIsLeft)
	{
		//��תͼ��
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

//�л�ͼƬ
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

		//���ͼƬ�Ѿ��л����ػ�
		m_bCurImageChange = true;
		//�������ű�
		m_fUserProportion = 0.0;
		//m_fDIsplayProportion = 1.0;
		m_bUseChangeProportion = false;

		//�����һ��ԭͼ
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

		//���·�ҳ��ʾ
		m_cTip.DelTool(this);
		ShowPicCountTips(NULL);
	}

	return true;
}
void CCImageViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent)
	{
	case emTimerOutOfWimCheck:
		{
			//����Ƿ�����������֮��,���������������֮��,�������ر�
			

			//����ػ�
			CheckRePaintDialog();
		}
		break;
	case emTimerProportionBG:
		{
			m_bProportionState = false;
			if (DrawAllImage())
				RePaintDialog();

			//ɾ����ʱ��
			KillTimer(emTimerProportionBG);
		}
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//��⵱ǰλ��	
ToolButtunID CCImageViewDlg::CheckItemPoint()
{
	//�������λ��
	UpdateCurPoint();

	for (short w = 0; w < emMax; ++w)
	{
		/*if (w == emThumbnail)
		{
			OutMsg(L"�������x=%d,y=%d", m_cCurPoint.x, m_cCurPoint.y);
			OutMsg(L"����ͼ��Χx=[%d-%d],y=[%d-%d]", m_TooleBtn[w].m_curPoint.x, m_TooleBtn[w].m_curPoint.x + m_TooleBtn[w].w(), m_TooleBtn[w].m_curPoint.y, m_TooleBtn[w].m_curPoint.y + m_TooleBtn[w].h());
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
	//�������λ��
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
	//�������λ��
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

//������Ӧ
ToolButtunID CCImageViewDlg::OnClickItem()
{
	ToolButtunID emID = CheckItemPoint();

	/*char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "id = %d, time = %d\n", emID, dwTickCount);
	OutputDebugStringA(szMsg);*/

	switch (emID)
	{
	case emBgImage:		//����ͼƬ
		break;
	case emBtnTurnPage:	//��ҳ��ť
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
		//OutputDebugStringA("�������ͼ\n");
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

			//���سɹ���,ȷ�����ڴ�С...
			m_stLastDrawSize.cx = nW < m_stCurScreenSize.cx ? nW : m_stCurScreenSize.cx;
			m_stLastDrawSize.cy = nH < m_stCurScreenSize.cy ? nH : m_stCurScreenSize.cy;

			m_stLastDrawSize.cx = m_stLastDrawSize.cx > DEFAULT_WINDOW_SIZE_W ? m_stLastDrawSize.cx : DEFAULT_WINDOW_SIZE_W;
			m_stLastDrawSize.cy = m_stLastDrawSize.cy > DEFAULT_WINDOW_SIZE_H ? m_stLastDrawSize.cy : DEFAULT_WINDOW_SIZE_H;

			//�޸Ĵ��ڴ�С
			//::MoveWindow(m_hWnd, 0, 0, m_stCurScreenSize.cx, m_stCurScreenSize.cy, FALSE);

			//���¼���λ��...����ˢ�´���
			int nX = (m_stCurScreenSize.cx - m_stLastDrawSize.cx) / 2;
			int nY = (m_stCurScreenSize.cy - m_stLastDrawSize.cy) / 2;

			//���¼���ͼƬλ��
			SetUserDragH(0);
			SetUserDragW(0);
			if (nW > m_stLastDrawSize.cx)
				SetUserDragW(float(nW - m_stLastDrawSize.cx) / 2);
			if (nH > m_stLastDrawSize.cy)
				SetUserDragH(float(nH - m_stLastDrawSize.cy) / 2);

			m_fThunmbClockX = 0;
			m_fThunmbClockY = 0;

			m_bIsFullScreen = false;

			//����϶����
			/*SetUserDragH(0);
			SetUserDragW(0);*/

			if(DrawAllImage())
				RePaintDialog(0, 0, nX, nY);

			m_bOriginalSize = true;

			return emReduction;

			//������Խ���һ��һ����
			//������ͼƬ�����ڳߴ�ﵽ����Ļ,��ô�Ϳ������ڼ��ص�������ͼ
			//if (nW >= m_stCurScreenSize.cx || nH >= m_stCurScreenSize.cy)
			//{
			//	//��������������ͼ,����ȥ��ȡԭͼ
			//	/*if (CLoadThread::Instance().GetCurFullPic(m_pCurImageFull))
			//	{*/
			//		nW = m_pCurImage->GetWidth();
			//		nH = m_pCurImage->GetHeight();

			//		//���سɹ���,ȷ�����ڴ�С...
			//		m_stLastDrawSize.cx = nW < m_stCurScreenSize.cx ? nW : m_stCurScreenSize.cx;
			//		m_stLastDrawSize.cy = nH < m_stCurScreenSize.cy ? nH : m_stCurScreenSize.cy;

			//		//���¼���λ��...����ˢ�´���
			//		int nX = (m_stCurScreenSize.cx - m_stLastDrawSize.cx) / 2;
			//		int nY = (m_stCurScreenSize.cy - m_stLastDrawSize.cy) / 2;

			//		//���зŴ����!!!
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
			//	//�����ֱ��1:1��
			//	//�����ߴ绹�ǲ���С��Ĭ��ֵ
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
	case emFullScreen:			//ȫ��
		{
			if (m_bIsFullScreen)
			{
				m_stLastDrawSize = m_stTempSize;
				m_stLastDrawPoint = m_stTempPoint;

				//�޸Ĵ��ڴ�С
				::MoveWindow(m_hWnd, 0, 0, m_stCurScreenSize.cx, m_stCurScreenSize.cy, FALSE);
			}
			else
			{
				m_stTempSize = m_stLastDrawSize;
				m_stTempPoint = m_stLastDrawPoint;

				//�޸Ĵ��ڴ�С
				::MoveWindow(m_hWnd, 0, 0, m_stCurScreenSize.cx, m_stCurScreenSize.cy, FALSE);
			}

			m_bCurImageChange = true;

			m_bIsFullScreen = !m_bIsFullScreen;

			if(DrawAllImage())
				RePaintDialog(0, 0, m_stTempPoint.x, m_stTempPoint.y);

			m_bOriginalSize = false;
		}
		break;
	case emRotation:			//��ת
		{
			if (!m_pCurImage->IsNull())
			{
				//m_pCurImage->RotateFlip(Rotate90FlipNone);
				CImage* pImage = new CImage;
				ImageRotate(pImage, m_pCurImage);
				
				m_cRotateNum++;

				//�ͷ�ԭ������Դ
				ClearRotateTemp(1);

				m_pCurImage = pImage;

				m_bCurImageChange = true;

				//��������ͼ
				m_bUseChangeProportion = false;
				m_fUserProportion = 0.0;
				//m_bUseFullPic = false;

				if(DrawAllImage())
					RePaintDialog(/*0, 0, m_stTempPoint.x, m_stTempPoint.y*/);

				m_bOriginalSize = false;
			}
		}
		break;
	case emSave:				//����
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

//����ػ�
ToolButtunID CCImageViewDlg::CheckRePaintDialog()
{
	//�������λ��
	UpdateCurPoint();
	
	//���ж��ǲ������ڼ��ص�ǰ�ļ�
	if (!m_bLoadSucess)
		TrunPage();

	//�ж�Ҫ��Ҫ�ػ�
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

//�Ŵ���С, Ĭ����1:1����
bool CCImageViewDlg::ChangeArea(char cIsAdd /*= 0*/)
{
	//ֱ�Ӽ���ԭͼ
	//if (m_fCurPicProportion < 1.0 && !m_pCurImageFull && CLoadThread::Instance().GetCurFullPic(m_pCurImageFull))
	//{
	//	OutputDebugStringA("����ͼ���سɹ�!\n");

	//	//���سɹ�����ͼ��,�������ű�
	//	//m_fCurPicProportion *= m_fDIsplayProportion;

	//	//�������ű�
	//}

	//�û����������ű���
	if (cIsAdd)
		m_bUseChangeProportion = true;


	//����ǰ�������ĵ�
	float fDisplayW = m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion);
	float fDisplayH = m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion);

	//float fCenterX = fDisplayW - m_fUserDragW - min(fDisplayW, m_stLastDrawSize.cx) / 2;
	//float fCenterY = fDisplayH - m_fUserDragH - min(fDisplayH, m_stLastDrawSize.cy) / 2;

	//����ǰҲ������ԭ����
	//float fProportion = m_fUserProportion;

	//���Ų���
	m_fUserProportion += float(cIsAdd * (DEFAULT_PROPORTION + m_fUserProportion * 0.1));

	//char szMsg[64] = {0};
	//_snprintf_s(szMsg, sizeof(szMsg), "m_fUserProportion = %f \n", m_fUserProportion);
	//OutputDebugStringA(szMsg);

	if ((m_fCurPicProportion + m_fUserProportion) > DEFAULT_MAX_ZOOM)
		m_fUserProportion = DEFAULT_MAX_ZOOM - m_fCurPicProportion;
	else if ((m_fCurPicProportion + m_fUserProportion) < DEFAULT_MAX_NARROW)
		m_fUserProportion = DEFAULT_MAX_NARROW - m_fCurPicProportion;


	//���ź��������
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

		////������ƫ����,��֤�Ŵ�����ĵ㲻��ƫ��
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
	//������ʧ�Ķ�ʱ��,��ɾ��ԭ����������
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

	//�Ŵ���С֮ǰ���浱ǰ���ĵ�,�Դ�Ϊ�����������

	//m_fDIsplayProportion += float(DEFAULT_PROPORTION * (float)cIsAdd);

	//ʹ��ԭͼ
	//if (!m_bUseFullPic && m_fDIsplayProportion >= 1.0 && m_fCurPicProportion < 1.0 && m_pCurImageFull)
	//{
	//	m_bUseFullPic = true;
	//	OutputDebugStringA("ʹ��ԭͼ!!!!\n");

	//	//��Ϊԭͼ�����ű���
	//	m_fDIsplayProportion = m_fCurPicProportion;
	//}

	//���ŵ�һ����С��������ͼ
	//if (m_bUseFullPic && m_pCurImageFull && m_fCurPicProportion \
	//	&& m_pCurImageFull->GetWidth() * m_fDIsplayProportion < m_pCurImage->GetWidth()\
	//	&& m_pCurImageFull->GetHeight() * m_fDIsplayProportion < m_pCurImage->GetHeight())
	//{
	//	m_bUseFullPic = false;

	//	//��������ͼ
	//	m_fDIsplayProportion = 1.0;

	//	OutputDebugStringA("��������ͼ!!!!\n");
	//}

	/*m_fUserProportion += float(DEFAULT_PROPORTION * (float)cIsAdd);

	if (m_fUserProportion > DEFAULT_MAX_ZOOM)
		m_fUserProportion = DEFAULT_MAX_ZOOM;
	else if (m_fUserProportion < DEFAULT_MAX_NARROW)
		m_fUserProportion = (float)DEFAULT_MAX_NARROW;*/

	/*m_bCurImageChange = true;

	char szMsg[64] = {0};
	_snprintf_s(szMsg, sizeof(szMsg), "��ǰ���ű� : %f \n", m_fCurPicProportion + m_fUserProportion);
	OutputDebugStringA(szMsg);*/

	return true;
}

//�����������
void CCImageViewDlg::UpdateCurPoint()
{
	//��ȡ��Ӧ������
	GetCursorPos( &m_cPoint );
	GetWindowRect(&m_stRct);

	//��ǰ�������λ��
	m_cCurPoint.x = m_cPoint.x - m_stRct.left;
	m_cCurPoint.y = m_cPoint.y - m_stRct.top;
}

//�������ͺ�·��
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

//������ת����
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

	// ������Դ
	HRSRC hRsrc =::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), L"png");
	if(hRsrc == NULL)
		return false;

	// ������Դ
	HGLOBAL hImgData =::LoadResource(AfxGetResourceHandle(), hRsrc);
	if(hImgData == NULL)
	{
		::FreeResource(hImgData);
		return false;
	}

	// �����ڴ��е�ָ����Դ
	LPVOID lpVoid =::LockResource(hImgData); 
	LPSTREAM pStream = NULL;
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize); 
	// ����ڴ��е�ָ����Դ
	::GlobalUnlock(hNew);

	// ��ָ���ڴ洴��������
	HRESULT ht =::CreateStreamOnHGlobal(hNew,TRUE,&pStream);
	if( ht != S_OK )
	{
		GlobalFree(hNew);
	}
	else
	{
		// ����ͼƬ
		pCImg->Load(pStream); 
		GlobalFree(hNew);
	}
	// �ͷ���Դ
	::FreeResource(hImgData);
	return true;
}

//��ʾ���ű���
void CCImageViewDlg::ShowThumnProportion()
{
	if (!m_TooleBtn[emProportionBG].m_pImage || m_TooleBtn[emProportionBG].m_pImage->IsNull())
		return;

	if (!m_bProportionState)
		return;

	//����λ��
	int nX = (m_stLastDrawSize.cx - m_TooleBtn[emProportionBG].m_pImage->GetWidth()) / 2;
	int nY = (m_stLastDrawSize.cy - m_TooleBtn[emProportionBG].m_pImage->GetHeight()) / 2;

	DrawImage(m_TooleBtn[emProportionBG].m_pImage, nX, nY);

	//д��
	CDC* pDc = CDC::FromHandle(m_hdcMemory);
	CFont cFont;
	cFont.CreatePointFont(DEFAULT_PRO_FONT_SIZE * 10, L"΢���ź�");

	//pDc->SetTextCharacterExtra(2);//�����ַ����
	pDc->SetBkMode(TRANSPARENT);  //���ñ���͸��
	pDc->SetTextColor(RGB(255,255,255));
	pDc->SelectObject(&cFont);

	TEXTMETRIC tm; 
	pDc->GetTextMetrics(&tm);

	//����ƽ��
	//m_pGh->SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	
	wchar_t szProp[16] = {0};
	_snwprintf_s(szProp, sizeof(szProp), L"%d%%", int((m_fCurPicProportion + m_fUserProportion) * 100));

	nY += (m_TooleBtn[emProportionBG].m_pImage->GetHeight() - (tm.tmHeight+tm.tmExternalLeading)) / 2;
	nX += (m_TooleBtn[emProportionBG].m_pImage->GetWidth() - ((wcslen(szProp) + 1) * tm.tmAveCharWidth)) / 2;

	//m_pGh->DrawString(szProp, wcslen(szProp), );
	pDc->TextOut(nX, nY, szProp);
}

//�������ָ��
void CCImageViewDlg::SetCur()
{
	if (emBgImage == CheckItemPoint())
		SetCursor(m_hCurNormal);
}

//�϶�ͼƬ
void CCImageViewDlg::DragImage(LPARAM lParam)
{
	/*m_fUserDragW += m_cDragTmp.x - GET_X_LPARAM(lParam);
	m_fUserDragH += m_cDragTmp.y - GET_Y_LPARAM(lParam);

	m_nThunmbClockX += (m_cDragTmp.x - GET_X_LPARAM(lParam)) / ((m_pCurImage->GetWidth() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosX);
	m_nThunmbClockY += (m_cDragTmp.y - GET_Y_LPARAM(lParam)) / ((m_pCurImage->GetHeight() * (m_fCurPicProportion + m_fUserProportion)) / m_nCurThumbPosY);*/

	SetUserDragW(m_fUserDragW + m_cDragTmp.x - GET_X_LPARAM(lParam));
	SetUserDragH(m_fUserDragH + m_cDragTmp.y - GET_Y_LPARAM(lParam));
}

//�϶�����ͼ
void CCImageViewDlg::DragThumb(LPARAM lParam)
{
	//��һ��
	/*m_fUserDragW -= (int)(float(m_cDragTmp.x - GET_X_LPARAM(lParam)) / ((float)m_TooleBtn[emThumbnail].w() / (float)m_pCurImage->GetWidth()));
	m_fUserDragH -= (int)(float(m_cDragTmp.y - GET_Y_LPARAM(lParam)) / ((float)m_TooleBtn[emThumbnail].h() / (float)m_pCurImage->GetHeight()));*/

	//�ڶ���
	/*m_fUserDragW -= (int)(float(m_cDragTmp.x - GET_X_LPARAM(lParam)) * (m_fCurPicProportion + m_fUserProportion));
	m_fUserDragH -= (int)(float(m_cDragTmp.y - GET_Y_LPARAM(lParam)) * (m_fCurPicProportion + m_fUserProportion));*/

	//������
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

	//����
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

	//����
	if (val < 0)
		val = 0;
	else if (val > (fDisplayH - min(m_stLastDrawSize.cy, fDisplayH)))
		val = fDisplayH - min(m_stLastDrawSize.cy, fDisplayH);

	if (m_nCurThumbPosH)
		m_fThunmbClockY = val * m_nCurThumbPosH / fDisplayH + m_nCurThumbPosY;

	m_fUserDragH = val; 
}

//��ʾ��ҳ������
void CCImageViewDlg::ShowPicCountTips(MSG* pMsg)
{
	if (!m_bShowTips)
	{
		m_cTip.DelTool(this);
		return;
	}

	//��....׼����Ҫд����
	wchar_t szTips[64] = {0};
	_snwprintf_s(szTips, sizeof(szTips), L"%d / %d", m_nCurPicIndex + 1, m_nAllPicCount);
	/*bool bIsNumber = false;
	if (0 == m_nCurPicIndex && m_bIsLeft)
		_snwprintf_s(szTips, sizeof(szTips), L"�Ѿ��ǵ�һ��.");
	else if (m_nAllPicCount == (m_nCurPicIndex + 1) && !m_bIsLeft)
		_snwprintf_s(szTips, sizeof(szTips), L"�Ѿ������һ����.");
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
	HDC memDC = CreateCompatibleDC(*hdc);    // ��ͼ�ڴ�DC
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
	// TODO: �ڴ���������������
	m_cLoadThread.SaveImageToFile(m_nCurPicIndex);
}

//�Ҽ��˵�
void CCImageViewDlg::ShowLBtnMenu()
{
	if (!m_TooleBtn[emRBtnMenu].m_bDisplayState)
		return;

	//�����ڴ�����
	if ((m_stLastRBtnPos.x + m_TooleBtn[emRBtnMenu].m_pImage->GetWidth()) > m_stLastDrawSize.cx)
		m_stLastRBtnPos.x = m_stLastDrawSize.cx - m_TooleBtn[emRBtnMenu].m_pImage->GetWidth();
	if ((m_stLastRBtnPos.y + m_TooleBtn[emRBtnMenu].m_pImage->GetHeight()) > m_stLastDrawSize.cy)
		m_stLastRBtnPos.y = m_stLastDrawSize.cy - m_TooleBtn[emRBtnMenu].m_pImage->GetHeight();

	m_TooleBtn[emRBtnMenu].m_curPoint = m_stLastRBtnPos;
	DrawImage(m_TooleBtn[emRBtnMenu].m_pImage, m_stLastRBtnPos.x, m_stLastRBtnPos.y);

	//������
	m_emLastMRtnIndex =  CheckRBtnMenuItem();
	if (m_emLastMRtnIndex != emRBtnMax)
	{
		//����
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

//������Ӧ
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

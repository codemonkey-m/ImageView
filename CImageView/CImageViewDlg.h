// CImageViewDlg.h : ͷ�ļ�
//

#pragma once

// CCImageViewDlg �Ի���
class CCImageViewDlg : public CDialog
{
// ����
public:
	CCImageViewDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = 102 };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	bool Init();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnCancel();
private:
	//�����߳�
	CLoadThread		m_cLoadThread;

	//GDI+
	ULONG_PTR		m_ldiplusToken;

	//User32dll
	//HMODULE			m_hUser32Dll;

	//UpdateLayeredWindow
	UDLWindow		m_hUpdateLayeredWindow;

	//....
	BLENDFUNCTION	m_Blend;

	HDC				m_hdcMemory;
	HDC				m_hdcTmp;
	Graphics		*m_pGh;

	//������һ�λ��ƵĴ�С,λ��
	SIZE			m_stLastDrawSize;
	POINT			m_stLastDrawPoint;

	//ȫ��ʱ��������Ĵ�С��λ��
	SIZE			m_stTempSize;
	POINT			m_stTempPoint;

	//���ڴ�С
	RECT			m_stRct;
	//��ǰ������λ��
	CPoint			m_cPoint;
	//��ǰ������λ��
	POINT			m_cCurPoint;
	//�϶�ǰ��¼�����λ��
	CPoint			m_cDragTmp;
	//��Ļ�ߴ�
	SIZE			m_stCurScreenSize;

	//��ǰ��ʾ��ͼƬ
	CImage			*m_pCurImage;

	//��ת����
	BYTE			m_cRotateNum;

	//��ǰͼƬ��ԭͼ
	//CImage			*m_pCurImageFull;
	//��ǰͼƬ�ṹ
	//ImageInfo		m_stCurImageInfo;

	//���سɹ����
	bool			m_bLoadSucess;

	//��ǰͼƬ�ļ������ű�,�����ԭͼ
	float			m_fCurPicProportion;
	//��ǰͼƬ����ʾ���ű�,����ڵ�һ�ε�����ͼ
	//float			m_fDIsplayProportion;
	//������ԭͼ�����ű�,��ǰ����ͼ�����ԭͼ��
	//float			m_fTmpProportion;
	//�û��������ű�,����
	float			m_fUserProportion;

	//�Ƿ���ʾ����
	bool			m_bProportionState;

	//�Ƿ�ʹ��ԭͼ
	//bool			m_bUseFullPic;
	//�û����������ű���
	bool			m_bUseChangeProportion;

	//�û��϶�ƫ����
	float			m_fUserDragW;
	float			m_fUserDragH;

	//���浱ǰ����ȡ��ͼƬλ��,Ϊ��������ͼ�ϻ��Ǹ��߿�

	//�����϶���ʶ
	bool			m_bIsDraging;
	//�����϶�����ͼ��ʶ
	bool			m_bIsDragThumb;

	//���Ų�����������������ʾ����ͼ���ҿ����϶�ͼƬ
	bool			m_bShowThumbnail;
	//�û��ر�����ͼ���
	bool			m_bUserCloseThumb;

	//�ػ���
	bool			m_bCurImageChange;

	//���߰�ť����
	TooleButton		m_TooleBtn[emMax];

	//����ͼ��
	map<UINT, CImage*>	m_mapIcons;

	//�Ƿ���ʾ��ҳ��ť
	bool			m_bShowTrunPage;

	//��ǰ�ļ��б�
	vector<CString>	m_vtFile;
	//��ǰĿ¼
	CString			m_strCurrentDirectory;
	//��ǰ��ʾ��ͼƬ����
	int				m_nCurPicIndex;
	//��ͼƬ����
	int				m_nAllPicCount;

	//��һ�ε���İ�ť
	BottomBtnID		m_emLastClickButton;

	//���淭ҳ��ť���ĸ�
	bool			m_bIsLeft;

	//������±��
	bool			m_bLeftButtonState;

	//ȫ�����
	bool			m_bIsFullScreen;
	//1:1���
	bool			m_bOriginalSize;

	//�������ͺ�·��
	LoadType		m_emLoadType;
	wstring			m_strLoadPath;
	vector<wstring> m_vtLoadFileList;

	void*			m_pLoadPath;

	HCURSOR			m_hCurNormal;

	float			m_fThunmbClockX;
	float			m_fThunmbClockY;
	float			m_fThunmbClockW;
	float			m_fThunmbClockH;

	int				m_nCurThumbPosX;
	int				m_nCurThumbPosY;
	int				m_nCurThumbPosW;
	int				m_nCurThumbPosH;

	//��ҳ��ťtips
	CToolTipCtrl	m_cTip;
	bool			m_bShowTips;

	//�����Ҽ�λ��
	POINT			m_stLastRBtnPos;
	//������������Ҽ��˵�λ��
	RMenuBtnID		m_emLastMRtnIndex;

	//��һ�λ�ȡ���ߴ���
	bool			m_bSizeFirst;

	//�Ƿ���ʵ��������
	bool			m_bIsRuning;

	//
public:	
	bool GetIsRuning() const { return m_bIsRuning; }
	void SetIsRuning(bool val) { m_bIsRuning = val; }
public:	
	void SetThunmbClockX(float val);
	void SetThunmbClockY(float val);
	void SetUserDragW(float val);
	void SetUserDragH(float val);
public:
	//�������ͺ�·��
	void SetLoadTypeAndPath(LoadType emType, const void* pSource);

	//��ʾ����ͼ
	void ShowThumbnail(float fPosX, float fPosY, float fWidth, float fHeight);

	//��ʾ���ű���
	void ShowThumnProportion();

	//��ʾ��ҳ������
	void ShowPicCountTips(MSG* pMsg);

	//�Ҽ��˵�
	void ShowLBtnMenu();

	//ˢ�´���
	void RePaintDialog(int nWidth = 0, int nHeight = 0, int nPosX = -1, int nPosY = -1);

	//�����Ļ�ͼ
	bool DrawImage(CImage* pImage, int nPosX = 0, int nPosY = 0, int nWidth = 0, int nHeight = 0, int nXScr = 0, int nYSrc = 0, int nSrcWidth = 0, int nSrcHeight = 0);

	//��ȫ��ͼ
	bool DrawAllImage();

	//������
	bool DrawBackGroundImage(int nWidth = 0, int nHeige = 0);

	//��������
	bool DrawToole();

	//����ҳ��ť
	bool DrawTurnPageBtn();

	//�����Ҫ�ػ�����
	bool CheckReDraw();

	//�л�ͼƬ
	bool TrunPage(char cIsAdd = 0, bool bInit = false);

	//�Ŵ���С, Ĭ����1:1����
	bool ChangeArea(char cIsAdd = 0);

	//��⵱ǰλ��
	ToolButtunID CheckItemPoint();
	BottomBtnID CheckBottomItem();
	RMenuBtnID CheckRBtnMenuItem();

	//������Ӧ
	ToolButtunID OnClickItem();
	BottomBtnID OnClickBottom();
	RMenuBtnID OnClickRBtnMenuItem();

	//����ػ�
	ToolButtunID CheckRePaintDialog();

	//�����������
	void UpdateCurPoint();

	//������ת����
	void ClearRotateTemp(BYTE bCount = 0);

	//����Դ����PNG
	bool LoadPngForRes( CImage* pCImg, UINT nResID );

	//�������ָ��
	void SetCur();

	//�϶�ͼƬ
	void DragImage(LPARAM lParam);
	//�϶�����ͼ
	void DragThumb(LPARAM lParam);

	//������Ӧ
	void OnRBtnKeyDown(MSG* pMsg);

	//���Ƶ����а�
	void OnClickCopy();
	//���ֵ��ļ�
	void OnClickSaveToFile();

public:
	//������ɫ��ʼ��
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//�϶���Ӧ
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

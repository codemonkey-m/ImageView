// CImageViewDlg.h : 头文件
//

#pragma once

// CCImageViewDlg 对话框
class CCImageViewDlg : public CDialog
{
// 构造
public:
	CCImageViewDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = 102 };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	bool Init();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnCancel();
private:
	//加载线程
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

	//保存上一次绘制的大小,位置
	SIZE			m_stLastDrawSize;
	POINT			m_stLastDrawPoint;

	//全屏时缓存上面的大小和位置
	SIZE			m_stTempSize;
	POINT			m_stTempPoint;

	//窗口大小
	RECT			m_stRct;
	//当前鼠标绝对位置
	CPoint			m_cPoint;
	//当前鼠标相对位置
	POINT			m_cCurPoint;
	//拖动前记录的鼠标位置
	CPoint			m_cDragTmp;
	//屏幕尺寸
	SIZE			m_stCurScreenSize;

	//当前显示的图片
	CImage			*m_pCurImage;

	//旋转次数
	BYTE			m_cRotateNum;

	//当前图片的原图
	//CImage			*m_pCurImageFull;
	//当前图片结构
	//ImageInfo		m_stCurImageInfo;

	//加载成功标记
	bool			m_bLoadSucess;

	//当前图片的加载缩放比,相对于原图
	float			m_fCurPicProportion;
	//当前图片的显示缩放比,相对于第一次的缩略图
	//float			m_fDIsplayProportion;
	//缓存下原图的缩放比,当前缩略图相对于原图的
	//float			m_fTmpProportion;
	//用户操作缩放比,绝对
	float			m_fUserProportion;

	//是否显示比例
	bool			m_bProportionState;

	//是否使用原图
	//bool			m_bUseFullPic;
	//用户调整了缩放比例
	bool			m_bUseChangeProportion;

	//用户拖动偏移量
	float			m_fUserDragW;
	float			m_fUserDragH;

	//保存当前画截取的图片位置,为了在缩略图上画那个边框

	//正在拖动标识
	bool			m_bIsDraging;
	//正在拖动缩略图标识
	bool			m_bIsDragThumb;

	//缩放操作到超过窗口则显示缩略图并且可以拖动图片
	bool			m_bShowThumbnail;
	//用户关闭缩略图标记
	bool			m_bUserCloseThumb;

	//重绘标记
	bool			m_bCurImageChange;

	//工具按钮数组
	TooleButton		m_TooleBtn[emMax];

	//工具图标
	map<UINT, CImage*>	m_mapIcons;

	//是否显示翻页按钮
	bool			m_bShowTrunPage;

	//当前文件列表
	vector<CString>	m_vtFile;
	//当前目录
	CString			m_strCurrentDirectory;
	//当前显示的图片索引
	int				m_nCurPicIndex;
	//总图片数量
	int				m_nAllPicCount;

	//上一次点击的按钮
	BottomBtnID		m_emLastClickButton;

	//保存翻页按钮是哪个
	bool			m_bIsLeft;

	//左键按下标记
	bool			m_bLeftButtonState;

	//全屏标记
	bool			m_bIsFullScreen;
	//1:1标记
	bool			m_bOriginalSize;

	//加载类型和路径
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

	//翻页按钮tips
	CToolTipCtrl	m_cTip;
	bool			m_bShowTips;

	//缓存右键位置
	POINT			m_stLastRBtnPos;
	//缓存鼠标所在右键菜单位置
	RMenuBtnID		m_emLastMRtnIndex;

	//第一次获取到尺寸标记
	bool			m_bSizeFirst;

	//是否有实例在运行
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
	//设置类型和路径
	void SetLoadTypeAndPath(LoadType emType, const void* pSource);

	//显示缩略图
	void ShowThumbnail(float fPosX, float fPosY, float fWidth, float fHeight);

	//显示缩放比例
	void ShowThumnProportion();

	//显示翻页处数量
	void ShowPicCountTips(MSG* pMsg);

	//右键菜单
	void ShowLBtnMenu();

	//刷新窗口
	void RePaintDialog(int nWidth = 0, int nHeight = 0, int nPosX = -1, int nPosY = -1);

	//单纯的画图
	bool DrawImage(CImage* pImage, int nPosX = 0, int nPosY = 0, int nWidth = 0, int nHeight = 0, int nXScr = 0, int nYSrc = 0, int nSrcWidth = 0, int nSrcHeight = 0);

	//画全部图
	bool DrawAllImage();

	//画背景
	bool DrawBackGroundImage(int nWidth = 0, int nHeige = 0);

	//画工具栏
	bool DrawToole();

	//画翻页按钮
	bool DrawTurnPageBtn();

	//检查需要重绘的组件
	bool CheckReDraw();

	//切换图片
	bool TrunPage(char cIsAdd = 0, bool bInit = false);

	//放大缩小, 默认做1:1处理
	bool ChangeArea(char cIsAdd = 0);

	//检测当前位置
	ToolButtunID CheckItemPoint();
	BottomBtnID CheckBottomItem();
	RMenuBtnID CheckRBtnMenuItem();

	//单击相应
	ToolButtunID OnClickItem();
	BottomBtnID OnClickBottom();
	RMenuBtnID OnClickRBtnMenuItem();

	//检查重绘
	ToolButtunID CheckRePaintDialog();

	//更新鼠标坐标
	void UpdateCurPoint();

	//清理旋转缓存
	void ClearRotateTemp(BYTE bCount = 0);

	//从资源加载PNG
	bool LoadPngForRes( CImage* pCImg, UINT nResID );

	//设置鼠标指针
	void SetCur();

	//拖动图片
	void DragImage(LPARAM lParam);
	//拖动缩略图
	void DragThumb(LPARAM lParam);

	//按键响应
	void OnRBtnKeyDown(MSG* pMsg);

	//复制到剪切板
	void OnClickCopy();
	//保持到文件
	void OnClickSaveToFile();

public:
	//窗体颜色初始化
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//拖动相应
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

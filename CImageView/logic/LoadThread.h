#pragma once
class CLoadThread
{
public:
	CLoadThread(void);
	~CLoadThread(void);

	static CLoadThread &Instance()
	{
		static CLoadThread m_cLoad;
		return m_cLoad;
	}

public:
	short StartThread(SIZE stScreen);
	void StopThread();

	bool LoadFile(LoadType emType, void* pSource, LONG nW = DEFAULT_WINDOW_SIZE_W, LONG nH = DEFAULT_WINDOW_SIZE_H);

	//加载指定文件夹的所有文件名到列表
	bool LoadAllFileNameFromDir();

	//判断一个文件拓展名是否支持
	bool IsImage(CString pFileName);

	//加载列表里的部分文件的缩略图
	bool LoadImageFromList();

	//在列表里获取一个图片
	bool GetImageFromList(CImage* &pImage, int& nIndex, SIZE* &stScreenSize, int* pAllCount);

	//获取当前完整图片
	bool GetCurFullPic(Image* &pImage);

	//保存一幅图片
	bool SaveImageToFile(UINT nIndex);

	//加载一幅图片的完整图
	bool LoadFullFile();

	//根据文件名获取文件索引
	int GetIndexFromFileName(wstring strFileName);

	//设置初始窗口大小
	void SetInitWindowSize(int nW, int nH);
private:
	//当前线程状态
	bool m_bThreadState;

	//CWinThread*			m_pThread;

	CThreadLockEx		m_cLock;

	//当前图片的完整图
	Image*				m_pCurFullPic;

	//当前图片索引
	int					m_nCurPicIndex;

	//标记要预加载上一张还是下一张
	int					m_nLoadImagePos;

	//当前图片加载成功标记
	bool				m_bCurPicLoadSuccess;

	//标记是否切换了图片
	bool				m_bCurPicIsChange;

	//当前窗口大小
	SIZE				m_stCurWinSize;

	//当前屏幕大小
	SIZE				m_stCurScreenSize;

	//当前加载类型
	LoadType			m_emLoadType;

	//当前是否有扫描任务
	bool				m_bInScaning;

	//当前是否有加载任务
	bool				m_bInLoading;

	//第一次加载标记
	bool				m_bFirst;

	//当前目录
	wstring				m_strCurrentDirectory;
	//初始化的文件名
	wstring				m_strInitFileName;

	//文件列表
	vector<wstring>		m_vtFiles;
	//缩略图列表
	list<ImageInfo>		m_listThumbnai;
	ImageInfo* Find(int nIndex);

	bool CompressImagePixel(Image* pSource, Image* &pOut, UINT ulNewHeigth, UINT ulNewWidth);
	bool GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid);
public:	
	bool GetThreadState() const { return m_bThreadState; }
	void SetThreadState(bool val) { m_bThreadState = val; }
public:	
	LoadType GetLoadType() const { return m_emLoadType; }
	void SetLoadType(LoadType val) { m_emLoadType = val; }
public:	
	bool GetInLoading() const { return m_bInLoading; }
	void SetInLoading(bool val) { m_bInLoading = val; }
	bool GetInScaning() const { return m_bInScaning; }
	void SetInScaning(bool val) { m_bInScaning = val; }
public:	
	bool GetCurPicIsChange() const { return m_bCurPicIsChange; }
	void SetCurPicIsChange(bool val) { m_bCurPicIsChange = val; }
};


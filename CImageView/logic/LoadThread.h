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

	//����ָ���ļ��е������ļ������б�
	bool LoadAllFileNameFromDir();

	//�ж�һ���ļ���չ���Ƿ�֧��
	bool IsImage(CString pFileName);

	//�����б���Ĳ����ļ�������ͼ
	bool LoadImageFromList();

	//���б����ȡһ��ͼƬ
	bool GetImageFromList(CImage* &pImage, int& nIndex, SIZE* &stScreenSize, int* pAllCount);

	//��ȡ��ǰ����ͼƬ
	bool GetCurFullPic(Image* &pImage);

	//����һ��ͼƬ
	bool SaveImageToFile(UINT nIndex);

	//����һ��ͼƬ������ͼ
	bool LoadFullFile();

	//�����ļ�����ȡ�ļ�����
	int GetIndexFromFileName(wstring strFileName);

	//���ó�ʼ���ڴ�С
	void SetInitWindowSize(int nW, int nH);
private:
	//��ǰ�߳�״̬
	bool m_bThreadState;

	//CWinThread*			m_pThread;

	CThreadLockEx		m_cLock;

	//��ǰͼƬ������ͼ
	Image*				m_pCurFullPic;

	//��ǰͼƬ����
	int					m_nCurPicIndex;

	//���ҪԤ������һ�Ż�����һ��
	int					m_nLoadImagePos;

	//��ǰͼƬ���سɹ����
	bool				m_bCurPicLoadSuccess;

	//����Ƿ��л���ͼƬ
	bool				m_bCurPicIsChange;

	//��ǰ���ڴ�С
	SIZE				m_stCurWinSize;

	//��ǰ��Ļ��С
	SIZE				m_stCurScreenSize;

	//��ǰ��������
	LoadType			m_emLoadType;

	//��ǰ�Ƿ���ɨ������
	bool				m_bInScaning;

	//��ǰ�Ƿ��м�������
	bool				m_bInLoading;

	//��һ�μ��ر��
	bool				m_bFirst;

	//��ǰĿ¼
	wstring				m_strCurrentDirectory;
	//��ʼ�����ļ���
	wstring				m_strInitFileName;

	//�ļ��б�
	vector<wstring>		m_vtFiles;
	//����ͼ�б�
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


#include "stdafx.h"
#include "LoadThread.h"

UINT ThreadLoadImages(LPVOID lParam);

CLoadThread::CLoadThread(void)
{
	m_bThreadState = false;
	m_emLoadType = emLoadMax;
	m_bInScaning = false;
	m_bInLoading = false;
	m_pCurFullPic = NULL;
	m_nCurPicIndex = 0;
	m_bCurPicIsChange = false;
	m_bCurPicLoadSuccess = false;
	m_bFirst = true;
	m_nLoadImagePos = 0;
}


CLoadThread::~CLoadThread(void)
{
}

short CLoadThread::StartThread(SIZE stScreen)
{
	m_stCurScreenSize = stScreen;

	if (m_bThreadState)
		return -1;

	m_bThreadState = true;

	OutputDebugStringA("�����߳�����!\n");
	AfxBeginThread(ThreadLoadImages, this);

	return 1;
}

void CLoadThread::StopThread()
{
	m_cLock.Lock();

	m_bThreadState = false;

	m_cLock.UnLock();
}

bool CLoadThread::LoadFile(LoadType emType, void* pSource, LONG nW /*= DEFAULT_WINDOW_SIZE_X*/, LONG nH /*= DEFAULT_WINDOW_SIZE_Y*/)
{
	if (!pSource)
		return false;

	m_cLock.Lock();
	m_vtFiles.clear();

	switch (emType)
	{
	case emLoadList:
		{
			vector<wstring>* pSr = (vector<wstring>*)pSource;
			for (vector<wstring>::iterator itera = pSr->begin(); itera != pSr->end(); ++itera)
				m_vtFiles.push_back(itera->c_str());
		}
		break;
	case emLoadDir:
		{
			if (!SplitePath(((wstring*)pSource)->data(), m_strCurrentDirectory, m_strInitFileName))
			{
				AfxMessageBox(L"·������");
				return false;
			}

			//����ɨ������
			m_bInScaning = true;
		}
		break;
	default:
		break;
	}

	//�����������
	m_emLoadType = emType;

	//������������
	m_bInLoading = true;

	//��¼���ڴ�С�Լ�������ͼ
	//m_stCurWinSize.cx = nW;
	//m_stCurWinSize.cy = nH;

	m_cLock.UnLock();
	return true;
}

bool CLoadThread::LoadAllFileNameFromDir()
{
	if (m_strCurrentDirectory.empty())
		return false;

	m_vtFiles.clear();

	CFileFind finder;
	BOOL bWorking = finder.FindFile( CString(m_strCurrentDirectory.c_str()) + L"\\*.*");
	while( bWorking )
	{
		bWorking = finder.FindNextFile();
		if( finder.IsDots() || finder.IsDirectory() )
			continue;
		else
			if (IsImage(finder.GetFileName().MakeLower()))
				m_vtFiles.push_back( finder.GetFileName().GetBuffer() );
	}

	//����������ñ����������
	m_bInScaning = false;

	//��ʼ����ʱ��,���ҵ���ʼ����ͼƬ����
	m_nCurPicIndex = GetIndexFromFileName(m_strInitFileName);
	return true;
}

//�ж�һ���ļ���չ���Ƿ�֧��
bool CLoadThread::IsImage(CString pFileName)
{
	if (!pFileName)
		return false;

	CString str(pFileName);
	str.MakeLower();

	short wIndex = 0;
	while (g_szFileExtension[wIndex])
	{
		if (-1 != str.Find(g_szFileExtension[wIndex]))
			//�ҵ���
			return true;

		wIndex++;
	}

	return false;
}

//�����б���Ĳ����ļ�������ͼ
bool CLoadThread::LoadImageFromList()
{
	//����Ԥ����
	/*if (0 == m_nLoadImagePos && !m_bFirst)
	{
		m_bInLoading = false;
		return true;
	}*/

	OutputDebugString(L"----------���ػ����ļ�-----------\n");

	bool bChange = false;

	//ɾ������Ҫ���ļ�
	for (list<ImageInfo>::iterator itera = m_listThumbnai.begin(); itera != m_listThumbnai.end();)
	{
		if (itera->m_nIndex > (m_nCurPicIndex + DEFAULT_LOAD_NUM) || itera->m_nIndex < (m_nCurPicIndex - DEFAULT_LOAD_NUM))
		{
			ImageInfo image = *itera;

			wchar_t sz[MAX_PATH] = { 0 };
			_snwprintf_s(sz, sizeof(sz), L"ɾ����[%d]��ͼƬ[%s] \n", image.m_nIndex, m_vtFiles[image.m_nIndex].data());
			OutputDebugString(sz);

			itera = m_listThumbnai.erase(itera);
			delete image.m_pThumbnai;
			image.m_pThumbnai = NULL;

			bChange = true;
		}
		else
			itera++;
	}

	//�����µ��ļ�
	for (int i = (m_nCurPicIndex - DEFAULT_LOAD_NUM); i < (m_nCurPicIndex + DEFAULT_LOAD_NUM); ++i)
	{
		if (i < 0 || i >= (int)m_vtFiles.size())
			continue;

		//�Ѵ��ڵĲ�����
		bool bIsLoad = false;
		for (list<ImageInfo>::iterator itera = m_listThumbnai.begin(); itera != m_listThumbnai.end(); ++itera)
		{
			if (itera->m_nIndex == i)
			{
				bIsLoad = true;
				break;
			}
		}
		if (bIsLoad)
			continue;

		ImageInfo stImage;
		stImage.m_pThumbnai->Load((m_strCurrentDirectory + L"\\" + m_vtFiles[i]).data());
		if (stImage.m_pThumbnai->IsNull())
		{
			//����ʧ�ܵ�ֱ�Ӳ�����
			m_vtFiles.erase(m_vtFiles.begin() + i);
			continue;
		}

		//������Ļ�ߴ����ѹ��
		int nScaleX = m_stCurScreenSize.cx;
		int nScaleY = m_stCurScreenSize.cy;

		//�ж��Ƿ���ҪתΪ����ͼ, �ȵ�ǰ��ʾ����Ĳ�ѹ��
		int nThumbnailX = stImage.m_pThumbnai->GetWidth();
		int nThumbnailY = stImage.m_pThumbnai->GetHeight();
		if (nThumbnailX > nScaleX || nThumbnailY > nScaleY)
		{
			float shear = (float)nThumbnailX / (float)nThumbnailY;

			//���Ƚϴ����
			if ((float(nThumbnailX) / nScaleX) >= (float(nThumbnailY) / nScaleY))
			{
				//�������ű�
				//stImage.m_fProportion = (float)nScaleX / (float)nThumbnailX;

				nThumbnailX = nScaleX;
				nThumbnailY = int(nThumbnailX / shear);
			}
			else
			{
				//�������ű�
				//stImage.m_fProportion = (float)nScaleY / (float)nThumbnailY;

				nThumbnailY = nScaleY;
				nThumbnailX = int(nThumbnailY * shear);
			}

			DWORD dwA = GetTickCount();

			//GDI�ṩ��������ѹ������ʧ��,��ҪҪ����Щ�����ֻ��ĵ���Ƭ,���ز�������
			//stImage.m_pThumbnai = pImage->GetThumbnailImage(nThumbnailX, nThumbnailY,NULL,NULL)->Clone();

			//���Ծ���ȫͼ����DC����,Ȼ����������
			//CompressImagePixel(pImage, stImage.m_pThumbnai, nThumbnailY, nThumbnailX);

			//stImage.m_bIsThumbnai = true;

			/*if (m_bFirst)
			{
				m_stCurWinSize.cx = stImage.m_pThumbnai->GetWidth() > DEFAULT_WINDOW_SIZE_W ? stImage.m_pThumbnai->GetWidth() : DEFAULT_WINDOW_SIZE_W;
				m_stCurWinSize.cy = stImage.m_pThumbnai->GetHeight() > DEFAULT_WINDOW_SIZE_H ? stImage.m_pThumbnai->GetHeight() : DEFAULT_WINDOW_SIZE_H;
			}*/
		}
		/*else
		{
		if (m_bFirst)
		{
		m_stCurWinSize.cx = nThumbnailX > DEFAULT_WINDOW_SIZE_W ? nThumbnailX : DEFAULT_WINDOW_SIZE_W;
		m_stCurWinSize.cy = nThumbnailY > DEFAULT_WINDOW_SIZE_H ? nThumbnailY : DEFAULT_WINDOW_SIZE_H;
		}
		}*/

		if (m_bFirst && i == m_nCurPicIndex)
		{
			m_stCurWinSize.cx = nThumbnailX > DEFAULT_WINDOW_SIZE_W ? nThumbnailX : DEFAULT_WINDOW_SIZE_W;
			m_stCurWinSize.cy = nThumbnailY > DEFAULT_WINDOW_SIZE_H ? nThumbnailY : DEFAULT_WINDOW_SIZE_H;
			m_bFirst = false;
		}

		/*if (stImage.m_pThumbnai->GetLastStatus())
		continue;*/

		stImage.m_bInit = true;
		stImage.m_nIndex = i;

		if (m_nLoadImagePos < 0)
			m_listThumbnai.push_front(stImage);
		else
			m_listThumbnai.push_back(stImage);

		OutMsg(L"���ص�[%d]��ͼƬ[%s] \n", i, m_vtFiles[i].data());

		bChange = true;

		//����һ����Ƭ���˳�,�ȴ��´α�����ʱ�����
		break;
	}

	//int i = m_nCurPicIndex;
	//int nNum = 2;

	////һ���л�һ��ͼƬ,Ԥ����Ӧ��Ҳֻ�Ǳ仯һ��
	//if (m_nCurPicIndex >= 1 && m_nCurPicIndex < ((int)m_vtFiles.size() - 1))
	//{
	//	if (m_nLoadImagePos > 0)
	//	{
	//		nNum = ++i + 1;

	//		if (3 == m_listThumbnai.size())
	//		{
	//			ImageInfo image = m_listThumbnai.front();
	//			m_listThumbnai.pop_front();

	//			delete image.m_pThumbnai;
	//			image.m_pThumbnai = NULL;
	//		}
	//	}
	//	else
	//	{
	//		nNum = i--;

	//		if (3 == m_listThumbnai.size())
	//		{
	//			ImageInfo image = m_listThumbnai.back();
	//			m_listThumbnai.pop_back();

	//			delete image.m_pThumbnai;
	//			image.m_pThumbnai = NULL;
	//		}
	//	}
	//}

	//for (i; i < nNum; ++i)
	//{
	//	if (i < 0 || i >= (int)m_vtFiles.size())
	//		continue;

	//	ImageInfo stImage;
	//	stImage.m_pThumbnai->Load(m_vtFiles[i].data());
	//	if (stImage.m_pThumbnai->IsNull())
	//	{
	//		//����ʧ�ܵ�ֱ�Ӳ�����
	//		m_vtFiles.erase(m_vtFiles.begin() + i);
	//		continue;
	//	}

	//	//������Ļ�ߴ����ѹ��
	//	int nScaleX = m_stCurScreenSize.cx;
	//	int nScaleY = m_stCurScreenSize.cy;

	//	//�ж��Ƿ���ҪתΪ����ͼ, �ȵ�ǰ��ʾ����Ĳ�ѹ��
	//	int nThumbnailX = stImage.m_pThumbnai->GetWidth();
	//	int nThumbnailY = stImage.m_pThumbnai->GetHeight();
	//	if (nThumbnailX > nScaleX || nThumbnailY > nScaleY)
	//	{
	//		float shear = (float)nThumbnailX/(float)nThumbnailY;

	//		//���Ƚϴ����
	//		if ((float(nThumbnailX) / nScaleX) >= (float(nThumbnailY) / nScaleY))
	//		{
	//			//�������ű�
	//			//stImage.m_fProportion = (float)nScaleX / (float)nThumbnailX;

	//			nThumbnailX = nScaleX;
	//			nThumbnailY = int(nThumbnailX / shear);
	//		}
	//		else
	//		{
	//			//�������ű�
	//			//stImage.m_fProportion = (float)nScaleY / (float)nThumbnailY;

	//			nThumbnailY = nScaleY;
	//			nThumbnailX = int(nThumbnailY * shear);
	//		}

	//		DWORD dwA = GetTickCount();

	//		//GDI�ṩ��������ѹ������ʧ��,��ҪҪ����Щ�����ֻ��ĵ���Ƭ,���ز�������
	//		//stImage.m_pThumbnai = pImage->GetThumbnailImage(nThumbnailX, nThumbnailY,NULL,NULL)->Clone();

	//		//���Ծ���ȫͼ����DC����,Ȼ����������
	//		//CompressImagePixel(pImage, stImage.m_pThumbnai, nThumbnailY, nThumbnailX);

	//		//stImage.m_bIsThumbnai = true;

	//		if (m_bFirst)
	//		{
	//			m_stCurWinSize.cx = stImage.m_pThumbnai->GetWidth() > DEFAULT_WINDOW_SIZE_W ? stImage.m_pThumbnai->GetWidth() : DEFAULT_WINDOW_SIZE_W;
	//			m_stCurWinSize.cy = stImage.m_pThumbnai->GetHeight() > DEFAULT_WINDOW_SIZE_H ? stImage.m_pThumbnai->GetHeight() : DEFAULT_WINDOW_SIZE_H;
	//		}
	//	}
	//	/*else
	//	{
	//		if (m_bFirst)
	//		{
	//			m_stCurWinSize.cx = nThumbnailX > DEFAULT_WINDOW_SIZE_W ? nThumbnailX : DEFAULT_WINDOW_SIZE_W;
	//			m_stCurWinSize.cy = nThumbnailY > DEFAULT_WINDOW_SIZE_H ? nThumbnailY : DEFAULT_WINDOW_SIZE_H;
	//		}
	//	}*/

	//	if (m_bFirst)
	//	{
	//		m_stCurWinSize.cx = nThumbnailX > DEFAULT_WINDOW_SIZE_W ? nThumbnailX : DEFAULT_WINDOW_SIZE_W;
	//		m_stCurWinSize.cy = nThumbnailY > DEFAULT_WINDOW_SIZE_H ? nThumbnailY : DEFAULT_WINDOW_SIZE_H;
	//	}

	//	/*if (stImage.m_pThumbnai->GetLastStatus())
	//		continue;*/

	//	stImage.m_bInit = true;
	//	stImage.m_nIndex = i;

	//	if (m_nLoadImagePos < 0)
	//		m_listThumbnai.push_front(stImage);
	//	else
	//		m_listThumbnai.push_back(stImage);

	//	wchar_t sz[MAX_PATH] = {0};
	//	_snwprintf_s(sz, sizeof(sz), L"���ص�[%d]��ͼƬ[%s] \n", i, m_vtFiles[i]);
	//	OutputDebugString(sz);

	//	m_bFirst = false;

	//	/*delete pImage;
	//	pImage = NULL;*/
	//}

	//for (vector<CString>::iterator itera = m_vtFiles.begin(); itera != m_vtFiles.end(); )
	//{
	//	Image* pImage = Image::FromFile(m_strCurrentDirectory + L"\\" + *itera);
	//	if (pImage->GetLastStatus())
	//	{
	//		itera = m_vtFiles.erase(itera);
	//		continue;
	//	}

	//	ImageInfo stImage;
	//	stImage.m_pThumbnai = pImage->Clone();

	//	//������Ļ�ߴ����ѹ��
	//	int nScaleX = m_stCurScreenSize.cx;
	//	int nScaleY = m_stCurScreenSize.cy;

	//	//�ж��Ƿ���ҪתΪ����ͼ, �ȵ�ǰ��ʾ����Ĳ�ѹ��
	//	int nThumbnailX = pImage->GetWidth();
	//	int nThumbnailY = pImage->GetHeight();
	//	if (nThumbnailX > nScaleX || nThumbnailY > nScaleY)
	//	{
	//		float shear = (float)nThumbnailX/(float)nThumbnailY;

	//		//���Ƚϴ����
	//		if ((float(nThumbnailX) / nScaleX) >= (float(nThumbnailY) / nScaleY))
	//		{
	//			//�������ű�
	//			stImage.m_fProportion = (float)nScaleX / (float)nThumbnailX;

	//			nThumbnailX = nScaleX;
	//			nThumbnailY = int(nThumbnailX / shear);
	//		}
	//		else
	//		{
	//			//�������ű�
	//			stImage.m_fProportion = (float)nScaleY / (float)nThumbnailY;

	//			nThumbnailY = nScaleY;
	//			nThumbnailX = int(nThumbnailY * shear);
	//		}

	//		DWORD dwA = GetTickCount();

	//		//GDI�ṩ��������ѹ������ʧ��,��ҪҪ����Щ�����ֻ��ĵ���Ƭ,���ز�������
	//		//stImage.m_pThumbnai = pImage->GetThumbnailImage(nThumbnailX, nThumbnailY,NULL,NULL)->Clone();

	//		//���Ծ���ȫͼ����DC����,Ȼ����������
	//		CompressImagePixel(pImage, stImage.m_pThumbnai, nThumbnailY, nThumbnailX);

	//		stImage.m_bIsThumbnai = true;
	//		wchar_t sz[64] = {0};
	//		_snwprintf_s(sz, sizeof(sz), L"%s, time = %d \n", *itera, GetTickCount() - dwA);
	//		OutputDebugString(sz);

	//		if (m_bFirst)
	//		{
	//			m_stCurWinSize.cx = stImage.m_pThumbnai->GetWidth();
	//			m_stCurWinSize.cy = stImage.m_pThumbnai->GetHeight();
	//		}
	//	}
	//	else
	//	{
	//		if (m_bFirst && (nThumbnailX < DEFAULT_WINDOW_SIZE_W || nThumbnailY < DEFAULT_WINDOW_SIZE_H))
	//		{
	//			m_stCurWinSize.cx = DEFAULT_WINDOW_SIZE_W;
	//			m_stCurWinSize.cy = DEFAULT_WINDOW_SIZE_H;
	//		}
	//	}

	//	if (stImage.m_pThumbnai->GetLastStatus())
	//		continue;

	//	stImage.m_bInit = true;
	//	m_listThumbnai.push_back(stImage);

	//	//�����ļ�����,�Լ���ȫͼ
	//	wcscmp(stImage.m_szFileName, *itera);

	//	m_bFirst = false;

	//	delete pImage;
	//	pImage = NULL;

	//	++itera;
	//}

	//�����������
	if(!bChange)
		m_bInLoading = false;

	return true;
}

//���б����ȡһ��ͼƬ
bool CLoadThread::GetImageFromList(CImage* &pImage, int& nIndex, SIZE* &stScreenSize, int* pAllCount)
{
	bool m_bSuccess = true;
	m_cLock.Lock();

	//��������ǳ�ʼ��
	if (nIndex < 0)
		nIndex = m_nCurPicIndex;

	//һֱ���
	if ((UINT)nIndex >= m_vtFiles.size())
		nIndex = m_vtFiles.size() - 1;

	ImageInfo* pImageinfo = Find(nIndex);
	if (!pImageinfo)
		//�ļ���δ������,������������
		m_bSuccess = false;
	else
	{
		if (!pImageinfo || !pImageinfo->m_bInit || pImageinfo->m_pThumbnai->IsNull())
			m_bSuccess = false;
		else
		{
			pImage = pImageinfo->m_pThumbnai;
			//fProportion = pImageinfo->m_fProportion;
		}
	}

	if (stScreenSize)
	{
		stScreenSize->cx = m_stCurWinSize.cx;
		stScreenSize->cy = m_stCurWinSize.cy;
	}

	//�ж�Ҫ��ҪԤ����ԭͼ
	/*if (pImageinfo && pImageinfo->m_bIsThumbnai)
	{
		m_bCurPicIsChange = true;
	}*/

	if (m_bSuccess)
	{
		m_nCurPicIndex = nIndex;
		//������������,���ص�ǰͼƬ������
		m_bInLoading = true;

		if (pAllCount)
			*pAllCount = m_vtFiles.size();
	}

	//�������,����ǰ��ԭͼ����
	delete m_pCurFullPic;
	m_pCurFullPic = NULL;

	m_cLock.UnLock();

	return m_bSuccess;
}

ImageInfo* CLoadThread::Find(int nIndex)
{
	list<ImageInfo>::iterator itera = m_listThumbnai.begin();
	for (; itera != m_listThumbnai.end(); ++itera)
	{
		m_nLoadImagePos = itera->m_nIndex - m_nCurPicIndex;
		if (itera->m_nIndex == nIndex)
			return &*itera;
	}

	m_nLoadImagePos = 0;
	return NULL;
}

bool CLoadThread::CompressImagePixel(Image* pSource, Image* &pOut, UINT ulNewHeigth, UINT ulNewWidth)
{
	// ��ȡ����
	UINT unOriHeight = pSource->GetHeight();
	UINT unOriWidth = pSource->GetWidth();

	CLSID encoderClsid;
	if ( unOriWidth < 1 || unOriHeight < 1 ) 
		return false;
		
	// Get the CLSID of the JPEG encoder.
	if ( !GetEncoderClsid(L"image/jpeg", &encoderClsid) )
		return false;

	REAL fSrcX = 0.0f;
	REAL fSrcY = 0.0f;
	REAL fSrcWidth = (REAL) unOriWidth;
	REAL fSrcHeight = (REAL) unOriHeight ;
	RectF RectDest( 0.0f, 0.0f, (REAL)ulNewWidth, (REAL)ulNewHeigth);

	Bitmap* pTempBitmap = new Bitmap( ulNewWidth, ulNewHeigth );
	Graphics* graphics = NULL;

	if ( !pTempBitmap )
		return false;

	graphics = Graphics::FromImage( pTempBitmap );
	if ( !graphics )
		return false;

	if (graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality))
		return false;

	if (graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality))
		return false;

	if (graphics->DrawImage( pSource, RectDest, fSrcX, fSrcY, fSrcWidth, fSrcHeight,
		UnitPixel, NULL, NULL, NULL))
		return false;

	pOut = pTempBitmap->Clone(0, 0, ulNewWidth, ulNewHeigth, PixelFormat32bppARGB);

	/*if (pTempBitmap->Save( pszDestFilePah, &encoderClsid, NULL ))
	return false;*/

	if ( NULL != graphics ) {
			delete graphics;
			graphics = NULL;
		}

	if ( NULL != pTempBitmap ) {
		delete pTempBitmap;
		pTempBitmap = NULL;
	}

	return true;
}

bool CLoadThread::GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid)
{
		UINT  unNum = 0;          // number of image encoders
		UINT  unSize = 0;         // size of the image encoder array in bytes

		ImageCodecInfo* pImageCodecInfo = NULL;

		// How many encoders are there?
		// How big (in bytes) is the array of all ImageCodecInfo objects?
		GetImageEncodersSize( &unNum, &unSize );
		if ( 0 == unSize ) {
			return false;  // Failure
		}

		// Create a buffer large enough to hold the array of ImageCodecInfo
		// objects that will be returned by GetImageEncoders.
		pImageCodecInfo = (ImageCodecInfo*)( malloc(unSize) );
		if ( !pImageCodecInfo ) {
			return false;  // Failure
		}

		// GetImageEncoders creates an array of ImageCodecInfo objects
		// and copies that array into a previously allocated buffer. 
		// The third argument, imageCodecInfos, is a pointer to that buffer. 
		GetImageEncoders( unNum, unSize, pImageCodecInfo );

		for ( UINT j = 0; j < unNum; ++j ) {
			if ( wcscmp( pImageCodecInfo[j].MimeType, pszFormat ) == 0 ) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				pImageCodecInfo = NULL;
				return true;  // Success
			}    
		}

		free( pImageCodecInfo );
		pImageCodecInfo = NULL;
		return false;  // Failure
}

bool CLoadThread::SaveImageToFile(UINT nIndex)
{
	bool bSaveState = true;
	m_cLock.Lock();

	if (nIndex > m_vtFiles.size())
		bSaveState = false;

	Image* pImage = NULL;
	if (bSaveState)
	{
		pImage = Image::FromFile(m_vtFiles[nIndex].data());
		if (pImage->GetLastStatus())
			bSaveState = false;
	}

	if (bSaveState && pImage)
	{
		CFileDialog fileDlg(FALSE, L"png",L"��ͼ", OFN_OVERWRITEPROMPT, L"����ֲ����ͼ��(*.png)|*.png||");
		if(fileDlg.DoModal() == IDOK)
		{
			CLSID encoderClsid;
			if ( GetEncoderClsid(L"image/png", &encoderClsid) ) 
				bSaveState = pImage->Save(fileDlg.GetPathName().GetBuffer(), &encoderClsid, NULL) ? false : true;
			else
				bSaveState = false;
		}
	}

	m_cLock.UnLock();
	return bSaveState;
}

//����һ��ͼƬ������ͼ
bool CLoadThread::LoadFullFile()
{
	if ((UINT)m_nCurPicIndex > m_vtFiles.size())
		return false;

	m_bCurPicLoadSuccess = false;

	/*delete m_pCurFullPic;
	m_pCurFullPic = NULL;*/

	m_pCurFullPic = Image::FromFile(m_vtFiles[m_nCurPicIndex].data());
	if (m_pCurFullPic->GetLastStatus())
		return false;

	m_bCurPicLoadSuccess = true;

	//������Ҫ����
	m_bCurPicIsChange = false;

	return m_bCurPicLoadSuccess;
}

//�����ļ�����ȡ�ļ�����
int CLoadThread::GetIndexFromFileName(wstring strFileName)
{
	int nIndex = 0;
	for (vector<wstring>::iterator it = m_vtFiles.begin(); it != m_vtFiles.end(); ++it/*, ++nIndex*/)
	{
		if ((*it) == strFileName)
			break;
	}

	return nIndex;
}

//���ó�ʼ���ڴ�С
void CLoadThread::SetInitWindowSize(int nW, int nH)
{

}

//��ȡ��ǰ����ͼƬ
bool CLoadThread::GetCurFullPic(Image* &pImage)
{
	bool bState;

	m_cLock.Lock();

	if (m_bCurPicLoadSuccess)
		pImage = m_pCurFullPic;
	
	bState = m_bCurPicLoadSuccess;

	if (m_bCurPicLoadSuccess)
		OutputDebugStringA("��ȡԭͼ�ɹ�!\n");
	else
		OutputDebugStringA("��ȡԭͼʧ��!\n");

	m_cLock.UnLock();

	return bState;
}

//�����߳�
UINT ThreadLoadImages(LPVOID lParam)
{
	if (!lParam)
		return -1;

	CLoadThread* pLoadThread = (CLoadThread*)lParam;

	while (pLoadThread->GetThreadState())
	{
		if (pLoadThread->GetInScaning() \
			&& emLoadDir == pLoadThread->GetLoadType())
		{
			OutputDebugStringA("�����ļ��б�\n");

			//�����ļ��б�
			pLoadThread->LoadAllFileNameFromDir();
		}

		//�����ļ�
		if (pLoadThread->GetInLoading())
		{
			//OutputDebugStringA("�����ļ�����ͼ\n");

			pLoadThread->LoadImageFromList();
		}

		//����ԭͼ
		if (pLoadThread->GetCurPicIsChange())
		{
			OutputDebugStringA("����ԭͼ\n");

			pLoadThread->LoadFullFile();
		}

		//����Ҫ���ź���...û��ô�ϸ�
		Sleep(1);
	}

	OutputDebugStringA("�����߳������˳�!\n");
	return 1;
}
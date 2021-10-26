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

	OutputDebugStringA("加载线程启动!\n");
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
				AfxMessageBox(L"路径错误");
				return false;
			}

			//启动扫描任务
			m_bInScaning = true;
		}
		break;
	default:
		break;
	}

	//标记任务类型
	m_emLoadType = emType;

	//启动加载任务
	m_bInLoading = true;

	//记录窗口大小以加载缩略图
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

	//加载完成设置本次任务结束
	m_bInScaning = false;

	//初始化的时候,查找到初始化的图片索引
	m_nCurPicIndex = GetIndexFromFileName(m_strInitFileName);
	return true;
}

//判断一个文件拓展名是否支持
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
			//找到了
			return true;

		wIndex++;
	}

	return false;
}

//加载列表里的部分文件的缩略图
bool CLoadThread::LoadImageFromList()
{
	//不用预加载
	/*if (0 == m_nLoadImagePos && !m_bFirst)
	{
		m_bInLoading = false;
		return true;
	}*/

	OutputDebugString(L"----------加载缓存文件-----------\n");

	bool bChange = false;

	//删除不需要的文件
	for (list<ImageInfo>::iterator itera = m_listThumbnai.begin(); itera != m_listThumbnai.end();)
	{
		if (itera->m_nIndex > (m_nCurPicIndex + DEFAULT_LOAD_NUM) || itera->m_nIndex < (m_nCurPicIndex - DEFAULT_LOAD_NUM))
		{
			ImageInfo image = *itera;

			wchar_t sz[MAX_PATH] = { 0 };
			_snwprintf_s(sz, sizeof(sz), L"删除第[%d]张图片[%s] \n", image.m_nIndex, m_vtFiles[image.m_nIndex].data());
			OutputDebugString(sz);

			itera = m_listThumbnai.erase(itera);
			delete image.m_pThumbnai;
			image.m_pThumbnai = NULL;

			bChange = true;
		}
		else
			itera++;
	}

	//加载新的文件
	for (int i = (m_nCurPicIndex - DEFAULT_LOAD_NUM); i < (m_nCurPicIndex + DEFAULT_LOAD_NUM); ++i)
	{
		if (i < 0 || i >= (int)m_vtFiles.size())
			continue;

		//已存在的不加载
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
			//加载失败的直接不算了
			m_vtFiles.erase(m_vtFiles.begin() + i);
			continue;
		}

		//按照屏幕尺寸进行压缩
		int nScaleX = m_stCurScreenSize.cx;
		int nScaleY = m_stCurScreenSize.cy;

		//判断是否需要转为缩略图, 比当前显示器大的才压缩
		int nThumbnailX = stImage.m_pThumbnai->GetWidth();
		int nThumbnailY = stImage.m_pThumbnai->GetHeight();
		if (nThumbnailX > nScaleX || nThumbnailY > nScaleY)
		{
			float shear = (float)nThumbnailX / (float)nThumbnailY;

			//按比较大的来
			if ((float(nThumbnailX) / nScaleX) >= (float(nThumbnailY) / nScaleY))
			{
				//计算缩放比
				//stImage.m_fProportion = (float)nScaleX / (float)nThumbnailX;

				nThumbnailX = nScaleX;
				nThumbnailY = int(nThumbnailX / shear);
			}
			else
			{
				//计算缩放比
				//stImage.m_fProportion = (float)nScaleY / (float)nThumbnailY;

				nThumbnailY = nScaleY;
				nThumbnailX = int(nThumbnailY * shear);
			}

			DWORD dwA = GetTickCount();

			//GDI提供的这玩意压缩严重失真,主要要是那些比如手机拍的照片,像素不清晰的
			//stImage.m_pThumbnai = pImage->GetThumbnailImage(nThumbnailX, nThumbnailY,NULL,NULL)->Clone();

			//所以就先全图画到DC里面,然后再区出来
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

		OutMsg(L"加载第[%d]张图片[%s] \n", i, m_vtFiles[i].data());

		bChange = true;

		//加载一个照片就退出,等待下次遍历的时候加载
		break;
	}

	//int i = m_nCurPicIndex;
	//int nNum = 2;

	////一次切换一张图片,预加载应该也只是变化一张
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
	//		//加载失败的直接不算了
	//		m_vtFiles.erase(m_vtFiles.begin() + i);
	//		continue;
	//	}

	//	//按照屏幕尺寸进行压缩
	//	int nScaleX = m_stCurScreenSize.cx;
	//	int nScaleY = m_stCurScreenSize.cy;

	//	//判断是否需要转为缩略图, 比当前显示器大的才压缩
	//	int nThumbnailX = stImage.m_pThumbnai->GetWidth();
	//	int nThumbnailY = stImage.m_pThumbnai->GetHeight();
	//	if (nThumbnailX > nScaleX || nThumbnailY > nScaleY)
	//	{
	//		float shear = (float)nThumbnailX/(float)nThumbnailY;

	//		//按比较大的来
	//		if ((float(nThumbnailX) / nScaleX) >= (float(nThumbnailY) / nScaleY))
	//		{
	//			//计算缩放比
	//			//stImage.m_fProportion = (float)nScaleX / (float)nThumbnailX;

	//			nThumbnailX = nScaleX;
	//			nThumbnailY = int(nThumbnailX / shear);
	//		}
	//		else
	//		{
	//			//计算缩放比
	//			//stImage.m_fProportion = (float)nScaleY / (float)nThumbnailY;

	//			nThumbnailY = nScaleY;
	//			nThumbnailX = int(nThumbnailY * shear);
	//		}

	//		DWORD dwA = GetTickCount();

	//		//GDI提供的这玩意压缩严重失真,主要要是那些比如手机拍的照片,像素不清晰的
	//		//stImage.m_pThumbnai = pImage->GetThumbnailImage(nThumbnailX, nThumbnailY,NULL,NULL)->Clone();

	//		//所以就先全图画到DC里面,然后再区出来
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
	//	_snwprintf_s(sz, sizeof(sz), L"加载第[%d]张图片[%s] \n", i, m_vtFiles[i]);
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

	//	//按照屏幕尺寸进行压缩
	//	int nScaleX = m_stCurScreenSize.cx;
	//	int nScaleY = m_stCurScreenSize.cy;

	//	//判断是否需要转为缩略图, 比当前显示器大的才压缩
	//	int nThumbnailX = pImage->GetWidth();
	//	int nThumbnailY = pImage->GetHeight();
	//	if (nThumbnailX > nScaleX || nThumbnailY > nScaleY)
	//	{
	//		float shear = (float)nThumbnailX/(float)nThumbnailY;

	//		//按比较大的来
	//		if ((float(nThumbnailX) / nScaleX) >= (float(nThumbnailY) / nScaleY))
	//		{
	//			//计算缩放比
	//			stImage.m_fProportion = (float)nScaleX / (float)nThumbnailX;

	//			nThumbnailX = nScaleX;
	//			nThumbnailY = int(nThumbnailX / shear);
	//		}
	//		else
	//		{
	//			//计算缩放比
	//			stImage.m_fProportion = (float)nScaleY / (float)nThumbnailY;

	//			nThumbnailY = nScaleY;
	//			nThumbnailX = int(nThumbnailY * shear);
	//		}

	//		DWORD dwA = GetTickCount();

	//		//GDI提供的这玩意压缩严重失真,主要要是那些比如手机拍的照片,像素不清晰的
	//		//stImage.m_pThumbnai = pImage->GetThumbnailImage(nThumbnailX, nThumbnailY,NULL,NULL)->Clone();

	//		//所以就先全图画到DC里面,然后再区出来
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

	//	//保存文件名字,以加载全图
	//	wcscmp(stImage.m_szFileName, *itera);

	//	m_bFirst = false;

	//	delete pImage;
	//	pImage = NULL;

	//	++itera;
	//}

	//加载任务完成
	if(!bChange)
		m_bInLoading = false;

	return true;
}

//在列表里获取一个图片
bool CLoadThread::GetImageFromList(CImage* &pImage, int& nIndex, SIZE* &stScreenSize, int* pAllCount)
{
	bool m_bSuccess = true;
	m_cLock.Lock();

	//这种情况是初始化
	if (nIndex < 0)
		nIndex = m_nCurPicIndex;

	//一直向后翻
	if ((UINT)nIndex >= m_vtFiles.size())
		nIndex = m_vtFiles.size() - 1;

	ImageInfo* pImageinfo = Find(nIndex);
	if (!pImageinfo)
		//文件还未被载入,返回正在载入
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

	//判断要不要预加载原图
	/*if (pImageinfo && pImageinfo->m_bIsThumbnai)
	{
		m_bCurPicIsChange = true;
	}*/

	if (m_bSuccess)
	{
		m_nCurPicIndex = nIndex;
		//启动加载任务,加载当前图片的左右
		m_bInLoading = true;

		if (pAllCount)
			*pAllCount = m_vtFiles.size();
	}

	//无论如何,清理当前的原图加载
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
	// 获取长宽
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
		CFileDialog fileDlg(FALSE, L"png",L"截图", OFN_OVERWRITEPROMPT, L"可移植网络图形(*.png)|*.png||");
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

//加载一幅图片的完整图
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

	//不再需要加载
	m_bCurPicIsChange = false;

	return m_bCurPicLoadSuccess;
}

//根据文件名获取文件索引
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

//设置初始窗口大小
void CLoadThread::SetInitWindowSize(int nW, int nH)
{

}

//获取当前完整图片
bool CLoadThread::GetCurFullPic(Image* &pImage)
{
	bool bState;

	m_cLock.Lock();

	if (m_bCurPicLoadSuccess)
		pImage = m_pCurFullPic;
	
	bState = m_bCurPicLoadSuccess;

	if (m_bCurPicLoadSuccess)
		OutputDebugStringA("获取原图成功!\n");
	else
		OutputDebugStringA("获取原图失败!\n");

	m_cLock.UnLock();

	return bState;
}

//加载线程
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
			OutputDebugStringA("加载文件列表\n");

			//加载文件列表
			pLoadThread->LoadAllFileNameFromDir();
		}

		//加载文件
		if (pLoadThread->GetInLoading())
		{
			//OutputDebugStringA("加载文件缩略图\n");

			pLoadThread->LoadImageFromList();
		}

		//加载原图
		if (pLoadThread->GetCurPicIsChange())
		{
			OutputDebugStringA("加载原图\n");

			pLoadThread->LoadFullFile();
		}

		//不需要用信号量...没那么严格
		Sleep(1);
	}

	OutputDebugStringA("加载线程正常退出!\n");
	return 1;
}
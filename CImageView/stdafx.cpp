// stdafx.cpp : 只包括标准包含文件的源文件
// CImageView.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


void ImageHorizontalFlip(ATL::CImage* &dest, ATL::CImage* &src)
{
	dest->Create(src->GetWidth(), src->GetHeight(), src->GetBPP(), ATL::CImage::createAlphaChannel);

	CPoint points[3];
	points[0] = CPoint(src->GetWidth(), 0);
	points[1] = CPoint(0, 0);
	points[2] = CPoint(src->GetWidth(), src->GetHeight());
	src->PlgBlt(dest->GetDC(), points);
}

void ImageRotate(ATL::CImage* &dest, ATL::CImage* &src)
{
	int imageWidth=src->GetWidth();
	int imageHeight=src->GetHeight();

	if (dest->IsNull())
	{
		int nBPP = src->GetBPP();
		if (32 == nBPP)
			dest->Create(imageHeight, imageWidth, nBPP, ATL::CImage::createAlphaChannel);
		else
			dest->Create(imageHeight, imageWidth, nBPP /*32, ATL::CImage::createAlphaChannel*/);
	}

	POINT pt[3];
	pt[0].x=imageHeight;pt[0].y=0;
	pt[1].x=imageHeight;pt[1].y=imageWidth;
	pt[2].x=0;pt[2].y=0;

	src->PlgBlt(dest->GetDC(), pt, 0,0,imageWidth,imageHeight);
}

void ImageCopy(CImage* const &srcImage, CImage* &destImage, int x, int y, int w, int h)
{
	if (!srcImage || srcImage->IsNull())
		return ;

	if (w == 0 || h == 0)
	{
		w = srcImage->GetWidth() - x;
		h = srcImage->GetHeight() - y;
	}

	int MaxColors = srcImage->GetMaxColorTableEntries();
	RGBQUAD* ColorTab;
	ColorTab = new RGBQUAD[MaxColors];

	CDC *pDCsrc,*pDCdrc;
	if (!destImage->IsNull())
	{
		destImage->Destroy();
	}

	int nBPP = srcImage->GetBPP();
	if (32 == nBPP)
		destImage->Create(w, h, nBPP, ATL::CImage::createAlphaChannel);
	else
		destImage->Create(w, h, nBPP /*32, ATL::CImage::createAlphaChannel*/);


	if (srcImage->IsIndexed())
	{
		srcImage->GetColorTable(0,MaxColors,ColorTab);
		destImage->SetColorTable(0,MaxColors,ColorTab);
	}

	pDCsrc=CDC::FromHandle(srcImage->GetDC());
	pDCdrc=CDC::FromHandle(destImage->GetDC());
	pDCdrc->BitBlt(0,0,w,h,pDCsrc,x,y,SRCCOPY);
	srcImage->ReleaseDC();
	destImage->ReleaseDC();
	delete ColorTab;

	//创建新图像
	//int srcBitsCount=srcImage->GetBPP();
	//if(srcBitsCount==32)   //支持alpha通道
	//{
	//	destImage->Create(w,h,srcBitsCount,1);
	//}
	//else
	//{
	//	destImage->Create(w,h,srcBitsCount,0);
	//}
	////加载调色板
	//if(srcBitsCount<=8&&srcImage->IsIndexed())//需要调色板
	//{
	//	RGBQUAD pal[256];
	//	int nColors=srcImage->GetMaxColorTableEntries();
	//	if(nColors>0)
	//	{     
	//		srcImage->GetColorTable(0,nColors,pal);
	//		destImage->SetColorTable(0,nColors,pal);//复制调色板程序
	//	}   
	//} 
	////目标图像参数
	//BYTE *destPtr=(BYTE*)destImage->GetBits();
	//int destPitch=destImage->GetPitch();
	//BYTE* srcPtr=(BYTE*)srcImage->GetBits() + x;
	//int srcPitch=srcImage->GetPitch() - x + w; 
	////复制图像数据
	//for(int n = 0; y<(y + h);y++, n++)
	//{
	//	memcpy( destPtr+n*destPitch, srcPtr+y*srcPitch, abs(srcPitch) );
	//} 
}

bool SplitePath(IN const wchar_t* pSource, OUT wstring& strPath, OUT wstring& strFile)
{
	if (!pSource)
		return false;

	//查找斜杠,反斜杠
	const wchar_t* pPos = NULL;
	for (size_t i = 0; i < wcslen(pSource); ++i)
	{
		if (pSource[i] == '/' || pSource[i] == '\\')
			pPos = pSource + i;
	}

	//整个字符串都没有斜杠,反斜杠
	if (!pPos)
		return false;

	//判断路径是文件还是目录,目录直接返回
	if (GetFileAttributes(pSource) && FILE_ATTRIBUTE_DIRECTORY)
	{
		strPath = pSource;
		return true;
	}

	//取出字符串
	strPath = wstring(pSource, pPos);
	//这个文件名有可能没有后缀
	strFile = wstring(++pPos);
	return true;
}

//输出信息
void OutMsg(wchar_t* pStr, ...)
{
	va_list args;
	va_start(args, pStr);
	wchar_t szMsg[1024] = { 0 };
	_vsntprintf(szMsg, sizeof(szMsg), pStr, args);
	va_end(args);

	wcscat(szMsg, L"\n");
	OutputDebugString(szMsg);
}

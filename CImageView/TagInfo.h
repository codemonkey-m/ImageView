#pragma once

#include "resource.h"

//默认窗口大小
#define DEFAULT_WINDOW_SIZE_W	600
#define DEFAULT_WINDOW_SIZE_H	400

//默认单击时长
#define DEFAULT_LBUTTON_CLICK	100

//默认放大缩小极限
#define DEFAULT_MAX_ZOOM		16.0f
#define DEFAULT_MAX_NARROW		0.05f

//转轴滚动一次变化的值
#define DEFAULT_PROPORTION		0.2f

//默认放大后的缩略图大小
#define DEFAULT_THUMBNAUL_W		250
#define DEFAULT_THUMBNAUL_H		160

//默认加载缓存文件数量,加载后面DEFAULT_LOAD_NUM和前面DEFAULT_LOAD_NUM个
#define DEFAULT_LOAD_NUM		3

//比例显示时长
#define DEFAULT_PRORORTIO_TMIE	500
//比例显示字体大小
#define DEFAULT_PRO_FONT_SIZE	15

//翻页处数量显示的字体大小
#define DEFAULT_COUNT_FONT_SIZE	10

//缩略图上的关闭按钮相对原图缩小比例
#define DEFAULT_THUM_CLOSE_PRO	1

//工具们,这里有顺序要求,在检测鼠标所在当前控件时,靠上的会先检测,覆盖后面的
typedef enum emToolButtonID
{
	emRBtnMenu,			//右键菜单
	emRBtnMenuTips,		//右键菜单的高亮
	emBtnTurnPage,		//翻页按钮
	emToolButtons,		//工具栏按钮们
	emToolButtonTips,	//工具栏按钮们的高亮
	emCloseButton,		//关闭按钮
	emTipsImage,		//加载中提示图片
	emProportionBG,		//比例背景
	emCloseBG,			//关闭按钮背景
	emThumClose,		//缩略图关闭按钮
	emThumCloseBG,		//缩略图关闭按钮背景
	emThumRect,			//缩略图上当前显示块
	emThumbnail,		//底部缩略图
	emBgImage,			//背景

	emMax,
}ToolButtunID;

static const UINT g_nOnlyRes[] = 
{
	IDR_PNG_RBTNMUNE,
	IDR_PNG_RBTNMUNE_TIPS,
	IDR_PNG_TURNPG,
	IDR_PNG_TOOLBTN,
	IDR_PNG_TOOLTIP,
	IDR_PNG_CLOSEBTN,
	IDR_PNG_LODING,
	IDR_PNG_PROBG,		//比例背景
	IDR_PNG_CLOSTBG,	//关闭背景
	IDR_PNG_THUMBBG,	//缩略图上当前显示块
	IDR_PNG_BGIMG,
};

//资源,和上面对应
static const UINT g_nFileNames[] = 
{
	IDR_PNG_RBTNMUNE,
	IDR_PNG_RBTNMUNE_TIPS,
	IDR_PNG_TURNPG,
	IDR_PNG_TOOLBTN,
	IDR_PNG_TOOLTIP,
	IDR_PNG_CLOSEBTN,
	IDR_PNG_LODING,
	IDR_PNG_PROBG,		//比例背景
	IDR_PNG_CLOSTBG,	//关闭背景
	IDR_PNG_CLOSEBTN,	//缩略图关闭按钮
	IDR_PNG_CLOSTBG,	//缩略图关闭按钮背景
	IDR_PNG_THUMBBG,	//缩略图上当前显示块
	IDR_PNG_BGIMG,
	IDR_PNG_BGIMG,
};

//工具栏按钮们对应的位置和大小
static const RECT g_stTooltBtnPoints[] = 
{
	{20, 10, 44, 30},
	{60, 10, 81, 30},
	{100, 10, 120, 30},
	{131, 10, 157, 30},
	{169, 10, 193, 30},
};

//最下面工具栏的按钮们...
typedef enum emBottomBtns
{
	emReduction,		//1:1
	emFullScreen,		//全屏
	emRotation,			//旋转
	emSave,				//保存

	emBottomMax,
}BottomBtnID;

//右键菜单们对应的位置和大小
static const RECT g_stLBtnMenuPoints[] = 
{
	{0, 1, 141, 23},
	{0, 23, 141, 44},
};

//右键菜单们
typedef enum emRMenuBtns
{
	emRBtnCopy,			//复制
	emRBtnSave,			//另存为

	emRBtnMax,
}RMenuBtnID;

//太复杂了....先弃用....
typedef enum emButtonState
{
	emDontShow,			//不显示
	emNormal,			//正常
	emHeigh,			//高亮
	emDisable,			//禁用
}ButtonState;

typedef struct tagToolButton
{
	ToolButtunID		m_emID;		//按钮ID
	CImage*				m_pImage;	//按钮图片
	POINT				m_curPoint;	//所在位置
	bool				m_bDisplayState;//当前状态

	short				m_wShowW;	//显示的尺寸
	short				m_wShowH;	//显示的尺寸

	//bool				m_bIsDestroy;//已经释放了

	tagToolButton()
	{
		m_emID = emMax;
		m_curPoint.x = 0;
		m_curPoint.y = 0;
		m_wShowW = 0;
		m_wShowH = 0;
		m_bDisplayState = false;

		//m_pImage = new CImage;
	}

	~tagToolButton()
	{
		//if (!m_pImage->IsNull()/* && !m_bIsDestroy*/)
		//	m_pImage->Destroy();

		//delete m_pImage;
		//m_pImage = NULL;
	}

	UINT w() { return m_wShowW ? m_wShowW : (m_pImage ? m_pImage->GetWidth() : 0);}
	UINT h() { return m_wShowH ? m_wShowH : (m_pImage ? m_pImage->GetHeight() : 0);}
}TooleButton;

//加载的图片结构
typedef struct tagImageInfo
{
	int				m_nIndex;			//当前索引
	CImage*			m_pThumbnai;		//缩略图
	bool			m_bInit;			//加载完成

	tagImageInfo()
	{ 
		Release();

		m_pThumbnai = new CImage;
	}
	~tagImageInfo()
	{ 
		Release();

		if (m_pThumbnai)
		{
			if (!m_pThumbnai->IsNull())
				m_pThumbnai->Destroy();

			delete m_pThumbnai;
			m_pThumbnai = NULL;
		}
	}

	void Release()
	{
		m_nIndex = 0;
		m_bInit = false;
	}

	//拷贝构造
	tagImageInfo(const tagImageInfo& imageInfo)
	{
		m_nIndex = imageInfo.m_nIndex ;
		m_bInit = imageInfo.m_bInit;

		if (!imageInfo.m_pThumbnai->IsNull())
		{
			m_pThumbnai = new CImage;
			ImageCopy(imageInfo.m_pThumbnai, m_pThumbnai);
		}
	}
}ImageInfo;

typedef enum emLoadType
{
	emLoadList,
	emLoadDir,

	emLoadMax,
}LoadType;

//支持的图片拓展名
static const WCHAR* g_szFileExtension[] = 
{
	L".png",
	L".jpg",
	L".bmp",
	L".gif",
	NULL,
};

typedef enum emTimerType
{
	emTimerOutOfWimCheck,		//定时检测移出
	emTimerProportionBG,		//比例背景定时消失
}TimerType;
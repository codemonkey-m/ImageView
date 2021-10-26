#pragma once

#include "resource.h"

//Ĭ�ϴ��ڴ�С
#define DEFAULT_WINDOW_SIZE_W	600
#define DEFAULT_WINDOW_SIZE_H	400

//Ĭ�ϵ���ʱ��
#define DEFAULT_LBUTTON_CLICK	100

//Ĭ�ϷŴ���С����
#define DEFAULT_MAX_ZOOM		16.0f
#define DEFAULT_MAX_NARROW		0.05f

//ת�����һ�α仯��ֵ
#define DEFAULT_PROPORTION		0.2f

//Ĭ�ϷŴ�������ͼ��С
#define DEFAULT_THUMBNAUL_W		250
#define DEFAULT_THUMBNAUL_H		160

//Ĭ�ϼ��ػ����ļ�����,���غ���DEFAULT_LOAD_NUM��ǰ��DEFAULT_LOAD_NUM��
#define DEFAULT_LOAD_NUM		3

//������ʾʱ��
#define DEFAULT_PRORORTIO_TMIE	500
//������ʾ�����С
#define DEFAULT_PRO_FONT_SIZE	15

//��ҳ��������ʾ�������С
#define DEFAULT_COUNT_FONT_SIZE	10

//����ͼ�ϵĹرհ�ť���ԭͼ��С����
#define DEFAULT_THUM_CLOSE_PRO	1

//������,������˳��Ҫ��,�ڼ��������ڵ�ǰ�ؼ�ʱ,���ϵĻ��ȼ��,���Ǻ����
typedef enum emToolButtonID
{
	emRBtnMenu,			//�Ҽ��˵�
	emRBtnMenuTips,		//�Ҽ��˵��ĸ���
	emBtnTurnPage,		//��ҳ��ť
	emToolButtons,		//��������ť��
	emToolButtonTips,	//��������ť�ǵĸ���
	emCloseButton,		//�رհ�ť
	emTipsImage,		//��������ʾͼƬ
	emProportionBG,		//��������
	emCloseBG,			//�رհ�ť����
	emThumClose,		//����ͼ�رհ�ť
	emThumCloseBG,		//����ͼ�رհ�ť����
	emThumRect,			//����ͼ�ϵ�ǰ��ʾ��
	emThumbnail,		//�ײ�����ͼ
	emBgImage,			//����

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
	IDR_PNG_PROBG,		//��������
	IDR_PNG_CLOSTBG,	//�رձ���
	IDR_PNG_THUMBBG,	//����ͼ�ϵ�ǰ��ʾ��
	IDR_PNG_BGIMG,
};

//��Դ,�������Ӧ
static const UINT g_nFileNames[] = 
{
	IDR_PNG_RBTNMUNE,
	IDR_PNG_RBTNMUNE_TIPS,
	IDR_PNG_TURNPG,
	IDR_PNG_TOOLBTN,
	IDR_PNG_TOOLTIP,
	IDR_PNG_CLOSEBTN,
	IDR_PNG_LODING,
	IDR_PNG_PROBG,		//��������
	IDR_PNG_CLOSTBG,	//�رձ���
	IDR_PNG_CLOSEBTN,	//����ͼ�رհ�ť
	IDR_PNG_CLOSTBG,	//����ͼ�رհ�ť����
	IDR_PNG_THUMBBG,	//����ͼ�ϵ�ǰ��ʾ��
	IDR_PNG_BGIMG,
	IDR_PNG_BGIMG,
};

//��������ť�Ƕ�Ӧ��λ�úʹ�С
static const RECT g_stTooltBtnPoints[] = 
{
	{20, 10, 44, 30},
	{60, 10, 81, 30},
	{100, 10, 120, 30},
	{131, 10, 157, 30},
	{169, 10, 193, 30},
};

//�����湤�����İ�ť��...
typedef enum emBottomBtns
{
	emReduction,		//1:1
	emFullScreen,		//ȫ��
	emRotation,			//��ת
	emSave,				//����

	emBottomMax,
}BottomBtnID;

//�Ҽ��˵��Ƕ�Ӧ��λ�úʹ�С
static const RECT g_stLBtnMenuPoints[] = 
{
	{0, 1, 141, 23},
	{0, 23, 141, 44},
};

//�Ҽ��˵���
typedef enum emRMenuBtns
{
	emRBtnCopy,			//����
	emRBtnSave,			//���Ϊ

	emRBtnMax,
}RMenuBtnID;

//̫������....������....
typedef enum emButtonState
{
	emDontShow,			//����ʾ
	emNormal,			//����
	emHeigh,			//����
	emDisable,			//����
}ButtonState;

typedef struct tagToolButton
{
	ToolButtunID		m_emID;		//��ťID
	CImage*				m_pImage;	//��ťͼƬ
	POINT				m_curPoint;	//����λ��
	bool				m_bDisplayState;//��ǰ״̬

	short				m_wShowW;	//��ʾ�ĳߴ�
	short				m_wShowH;	//��ʾ�ĳߴ�

	//bool				m_bIsDestroy;//�Ѿ��ͷ���

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

//���ص�ͼƬ�ṹ
typedef struct tagImageInfo
{
	int				m_nIndex;			//��ǰ����
	CImage*			m_pThumbnai;		//����ͼ
	bool			m_bInit;			//�������

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

	//��������
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

//֧�ֵ�ͼƬ��չ��
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
	emTimerOutOfWimCheck,		//��ʱ����Ƴ�
	emTimerProportionBG,		//����������ʱ��ʧ
}TimerType;
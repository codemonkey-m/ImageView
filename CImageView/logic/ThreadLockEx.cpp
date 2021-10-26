#include "StdAfx.h"
#include "ThreadLockEx.h"

CThreadLockEx::CThreadLockEx(void)
{
	InitializeCriticalSection(&m_cs);
}

CThreadLockEx::~CThreadLockEx(void)
{
	DeleteCriticalSection(&m_cs);
}

void CThreadLockEx::Lock()
{
	EnterCriticalSection(&m_cs);
}

void CThreadLockEx::UnLock()
{
	LeaveCriticalSection(&m_cs);
}


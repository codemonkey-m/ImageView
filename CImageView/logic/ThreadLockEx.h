#pragma once

class CThreadLockEx
{
public:
	CThreadLockEx(void);
	~CThreadLockEx(void);

public:
	void Lock();
	void UnLock();

private:
	CRITICAL_SECTION m_cs;
};

/******************************************************************************
�ļ�:	mutex.cpp
����:	�μ� mutex.h
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2014-08-28
�޸�:
*******************************************************************************/

#include "stdafx.h"
#include "mutex.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(mutex)


CThreadMutex::CThreadMutex(void) 
{
	::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
}
CThreadMutex::~CThreadMutex(void) {
	::DeleteCriticalSection(&m_cs);
}


CScopedLock::CScopedLock(CThreadMutex& _mutex)
	: m_mutex(_mutex)
{
	_mutex.Lock();
}


CScopedLock::~CScopedLock(void)
{
	m_mutex.Unlock();
}


W2X_DEFINE_NAME_SPACE_END(mutex)
W2X_NAME_SPACE_END

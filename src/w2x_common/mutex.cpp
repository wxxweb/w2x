/******************************************************************************
文件:	mutex.cpp
描述:	参见 mutex.h
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2014-08-28
修改:
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

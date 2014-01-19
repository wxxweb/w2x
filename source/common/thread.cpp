/*****************************************************************************
文件名：		thread.cpp
描述：		参见 thread.h
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2013.12.10
******************************************************************************/

#include "stdafx.h"
#include "thread.h"

W2X_NAME_SPACE_BEGIN

class CCriticalSection::CImpl
{
public:
	CImpl(void);
	~CImpl(void);
private:
	CImpl(const CImpl&);
	void operator =(const CImpl&);
public:
	LONG m_enter_count;
	CRITICAL_SECTION m_cs;
};

CCriticalSection::CImpl::CImpl(void)
	: m_enter_count(0)
{
	memset(&m_cs, 0, sizeof(CRITICAL_SECTION));
	::InitializeCriticalSection(&m_cs);
}

CCriticalSection::CImpl::~CImpl(void) 
{
	::DeleteCriticalSection(&m_cs);
}

CCriticalSection::CCriticalSection(void) : m_impl(new CImpl())
{
	
}

CCriticalSection::~CCriticalSection(void) 
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl));
}

void CCriticalSection::enter(void) 
{
	::EnterCriticalSection(&(m_impl->m_cs));
}

void CCriticalSection::leave(void) 
{
	::LeaveCriticalSection(&(m_impl->m_cs));
}

W2X_NAME_SPACE_END

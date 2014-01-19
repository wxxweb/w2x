/*****************************************************************************
�ļ�����		thread.cpp
������		�μ� thread.h
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2013.12.10
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

void CCriticalSection::Enter(void) 
{
	::EnterCriticalSection(&(m_impl->m_cs));
}

void CCriticalSection::Leave(void) 
{
	::LeaveCriticalSection(&(m_impl->m_cs));
}

W2X_NAME_SPACE_END

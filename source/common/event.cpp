/******************************************************************************
 * 文件：	event.cpp
 * 描述：	参见 event.h
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-24
*******************************************************************************/

#include "stdafx.h"
#include "event.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEvent::CEventImpl
{
public:
	CEventImpl(LPCTSTR _type_name);
	~CEventImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CEventImpl)

public:
	LPTSTR m_type_name_ptr;
	CRefPtr<IBase> m_source_ptr;
	CRefPtr<IBase> m_target_ptr;
};


CEvent::CEventImpl::CEventImpl(LPCTSTR _type_name)
	: m_type_name_ptr(NULL)
{
	IF_NULL_ASSERT_RETURN(_type_name);

	const size_t count = _tcslen(_type_name) + 1;
	m_type_name_ptr = new TCHAR[count];
	IF_NULL_ASSERT_RETURN(m_type_name_ptr);

	_tcscpy_s(m_type_name_ptr, count, _type_name);
}

CEvent::CEventImpl::~CEventImpl(void) 
{
	SAFE_DELETE_ARRAY(m_type_name_ptr);
}


CEvent::CEvent(LPCTSTR _type_name)
	: m_event_impl_ptr(new CEventImpl(_type_name))
{
	ASSERT(NULL != m_event_impl_ptr);
}


CEvent::~CEvent(void)
{
	SAFE_DELETE(const_cast<CEventImpl*>(m_event_impl_ptr));
}


LPCTSTR CEvent::GetTypeName(void) const
{
	return m_event_impl_ptr->m_type_name_ptr;
}


CRefPtr<IBase>& CEvent::GetSource(void) const
{
	return m_event_impl_ptr->m_source_ptr;
}

void CEvent::SetSource(const CRefPtr<IBase>& _source_ptr)
{
	m_event_impl_ptr->m_source_ptr = _source_ptr;
}


CRefPtr<IBase>& CEvent::GetTarget(void) const
{
	return m_event_impl_ptr->m_target_ptr;
}


void CEvent::SetTarget(const CRefPtr<IBase>& _target_ptr)
{
	m_event_impl_ptr->m_target_ptr = _target_ptr;
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


/******************************************************************************
 * 文件：	event.cpp
 * 描述：	参见 event.h
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-24
*******************************************************************************/

#include "stdafx.h"
#include "event.h"
#include "mutex.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


const TCHAR CEvent::EVENT_UNKNOW[] = TEXT("EventUnknow");
const TCHAR CEvent::TARGET_UNNAMED[] = TEXT("TargetUnnamed");


class CEvent::CEventImpl
{
public:
	CEventImpl(LPCTSTR _type_name);
	~CEventImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CEventImpl)

public:
	inline void Reset(void);
	inline void SetTypeName(LPCTSTR _name);
	inline void SetTargetName(LPCTSTR _name);

public:
	LPTSTR m_type_name_ptr;
	LPTSTR m_target_name_ptr;
};


CEvent::CEventImpl::CEventImpl(LPCTSTR _type_name)
	: m_type_name_ptr(NULL)
	, m_target_name_ptr(NULL)
{
	this->SetTypeName(_type_name);
}


CEvent::CEventImpl::~CEventImpl(void) 
{
	SAFE_DELETE_ARRAY(m_type_name_ptr);
	SAFE_DELETE_ARRAY(m_target_name_ptr);
}


inline void CEvent::CEventImpl::Reset(void)
{
	SAFE_DELETE_ARRAY(m_type_name_ptr);
	SAFE_DELETE_ARRAY(m_target_name_ptr);
}

inline void CEvent::CEventImpl::SetTypeName(LPCTSTR _name)
{
	if (NULL == _name) {
		_name = CEvent::EVENT_UNKNOW;
	}

	SAFE_DELETE_ARRAY(m_type_name_ptr);
	const size_t count = _tcslen(_name) + 1;
	m_type_name_ptr = new TCHAR[count];
	IF_NULL_ASSERT_RETURN(m_type_name_ptr);

	_tcscpy_s(m_type_name_ptr, count, _name);
}


inline void CEvent::CEventImpl::SetTargetName(LPCTSTR _name)
{
	if (NULL == _name) {
		_name = TEXT("");
	}

	SAFE_DELETE_ARRAY(m_target_name_ptr);
	const size_t count = _tcslen(_name) + 1;
	m_target_name_ptr = new TCHAR[count];
	IF_NULL_ASSERT_RETURN(m_target_name_ptr);

	_tcscpy_s(m_target_name_ptr, count, _name);
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


void CEvent::Reset(void)
{
	m_event_impl_ptr->Reset();
}


LPCTSTR CEvent::GetTypeName(void) const
{
	return m_event_impl_ptr->m_type_name_ptr;
}


void CEvent::SetTypeName(LPCTSTR _type_name)
{
	return m_event_impl_ptr->SetTypeName(_type_name);
}


bool CEvent::IsTypeOf(CEvent& _event) const
{
	return this->IsTypeOf(_event.GetTypeName());
}


bool CEvent::IsTypeOf(LPCTSTR _type_name) const
{
	IF_NULL_RETURN_VALUE(_type_name, false);
	if (0 == _tcsicmp(m_event_impl_ptr->m_type_name_ptr, _type_name)) {
		return true;
	}
	return false;
}


LPCTSTR CEvent::GetTargetName(void) const
{
	return m_event_impl_ptr->m_target_name_ptr;
}


void CEvent::SetTargetName(LPCTSTR _name)
{
	return m_event_impl_ptr->SetTargetName(_name);
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


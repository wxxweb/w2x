/******************************************************************************
 * 文件：	event_dispathcer.cpp
 * 描述：	参见 event_dispathcer.h
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-24
*******************************************************************************/

#include "stdafx.h"
#include "event_dispatcher.h"
#include "event.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEventDispatcher::CDispImpl
{
	typedef std::multimap<TSTDSTR, FEventListener> EventListeners;

public:
	CDispImpl(const CRefPtr<IBase>& _target_ptr);
	~CDispImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CDispImpl)
W2X_IMPLEMENT_LOCKING(CDispImpl, CAutoLock)

public:
	inline bool AddEventListener(LPCTSTR _type_name, FEventListener _listener);

	inline bool DispatchEvent(const CEvent& _event_ref);

	inline bool HasEventListener(LPCTSTR _type_name) const;

	inline bool RemoveEventListener(LPCTSTR _type_name, FEventListener _listener);

private:
	CRefPtr<IBase> m_target_ptr;
	EventListeners m_listeners;
};


W2X_IMPLEMENT_LOCKING_CLASS(CEventDispatcher::CDispImpl, CAutoLock)


CEventDispatcher::CDispImpl::CDispImpl(const CRefPtr<IBase>& _target_ptr)
	: m_target_ptr(_target_ptr)
{

}


CEventDispatcher::CDispImpl::~CDispImpl(void)
{

}


inline bool CEventDispatcher::CDispImpl::AddEventListener(
	LPCTSTR _type_name, 
	FEventListener _listener
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_type_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener, false);

	CAutoLock this_lock(this);

	m_listeners.insert(std::pair<TSTDSTR, FEventListener>(_type_name, _listener));

	return true;
}


inline bool CEventDispatcher::CDispImpl::DispatchEvent(
	const CEvent& _event_ref
	)
{
	LPCTSTR type_name_ptr = _event_ref.GetTypeName();
	IF_NULL_ASSERT_RETURN_VALUE(type_name_ptr, false);

	CAutoLock this_lock(this);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(type_name_ptr);
		 range.first != range.second; ++(range.first))
	{
		FEventListener listener_fn_ptr = range.first->second;
		IF_NULL_ASSERT (listener_fn_ptr)
		{
			continue;
		}
		listener_fn_ptr(_event_ref);
	}

	return true;
}


inline bool CEventDispatcher::CDispImpl::HasEventListener(
	LPCTSTR _type_name
	) const
{
	CAutoLock this_lock(const_cast<CDispImpl*>(this));

	return m_listeners.end() != m_listeners.find(_type_name);
}


inline bool CEventDispatcher::CDispImpl::RemoveEventListener(
	LPCTSTR _type_name,
	FEventListener _listener
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_type_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener, false);

	CAutoLock this_lock(this);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(_type_name);
		 range.first != range.second; ++(range.first))
	{
		if (_listener == range.first->second)
		{
			m_listeners.erase(range.first);
			return true;
		}
	}

	return false;
}


CEventDispatcher::CEventDispatcher(const CRefPtr<IBase>& _target_ptr)
	: m_disp_impl_ptr(new CDispImpl(_target_ptr))
{

}


CEventDispatcher::~CEventDispatcher(void)
{
	SAFE_DELETE(const_cast<CDispImpl*>(m_disp_impl_ptr));
}


bool CEventDispatcher::AddEventListener(
	LPCTSTR _type_name, 
	FEventListener _listener
	)
{
	return m_disp_impl_ptr->AddEventListener(_type_name, _listener);
}

bool CEventDispatcher::DispatchEvent(const CEvent& _event_ref) const
{
	return m_disp_impl_ptr->DispatchEvent(_event_ref);
}


bool CEventDispatcher::HasEventListener(LPCTSTR _type_name) const
{
	return m_disp_impl_ptr->HasEventListener(_type_name);
}


bool CEventDispatcher::RemoveEventListener(
	LPCTSTR _type_name, 
	FEventListener _listener
	)
{
	return m_disp_impl_ptr->RemoveEventListener(_type_name, _listener);
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END
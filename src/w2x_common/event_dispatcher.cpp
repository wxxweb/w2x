/******************************************************************************
 * 文件:		event_dispathcer.cpp
 * 描述:		参见 event_dispathcer.h
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 创建:		2014-02-24
 * 修改:		2014-04-19
 ******************************************************************************/

#include "stdafx.h"
#include "event_dispatcher.h"
#include "event.h"
#include "mutex.h"
#include "log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEventDispatcher::CDispImpl
{
	struct ListenerInfo {
		EventListener _listener;
		PVOID _param;
	};

	typedef std::multimap<TSTDSTR, ListenerInfo> EventListeners;

public:
	CDispImpl(LPCTSTR _target_name);
	~CDispImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CDispImpl)
W2X_IMPLEMENT_LOCKING(CDispImpl, CAutoLock)

public:
	inline bool AddEventListener(
		LPCTSTR _type_name, 
		EventListener _listener_fn,
		PVOID _param
	);

	bool DispatchEvent(CEvent& _event_ref);

	inline bool HasEventListener(LPCTSTR _type_name) const;

	bool RemoveEventListener(LPCTSTR _type_name, EventListener _listener_fn);

private:
	LPTSTR			m_target_name_ptr;
	EventListeners	m_listeners;
};


W2X_IMPLEMENT_LOCKING_CLASS(CEventDispatcher::CDispImpl, CAutoLock)


CEventDispatcher::CDispImpl::CDispImpl(LPCTSTR _target_name)
	: m_target_name_ptr(NULL)
{
	if (NULL == _target_name) {
		_target_name = TEXT("");
	}

	const size_t count = _tcslen(_target_name) + 1;
	m_target_name_ptr = new TCHAR[count];
	IF_NULL_ASSERT_RETURN(m_target_name_ptr);

	_tcscpy_s(m_target_name_ptr, count, _target_name);
}


CEventDispatcher::CDispImpl::~CDispImpl(void)
{
	CAutoLock this_lock(this);
	SAFE_DELETE(m_target_name_ptr);
}


inline bool CEventDispatcher::CDispImpl::AddEventListener(
	LPCTSTR _type_name, 
	EventListener _listener_fn,
	PVOID _param
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_type_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener_fn, false);

	ListenerInfo info = {0};
	info._listener = _listener_fn;
	info._param = _param;

	CAutoLock this_lock(this);

	m_listeners.insert(
		std::pair<TSTDSTR, ListenerInfo>(_type_name, info));

	return true;
}


bool CEventDispatcher::CDispImpl::DispatchEvent(CEvent& _event_ref)
{
	LPCTSTR type_name_ptr = _event_ref.GetTypeName();
	IF_NULL_ASSERT_RETURN_VALUE(type_name_ptr, false);

	CAutoLock this_lock(this);

	EventListener listener_fn = NULL;
	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(type_name_ptr);
		 range.first != range.second; ++(range.first))
	{
		listener_fn = range.first->second._listener;
		IF_NULL_ASSERT (listener_fn)
		{
			continue;
		}

		if (NULL != m_target_name_ptr && TEXT('\0') != m_target_name_ptr[0]) {
			_event_ref.SetTargetName(m_target_name_ptr);
		}
		listener_fn(_event_ref, range.first->second._param);
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


bool CEventDispatcher::CDispImpl::RemoveEventListener(
	LPCTSTR _type_name,
	EventListener _listener_fn
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_type_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener_fn, false);

	CAutoLock this_lock(this);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(_type_name);
		 range.first != range.second; ++(range.first))
	{
		if (_listener_fn == range.first->second._listener)
		{
			m_listeners.erase(range.first);
			return true;
		}
	}

	return false;
}


CEventDispatcher::CEventDispatcher(LPCTSTR _target_name)
	: m_disp_impl_ptr(new CDispImpl(_target_name))
{

}


CEventDispatcher::~CEventDispatcher(void)
{
	SAFE_DELETE(const_cast<CDispImpl*>(m_disp_impl_ptr));
}


bool CEventDispatcher::AddEventListener(
	LPCTSTR _type_name, 
	EventListener _listener_fn,
	PVOID _param
	)
{
	return m_disp_impl_ptr->AddEventListener(_type_name, _listener_fn, _param);
}

bool CEventDispatcher::DispatchEvent(CEvent& _event_ref) const
{
	return m_disp_impl_ptr->DispatchEvent(_event_ref);
}


bool CEventDispatcher::HasEventListener(LPCTSTR _type_name) const
{
	return m_disp_impl_ptr->HasEventListener(_type_name);
}


bool CEventDispatcher::RemoveEventListener(
	LPCTSTR _type_name, 
	EventListener _listener_fn
	)
{
	return m_disp_impl_ptr->RemoveEventListener(_type_name, _listener_fn);
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END
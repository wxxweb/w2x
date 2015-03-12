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
#include "string_util.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEventDispatcher::CDispImpl
{
	typedef std::multimap<TSTDSTR, EventListenerPtr> EventListeners;

public:
	CDispImpl(LPCTSTR _target_name);
	~CDispImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CDispImpl)

public:
	inline bool AddEventListener(
		LPCTSTR _event_type,
		EventListenerPtr& _listener
	);

	bool DispatchEvent(CEvent& _event);

	inline bool HasEventListener(LPCTSTR _event_type);

	bool RemoveEventListener(
		LPCTSTR _event_type, 
		const EventListenerPtr& _listener
		);

	bool RemoveEventListener(
		LPCTSTR _event_type, 
		LPCTSTR _listener_id
		);

private:
	LPTSTR m_target_name_ptr;
	EventListeners m_listeners;
	mutex::CThreadMutex m_thread_mutex;
};


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
	SAFE_DELETE(m_target_name_ptr);
}


inline bool CEventDispatcher::CDispImpl::AddEventListener(
	LPCTSTR _event_type,
	EventListenerPtr& _listener
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_event_type, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener.get(), false);

	mutex::CScopedLock lock(m_thread_mutex);

	m_listeners.insert(
		std::pair<TSTDSTR, EventListenerPtr>(_event_type, _listener));
	_listener->SetRegistered(true);

	return true;
}


bool CEventDispatcher::CDispImpl::DispatchEvent(CEvent& _event)
{
	LPCTSTR type_name_ptr = _event.GetTypeName();
	IF_NULL_ASSERT_RETURN_VALUE(type_name_ptr, false);

	mutex::CScopedLock lock(m_thread_mutex);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(type_name_ptr);
		 range.first != range.second; ++(range.first))
	{
		EventListenerPtr& listener = range.first->second;
		IF_NULL_ASSERT (listener.get()) {
			continue;
		}
		if (false == listener->IsEnabled()) {
			continue;
		}
		
		if (NULL != m_target_name_ptr && TEXT('\0') != m_target_name_ptr[0])
		{
			_event.SetTargetName(m_target_name_ptr);
		}
		listener->SetProtected(true);
		listener->Execute(_event);
		listener->SetProtected(false);

		if (false == listener->IsRegistered()) {
			EventListeners::iterator itTemp = range.first;
			++(range.first);
			m_listeners.erase(itTemp);
			if (range.first == range.second) {
				return true;
			}
		}
	}

	return true;
}


inline bool CEventDispatcher::CDispImpl::HasEventListener(
	LPCTSTR _event_type
	)
{
	mutex::CScopedLock lock(m_thread_mutex);

	return m_listeners.end() != m_listeners.find(_event_type);
}


bool CEventDispatcher::CDispImpl::RemoveEventListener(
	LPCTSTR _event_type,
	const EventListenerPtr& _listener
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_event_type, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener.get(), false);

	mutex::CScopedLock lock(m_thread_mutex);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(_event_type);
		 range.first != range.second; ++(range.first))
	{
		if (range.first->second == _listener)
		{
			_listener->SetRegistered(false);
			if (false == _listener->IsProtected())
			{
				m_listeners.erase(range.first);
			}
			return true;
		}
	}

	return false;
}


bool CEventDispatcher::CDispImpl::RemoveEventListener(
	LPCTSTR _event_type,
	LPCTSTR _listener_id
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_event_type, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener_id, false);

	mutex::CScopedLock lock(m_thread_mutex);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		= m_listeners.equal_range(_event_type);
		range.first != range.second; ++(range.first))
	{
		EventListenerPtr& listener = range.first->second;
		LPCTSTR id = listener->GetListenerId();
		if (0 == _tcscmp(id, _listener_id))
		{
			listener->SetRegistered(false);
			if (false == listener->IsProtected())
			{
				m_listeners.erase(range.first);
			}
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
	LPCTSTR _event_type,
	EventListenerPtr& _listener
	)
{
	return m_disp_impl_ptr->AddEventListener(_event_type, _listener);
}

bool CEventDispatcher::DispatchEvent(CEvent& _event) const
{
	return m_disp_impl_ptr->DispatchEvent(_event);
}


bool CEventDispatcher::HasEventListener(LPCTSTR _event_type) const
{
	return m_disp_impl_ptr->HasEventListener(_event_type);
}


bool CEventDispatcher::RemoveEventListener(
	LPCTSTR _type_name, 
	LPCTSTR _listener_id
	)
{
	return m_disp_impl_ptr->RemoveEventListener(_type_name, _listener_id);
}


bool CEventDispatcher::RemoveEventListener(
	LPCTSTR _type_name, 
	const EventListenerPtr& _listener
	)
{
	return m_disp_impl_ptr->RemoveEventListener(_type_name, _listener);
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END
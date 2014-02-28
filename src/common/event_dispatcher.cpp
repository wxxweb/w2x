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
#include "log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEventDispatcher::CDispImpl
{
	enum {
		GC_INTERVAL_TIME = 60000,	// 两次垃圾回收的时间间隔
	};

	struct ListenerInfo {
		FEventListener listener_fn;
		CRefPtr<IBase> source_ptr;
		ListenerInfo(
			FEventListener _listener_fn, 
			const CRefPtr<IBase>& _source_ptr_ref
		) : listener_fn(_listener_fn), source_ptr(_source_ptr_ref)
		{}
	};

	typedef std::multimap<TSTDSTR, ListenerInfo> EventListeners;

public:
	CDispImpl(const CRefPtr<IBase>& _target_ptr);
	~CDispImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CDispImpl)
W2X_IMPLEMENT_LOCKING(CDispImpl, CAutoLock)

public:
	inline bool AddEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener_fn,
		const CRefPtr<IBase>& _source_ptr
	);

	bool DispatchEvent(CEvent& _event_ref);

	inline bool HasEventListener(LPCTSTR _type_name) const;

	bool RemoveEventListener(LPCTSTR _type_name, FEventListener _listener_fn);

	void DoGarbageCollection(void);

	void CheckGarbageCollection(void);

private:
	static DWORD CALLBACK GarbageCollectionThread(PVOID _param_ptr);

private:
	CRefPtr<IBase>	m_target_ptr;
	EventListeners	m_listeners;
	HANDLE			m_gc_thread_handle;
	HANDLE			m_gc_event_handle;
	DWORD			m_prev_gc_time;
};


W2X_IMPLEMENT_LOCKING_CLASS(CEventDispatcher::CDispImpl, CAutoLock)


CEventDispatcher::CDispImpl::CDispImpl(const CRefPtr<IBase>& _target_ptr)
	: m_target_ptr(_target_ptr)
	, m_gc_thread_handle(NULL)
	, m_gc_event_handle(NULL)
	, m_prev_gc_time(0)
{
	m_gc_thread_handle = ::CreateThread(
		NULL, 0, CDispImpl::GarbageCollectionThread, 
		reinterpret_cast<PVOID>(this), 0, NULL);

	IF_NULL_ASSERT (m_gc_thread_handle)
	{
		const DWORD code = ::GetLastError();
		w2x::log::LogError(
			TEXT("Create event dispatcher GC thread faild(%d)."), code);
	}
}


CEventDispatcher::CDispImpl::~CDispImpl(void)
{
	if (NULL != m_gc_event_handle)
	{
		::SetEvent(m_gc_event_handle);
		::CloseHandle(m_gc_event_handle);
		m_gc_event_handle = NULL;
	}

	if (NULL != m_gc_thread_handle)
	{
		IF_FALSE_ASSERT (
			WAIT_TIMEOUT != ::WaitForSingleObject(m_gc_thread_handle, 1000))
		{
			::TerminateThread(m_gc_thread_handle, 0);
		}
		::CloseHandle(m_gc_thread_handle);
		m_gc_thread_handle = NULL;
	}
}


inline bool CEventDispatcher::CDispImpl::AddEventListener(
	LPCTSTR _type_name, 
	FEventListener _listener_fn,
	const CRefPtr<IBase>& _source_ptr
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_type_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener_fn, false);

	this->CheckGarbageCollection();

	CAutoLock this_lock(this);

	m_listeners.insert(std::pair<TSTDSTR, ListenerInfo>(
		_type_name, ListenerInfo(_listener_fn, _source_ptr)));

	return true;
}


bool CEventDispatcher::CDispImpl::DispatchEvent(
	CEvent& _event_ref
	)
{
	LPCTSTR type_name_ptr = _event_ref.GetTypeName();
	IF_NULL_ASSERT_RETURN_VALUE(type_name_ptr, false);

	this->CheckGarbageCollection();

	CAutoLock this_lock(this);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(type_name_ptr);
		 range.first != range.second; ++(range.first))
	{
		ListenerInfo& info_ref = range.first->second;
		IF_NULL_ASSERT (info_ref.listener_fn)
		{
			continue;
		}

		_event_ref.SetSource(info_ref.source_ptr);
		info_ref.listener_fn(_event_ref);
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
	FEventListener _listener_fn
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_type_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_listener_fn, false);

	CAutoLock this_lock(this);

	for (std::pair<EventListeners::iterator, EventListeners::iterator> range 
		 = m_listeners.equal_range(_type_name);
		 range.first != range.second; ++(range.first))
	{
		ListenerInfo& info_ref = range.first->second;
		if (_listener_fn == info_ref.listener_fn)
		{
			m_listeners.erase(range.first);
			return true;
		}
	}

	return false;
}


void CEventDispatcher::CDispImpl::CheckGarbageCollection(void)
{
	if (NULL == m_gc_event_handle)
	{
		return;
	}

	if (GC_INTERVAL_TIME > ::GetTickCount() - m_prev_gc_time)
	{
		return;
	}
	
	CAutoLock this_lock(this);

	m_prev_gc_time = ::GetTickCount();

	if (0 == m_listeners.size())
	{
		return;
	}

	::SetEvent(m_gc_event_handle);
}


void CEventDispatcher::CDispImpl::DoGarbageCollection(void)
{
	w2x::log::LogInfo(TEXT("Begin Event Dispatcher GC..."));

	CAutoLock this_lock(this);

	const size_t orgn_size = m_listeners.size();

	for (EventListeners::iterator it = m_listeners.begin();
		 m_listeners.end() != it; NULL)
	{
		ListenerInfo& info_ref = it->second;
		if (NULL == info_ref.source_ptr.get())
		{
			++it;
			continue;
		}

		if (1 < info_ref.source_ptr->GetRefCount())
		{
			++it;
			continue;
		}

		m_listeners.erase(it++);
	}

	if (NULL != m_gc_event_handle)
	{
		::ResetEvent(m_gc_event_handle);
	}

	w2x::log::LogInfo(TEXT("End Event Dispatcher GC(%d)."),
		orgn_size - m_listeners.size());
}


DWORD CALLBACK CEventDispatcher::CDispImpl::GarbageCollectionThread(
	PVOID _param_ptr
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_param_ptr, -1);

	CDispImpl* const this_ptr = reinterpret_cast<CDispImpl*>(_param_ptr);

	this_ptr->m_gc_event_handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	IF_NULL_ASSERT (this_ptr->m_gc_event_handle)
	{
		const DWORD code = ::GetLastError();
		w2x::log::LogError(
			TEXT("Create event dispatcher GC event faild(%d)."), code);
		return -1;
	}

	while (WAIT_ABANDONED != 
		::WaitForSingleObject(this_ptr->m_gc_event_handle, INFINITE))
	{
		this_ptr->DoGarbageCollection();
	}

	return 0;
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
	FEventListener _listener_fn,
	const CRefPtr<IBase>& _source_ptr
	)
{
	return m_disp_impl_ptr->AddEventListener(_type_name, _listener_fn, _source_ptr);
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
	FEventListener _listener_fn
	)
{
	return m_disp_impl_ptr->RemoveEventListener(_type_name, _listener_fn);
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END
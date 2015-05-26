/******************************************************************************
文件:	timer.cpp
描述:	参见 timer.h
作者:	wxxweb
邮箱:	wxxweb@gmail.com
创建:	2015-04-13
修改:
*******************************************************************************/

#include "stdafx.h"
#include "timer.h"


W2X_NS_BEGIN
W2X_DEFINE_NS_BEGIN(utils)


#define TIMER_DISP_NAME	TEXT("TimerEventDisp")


class CTimer::CImpl
{
	enum {
		MIN_INTERVAL = 50,	// 最小更新时间为 50 毫秒
	};
public:
	CImpl(DWORD _delay, DWORD _repeat_count);
	~CImpl(void);

public:
	inline void SetDelay(DWORD _value);
	inline void SetRepeatCount(DWORD _value);
	static UINT CALLBACK TimerThread(PVOID _param);

public:
	DWORD m_current_count;
	DWORD m_delay;
	DWORD m_repeat_count;
	bool m_running;
	events::EventDispatcherPtr m_evt_disp;
};


CTimer::CImpl::CImpl(DWORD _delay, DWORD _repeat_count)
	: m_delay(_delay)
	, m_repeat_count(0 == _repeat_count ? 1 : _repeat_count)
	, m_current_count(0)
	, m_running(false)
	, m_evt_disp(new w2x::events::CEventDispatcher(TIMER_DISP_NAME))
{
}


CTimer::CImpl::~CImpl(void)
{}


inline void CTimer::CImpl::SetDelay(DWORD _value)
{
	m_delay = _value;
	if (MIN_INTERVAL > m_delay) {
		m_delay = MIN_INTERVAL;
	}
}


inline void CTimer::CImpl::SetRepeatCount(DWORD _value)
{
	m_repeat_count = _value;
	if (0 == m_repeat_count) {
		m_repeat_count = INFINITE;
	}
}


UINT CALLBACK CTimer::CImpl::TimerThread(PVOID _param)
{
	TimerPtr this_ptr = reinterpret_cast<CTimer*>(_param);
	IF_NULL_ASSERT_RETURN_VALUE(this_ptr, EXIT_FAILURE);

	CImpl* const impl = this_ptr->m_impl;
	IF_NULL_ASSERT_RETURN_VALUE(impl, EXIT_FAILURE);

	events::TimerEventPtr timer_event
		= new events::CTimerEvent(events::CTimerEvent::EVENT_TIMER, this_ptr.get());

	DWORD pass_time = 0;
	DWORD rest_time = 0;
	DWORD pre_time = pre_time = ::GetTickCount();

	for (impl->m_current_count = 0; impl->m_current_count < impl->m_repeat_count;)
	{
		if (false == impl->m_running) {
			break;
		}

		for (pass_time = 0; pass_time < impl->m_delay;
			pass_time = ::GetTickCount() - pre_time)
		{
			rest_time = impl->m_delay - pass_time;
			if (rest_time > impl->m_delay) {
				break; // timeout
			}
			if (rest_time >= MIN_INTERVAL) {
				::Sleep(MIN_INTERVAL);
			} else {
				::Sleep(rest_time);
			}
			if (false == impl->m_running) {
				break;
			}
		};
		pre_time = ::GetTickCount();
		
		if (false == impl->m_running) {
			break;
		}

		impl->m_current_count++;
		impl->m_evt_disp->DispatchEvent(*timer_event.get());
	}

	if (false == impl->m_running) {
		timer_event->SetTypeName(events::CTimerEvent::EVENT_TIMER_STOP);
		impl->m_evt_disp->DispatchEvent(*timer_event.get());
	}

	if (impl->m_current_count == impl->m_repeat_count) {
		timer_event->SetTypeName(events::CTimerEvent::EVENT_TIMER_COMPLETE);
		impl->m_evt_disp->DispatchEvent(*timer_event.get());
	}

	impl->m_running = false;
	return EXIT_SUCCESS;
}


CTimer::CTimer(DWORD _delay, DWORD _repeat_count)
	: m_impl(new CImpl(_delay, _repeat_count))
{
}


CTimer::~CTimer(void)
{}


TimerPtr CTimer::Create(DWORD _delay, DWORD _repeat_count)
{
	if (0 == _delay) {
		return NULL;
	}
	return new CTimer(_delay, _repeat_count);
}


void CTimer::Reset(void)
{
	m_impl->m_running = false;
	m_impl->m_current_count = 0;
}


bool CTimer::Start(void)
{
	if (true == m_impl->m_running) {
		return true;
	}
	m_impl->m_running = true;

	HANDLE thread_handle = reinterpret_cast<HANDLE>(
		_beginthreadex(NULL, 0, CImpl::TimerThread, this, 0, NULL));

	if (NULL == thread_handle) {
		ASSERT(false);
		m_impl->m_running = false;
		return false;
	}

	::CloseHandle(thread_handle);
	thread_handle = NULL;

	return true;
}


void CTimer::Stop(void)
{
	m_impl->m_running = false;
}


bool CTimer::IsRunning(void) const
{
	return m_impl->m_running;
}


DWORD CTimer::GetCurrentCount(void) const
{
	return m_impl->m_current_count;
}


DWORD CTimer::GetDelay(void) const
{
	return m_impl->m_delay;
}


void CTimer::SetDelay(DWORD _value)
{
	m_impl->SetDelay(_value);
}


DWORD CTimer::GetRepeatCount(void) const
{
	if (INFINITE == m_impl->m_repeat_count) {
		return 0;
	}
	return m_impl->m_repeat_count;
}
	

void CTimer::SetRepeatCount(DWORD _value)
{
	m_impl->SetRepeatCount(_value);
}

bool CTimer::AddEventListener(
	LPCTSTR _event_type,
	w2x::events::EventListenerPtr& _listener
	)
{
	return m_impl->m_evt_disp->AddEventListener(_event_type, _listener);
}

bool CTimer::DispatchEvent(w2x::events::CEvent& _event) const
{
	return m_impl->m_evt_disp->DispatchEvent(_event);
}

bool CTimer::HasEventListener(LPCTSTR _event_type) const
{
	return m_impl->m_evt_disp->HasEventListener(_event_type);
}

bool CTimer::RemoveEventListener(
	LPCTSTR _event_type,
	const w2x::events::EventListenerPtr& _listener
	)
{
	return m_impl->m_evt_disp->RemoveEventListener(_event_type, _listener);
}

bool CTimer::RemoveEventListener(
	LPCTSTR _event_type,
	LPCTSTR _listener_id
	)
{
	return m_impl->m_evt_disp->RemoveEventListener(_event_type, _listener_id);
}


W2X_DEFINE_NS_END(utils)
W2X_NS_END
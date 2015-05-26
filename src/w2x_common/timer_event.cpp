/******************************************************************************
�ļ�:	TimerEvent.cpp
����:	�μ� TimerEvent.h
����:	wxxweb
����:	wxxweb@gmail.com
����:	2015-04-13
�޸�:
*******************************************************************************/

#include "stdafx.h"
#include "timer_event.h"


W2X_NS_BEGIN
W2X_DEFINE_NS_BEGIN(events)


TCHAR CTimerEvent::EVENT_TIMER[] = TEXT("EventTimer");
TCHAR CTimerEvent::EVENT_TIMER_COMPLETE[] = TEXT("EventTimerComplete");
TCHAR CTimerEvent::EVENT_TIMER_STOP[] = TEXT("EventTimerStop");


CTimerEvent::CTimerEvent(LPCTSTR _type, utils::CTimer* _timer)
	: CEvent(_type)
	, m_timer(_timer)
{
	if (false == this->IsTypeOf(EVENT_TIMER) &&
		false == this->IsTypeOf(EVENT_TIMER_COMPLETE)) 
	{
		ASSERT(false);
		this->SetTypeName(EVENT_TIMER);
	}
	ASSERT(_timer);
}


CTimerEvent::~CTimerEvent(void)
{}


utils::CTimer* CTimerEvent::GetTimer(void) const
{
	return m_timer;
}


W2X_DEFINE_NS_END(events)
W2X_NS_END
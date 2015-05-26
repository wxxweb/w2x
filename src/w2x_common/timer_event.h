/******************************************************************************
�ļ�:	timer_event.h
����:	ֻҪ CTimer ����ﵽ Timer.delay ����ָ���ļ�������� CTimerEvent ����
����:	wxxweb
����:	wxxweb@gmail.com
����:	2015-04-13
�޸�:
*******************************************************************************/

#ifndef __W2X_COMMON_TIMER_EVENT_H__
#define __W2X_COMMON_TIMER_EVENT_H__
#pragma once


#include "event.h"


W2X_NS_BEGIN

W2X_DEFINE_NS_BEGIN(utils)
class CTimer;
W2X_DEFINE_NS_END(utils)

W2X_DEFINE_NS_BEGIN(events)


class W2X_COMMON_API CTimerEvent: public w2x::events::CEvent
{
public:
	/** ÿ�� CTimer ����ﵽ���� CTimer.delay ����ָ���ļ��ʱ���ȡ�*/
	static TCHAR EVENT_TIMER[];

	/** ÿ������� Timer.repeatCount ���õ�����������ȡ�*/
	static TCHAR EVENT_TIMER_COMPLETE[];

	/** ����ʱ�������֮ǰ��ֹͣʱ���ȡ�*/
	static TCHAR EVENT_TIMER_STOP[];

public:
	/** _type ȡֵ��EVENT_TIMER �� EVENT_TIMER_COMPLETE */
	CTimerEvent(LPCTSTR _type, utils::CTimer* _timer);
	virtual ~CTimerEvent(void);

public:
	utils::CTimer* GetTimer(void) const;

private:
	utils::CTimer* m_timer;
};


typedef w2x::CRefPtr<CTimerEvent> TimerEventPtr;


W2X_DEFINE_NS_END(events)
W2X_NS_END


#endif // __W2X_COMMON_TIMER_EVENT_H__
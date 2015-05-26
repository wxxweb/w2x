/******************************************************************************
文件:	timer_event.h
描述:	只要 CTimer 对象达到 Timer.delay 属性指定的间隔，调度 CTimerEvent 对象
作者:	wxxweb
邮箱:	wxxweb@gmail.com
创建:	2015-04-13
修改:
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
	/** 每当 CTimer 对象达到根据 CTimer.delay 属性指定的间隔时调度。*/
	static TCHAR EVENT_TIMER[];

	/** 每当它完成 Timer.repeatCount 设置的请求数后调度。*/
	static TCHAR EVENT_TIMER_COMPLETE[];

	/** 当定时器在完成之前被停止时调度。*/
	static TCHAR EVENT_TIMER_STOP[];

public:
	/** _type 取值：EVENT_TIMER 或 EVENT_TIMER_COMPLETE */
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
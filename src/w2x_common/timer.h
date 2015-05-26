/******************************************************************************
文件:	timer.h
描述:	可以创建新 Timer 对象，以便按指定的时间顺序运行代码。使用 Start() 方法来启动计时器。
		为 Timer 事件添加事件侦听器，以便将代码设置为按计时器间隔运行。可以创建 Timer 对象
		以运行一次或按指定间隔重复运行，从而按计划执行代码。计时器的最短更新时间为 50 毫秒，
		可能会按稍有偏差的间隔调度事件。
作者:	wxxweb
邮箱:	wxxweb@gmail.com
创建:	2015-04-10
修改:
*******************************************************************************/

#ifndef __W2X_COMMON_TIMER_H__
#define __W2X_COMMON_TIMER_H__
#pragma once


#include "event_dispatcher.h"
#include "timer_event.h"


W2X_NS_BEGIN
W2X_DEFINE_NS_BEGIN(utils)


class CTimer;
typedef CRefPtr<CTimer> TimerPtr;


class W2X_COMMON_API CTimer: public w2x::events::IEventDispatcher
{
private:
	CTimer(DWORD _delay, DWORD _repeat_count);
	virtual ~CTimer(void);

public:
	/**
	 * @brief 使用指定的 _delay 和 _repeat_ount 状态构造新的 Timer 对象。
	 * @note 计时器不会自动启动，必须调用 Start() 方法来启动它。
	 * @param _delay 计时器事件间的延迟（以毫秒为单位），如果小于 50 毫秒则为 50 毫秒。
	 * @param _repeat_count 指定重复次数。 运行指定次数，然后停止，如果为 INFINITE，
	 *                      则计时器重复无限次数，如果为 0，则只运行一次。
	 */
	static TimerPtr Create(DWORD _delay, DWORD _repeat_count = INFINITE);

public: // 计时器操作方法
	/**
	 * 如果计时器正在运行，则停止计时器，并将 currentCount 属性设回为 0，这类似于秒表的重置按钮。
	 * 然后，在调用 Start() 后，将运行计时器实例，运行次数为指定的重复次数（由 _repeat_count 值设置）。
	 */
	void Reset(void);

	/** 如果计时器尚未运行，则启动计时器。 */
	bool Start(void);

	/** 
	 * @brief 停止计时器。
	 * 如果在调用 Stop() 后调用 Start()，则将继续运行计时器实例，
	 * 运行次数为剩余的重复次数（由 repeatCount 属性设置）。
	 */
	void Stop(void);

public: // 计时器属性获取/设置

	/** 获取计时器的当前状态；如果计时器正在运行，则为 true，否则为 false。 */
	bool IsRunning(void) const;

	/** 获取计时器从 0 开始后触发的总次数。如果已重置了计时器，则只会计入重置后的触发次数。 */
	DWORD GetCurrentCount(void) const;

	/** 获取计时器事件间的延迟（以毫秒为单位）。 */
	DWORD GetDelay(void) const;

	/**
	 * @brief 设置计时器事件间的延迟（以毫秒为单位）。
	 * 如果在计时器正在运行时设置延迟间隔，则计时器将按相同的 repeatCount 迭代重新启动。
	 * 如果小于 50 毫秒则置为 50 毫秒。
	 */
	void SetDelay(DWORD _value);

	/** 获取计时器运行总次数 */
	DWORD GetRepeatCount(void) const;
	
	/**
	 * @brief 设置计时器运行总次数。
	 * 如果重复计数设置为 0，则计时器将持续不断运行，直至调用了 stop() 方法或程序停止。
	 * 如果重复计数不为 0，则将运行计时器，运行次数为指定的次数。如果设置的 repeatCount
	 * 总数等于或小于 currentCount，则计时器将停止并且不会再次触发。
	 */
	void SetRepeatCount(DWORD _value);

public: // 实现 events::IEventDispatcher 接口
	virtual bool AddEventListener(
		LPCTSTR _event_type,
		w2x::events::EventListenerPtr& _listener
		);

	virtual bool DispatchEvent(w2x::events::CEvent& _event) const;

	virtual bool HasEventListener(LPCTSTR _event_type) const;

	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		const w2x::events::EventListenerPtr& _listener
		);

	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		LPCTSTR _listener_id
		);

private:
	class CImpl;
	CImpl* const m_impl;
};


W2X_DEFINE_NS_END(utils)
W2X_NS_END


#endif // __DAOPHONE_COMMON_TIMER_H__
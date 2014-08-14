/*******************************************************************************
文件:	event_dispathcer.h
描述:	类 CEventDispatcher 用于管理事件监听器对事件的订阅，以及分发器对事件的分发。
		通常，让用户自定义的类能够调度事件，最简单方法是继承或聚合 CEventDispatcher。
		但是，如果由于逻辑关系或继承关系等缘故，无法或不便直接继承 CEventDispatcher，
		也可以通过实现 IEventDispatcher 接口，创建 CEventDispatcher 成员，并编写
		一些简单的包装函数，将调用连接到聚合的 CEventDispatcher 中。
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2014-02-23
修改:	2014-08-14
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_DISPATCHER_H__
#define __W2X_COMMON_EVENT_DISPATCHER_H__


#include "base.h"
#include "event_listener.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEvent;


class W2X_COMMON_API IEventDispatcher: public IBase
{
public:
	virtual ~IEventDispatcher(void) = 0 {}

W2X_IMPLEMENT_REFCOUNTING(IEventDispatcher)

public:
	/** 
	 * 注册事件监听器，用于订阅特定事件通知。
	 * @note 请使用 CEventDispatcher 对象注册事件侦听器函数，以使侦听器能够接收事件通知。
	 * @param _event_type 指定所要订阅的事件类型名称。
	 * @param _listener 事件监听器对象智能引用指针。
	 * @see IEventListener
	 */
	virtual bool AddEventListener(
		LPCTSTR _event_type,
		EventListenerPtr& _listener
	) = 0;

	/** 将事件分发给事件侦听器 */
	virtual bool DispatchEvent(CEvent& _event) const = 0;

	/** 检查是否为特定事件类型注册了事件侦听器 */
	virtual bool HasEventListener(LPCTSTR _event_type) const = 0;

	/** 根据事件监听器对象指针，移除已注册的事件监听器 */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		const EventListenerPtr& _listener
	) = 0;

	/** 根据事件监听器 ID，移除已注册的事件监听器 */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		LPCTSTR _listener_id
		) = 0;
};


class W2X_COMMON_API CEventDispatcher: public IEventDispatcher
{
public:
	/**
	 * CEventDispatcher 类通常作为基类，这意味着大多数情况下无需使用此构造函数，
	 * 但是，如果不便于继承 CEventDispatcher 或者需要实现 IEventDispatcher 接口，
	 * 那么请使用此构造函数来聚合 CEventDispatcher 类的实例。
	 * _target_name: 产生事件的目标对象名称，即事件目标。
	 */
	CEventDispatcher(LPCTSTR _target_name);

	virtual ~CEventDispatcher(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CEventDispatcher)

public:
	/** 注册事件侦听器函数，以使侦听器能够接收事件通知 */
	virtual bool AddEventListener(
		LPCTSTR _event_type,
		EventListenerPtr& _listener
	);

	/** 将事件分发给事件侦听器 */
	virtual bool DispatchEvent(CEvent& _event) const;

	/** 检查是否为特定事件类型注册了事件侦听器 */
	virtual bool HasEventListener(LPCTSTR _event_type) const;

	/** 根据事件监听器对象指针，移除已注册的事件监听器 */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		const EventListenerPtr& _listener
		);

	/** 根据事件监听器 ID，移除已注册的事件监听器 */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		LPCTSTR _listener_id
		);

private:
	class CDispImpl;
	CDispImpl* const m_disp_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_DISPATCHER_H__ */
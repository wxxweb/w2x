/******************************************************************************
 * 文件：	event_dispathcer.h
 * 描述：	IEventDispatcher 接口定义用于添加或删除事件侦听器的方法，检查是否已注册特定
 *          类型的事件侦听器，并调度事件。
 * 			通常，让用户自定义的类能够调度事件，最简单方法是继承或聚合 CEventDispatcher。
 * 			但是，如果由于逻辑关系或继承关系等缘故，无法或不便直接继承 CEventDispatcher，
 * 			也可以通过实现 IEventDispatcher 接口，创建 CEventDispatcher 成员，并编写
 * 			一些简单的包装函数，将调用连接到聚合的 CEventDispatcher 中。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_DISPATCHER_H__
#define __W2X_COMMON_EVENT_DISPATCHER_H__

#include "exports.h"
#include "macros.h"
#include "base.h"
#include "ref_ptr.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEvent;

/*
 * 事件侦听器函数类型。
 */
typedef void (*FEventListener)(const CEvent& _event_ref);

class W2X_COMMON_API IEventDispatcher: public IBase
{
public:
	virtual ~IEventDispatcher(void) = 0 {}

W2X_IMPLEMENT_REFCOUNTING(IEventDispatcher)

public:
	/*
	 * 请使用 CEventDispatcher 对象注册事件侦听器函数，以使侦听器能够接收事件通知。
	 * _source_ptr 为事件源指针，一般是指向事件侦听器 _listener 的拥有者类对象，
	 * 这样就可以在事件侦听器函数（全局函数或静态成员函数）中调用对象的成员函数。GC
	 * 会检测事件源对象的引用计数，若引用计数值为 1 则删除对应的事件侦听器。
	 */
	virtual bool AddEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener_fn,
		const CRefPtr<IBase>& _source_ptr
	) = 0;

	/*
	 * 将事件分发给事件侦听器。
	 */
	virtual bool DispatchEvent(CEvent& _event_ref) const = 0;

	/*
	 * 检查 CEventDispatcher 对象是否为特定事件类型注册了事件侦听器。
	 */
	virtual bool HasEventListener(LPCTSTR _type_name) const = 0;

	/*
	 * 从 CEventDispatcher 对象中删除事件侦听器。
	 */
	virtual bool RemoveEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener_fn
	) = 0;
};


class W2X_COMMON_API CEventDispatcher: public IEventDispatcher
{
public:
	/*
	 * CEventDispatcher 类通常作为基类，这意味着大多数情况下无需使用此构造函数，
	 * 但是，如果不便于继承 CEventDispatcher 或者需要实现 IEventDispatcher 接口，
	 * 那么请使用此构造函数来聚合 CEventDispatcher 类的实例。
	 * _target_ptr: 产生事件的目标对象指针，即事件目标。
	 */
	CEventDispatcher(const CRefPtr<IBase>& _target_ptr);

	virtual ~CEventDispatcher(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CEventDispatcher)

public:
	/*
	 * 注册事件侦听器函数，以使侦听器能够接收事件通知。
	 */
	virtual bool AddEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener_fn,
	 	const CRefPtr<IBase>& _source_ptr = NULL
	);

	/*
	 * 将事件分发给事件侦听器。
	 */
	virtual bool DispatchEvent(CEvent& _event_ref) const;

	/*
	 * 检查是否为特定事件类型注册了事件侦听器。
	 */
	virtual bool HasEventListener(LPCTSTR _type_name) const;

	/*
	 * 删除事件侦听器。
	 */
	virtual bool RemoveEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener_fn
	);

private:
	class CDispImpl;
	CDispImpl* const m_disp_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_DISPATCHER_H__ */
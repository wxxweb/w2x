/*******************************************************************************
文件:	event_listener.h
描述:	定义事件监听器接口类 IEventListener，当你需要使用不同类型的回调函数来定制事件监听
		器时，你需要继承这个接口类。通过它，你可以将全局函数/类静态成员函数和类成员函数作为
		事件回调函数。
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2014-08-13
修改:
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_LISTENER_H__
#define __W2X_COMMON_EVENT_LISTENER_H__

#include "macros.h"
#include "base.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)

class CEvent;
class IEventListener;
typedef CRefPtr<IEventListener> EventListenerPtr;

class W2X_COMMON_API IEventListener: public IBase
{
public:
	typedef void (CALLBACK *Function)(PVOID, const CEvent&);
	typedef void (CALLBACK IBase::*Selector)(const CEvent&);
	
public:
	virtual ~IEventListener(void) = 0;

W2X_IMPLEMENT_REFCOUNTING(IEventListener)

public:
	/** 使用全局函数/类静态函数创建事件监听器 */
	static EventListenerPtr Create(
		LPCTSTR _listener_id, 
		PVOID _target,
		Function _function
		);

	/** 使用类成员函数创建事件监听器 */
	static EventListenerPtr Create(
		LPCTSTR _listener_id, 
		IBase* _target,
		Selector _selector
		);

public:
	/** 检查事件监听器是否有效 */
	virtual bool IsValid(void) const = 0;

	/** 启用或者禁用事件监听器，仅当事件监听器为启用状态时才能接收到事件通知。*/
    virtual void SetEnabled(bool _enabled) = 0;

    /** 检查事件监听器是否启用 */
    virtual bool IsEnabled(void) const = 0;

	 /** 标记该事件监听器已经被事件分发器注册过 */
	virtual void SetRegistered(bool _registered);

	/** 检查该事件监听器是否被事件分发器注册过 */
	virtual bool IsRegistered() const = 0;

	/** 获取事件监听器 ID，可通过这个 ID 来查找事件监听器 */
    virtual LPCTSTR GetListenerId(void) const = 0;

	/** 执行事件监听器事件回调函数 */
	virtual void Execute(const CEvent& _event) = 0;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_LISTENER_H__ */
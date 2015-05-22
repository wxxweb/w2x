/*******************************************************************************
 * 文件:	event_listener.h
 * 描述:	定义事件监听器接口类 IEventListener，当你需要使用不同类型的回调函数来定制事件监听
 *			器时，你需要继承这个接口类。通过它，你可以将全局函数/类静态成员函数和类成员函数作为
 * 			事件回调函数。
 * 作者:	wu.xiongxing
 * 邮箱:	wxxweb@gmail.com
 * 日期:	2014-08-13
 * 修改:	2015-05-22
 ******************************************************************************/

#ifndef __W2X_COMMON_EVENT_LISTENER_H__
#define __W2X_COMMON_EVENT_LISTENER_H__

#include "macros.h"
#include "base.h"
#include <functional>


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)

class CEvent;
class IEventListener;
typedef CRefPtr<IEventListener> EventListenerPtr;

class W2X_COMMON_API IEventListener: public IBase
{
public:
	typedef std::function<void(CEvent&)> Callback;
	
public:
	virtual ~IEventListener(void) = 0;

W2X_IMPLEMENT_REFCOUNTING(IEventListener)

public:
	/** 
	 * 创建事件监听器。
	 * @param _listener_id 事件监听器唯一标识，如果为 NULL 或空字符串则自动分配。
	 * @param _callback 类函数对象，用做事件回调，可通过宏 W2X_CALLBACK_1 来构建。
	 * @see Callback
	 */
	static EventListenerPtr Create(
		LPCTSTR _listener_id,
		const Callback& _callback
		);

public:
	/** 检查事件监听器是否有效，当事件回调为空值时返回 false */
	virtual bool IsValid(void) const = 0;

	/** 检查检查两个对象是否为同一事件监听器，事件监听器 ID 相同即认为是同一对象 */
	virtual bool IsSame(const EventListenerPtr& _right) const = 0;

	/** 获取事件监听器 ID，可通过这个 ID 来查找事件监听器 */
	virtual LPCTSTR GetListenerId(void) const = 0;

	/** 启用或者禁用事件监听器，仅当事件监听器为启用状态时才能接收到事件通知。*/
    virtual void SetEnabled(bool _enabled) = 0;

    /** 检查事件监听器是否启用 */
    virtual bool IsEnabled(void) const = 0;

	 /** 标记该事件监听器已经被事件分发器注册过，若反注册则将在下次调用后从监听器列表中移除 */
	virtual void SetRegistered(bool _registered) = 0;

	/** 检查该事件监听器是否被事件分发器注册过 */
	virtual bool IsRegistered(void) const = 0;

	/** @private
	 * 在事件监听器被调用的过程中须对其进行保护，防止在同线程中移除监听器引发异常
	 */
	virtual void SetProtected(bool _protected) = 0;

	/** @private
	 * 检查该事件监听器是否处于保护状态
	 */
	virtual bool IsProtected(void) const = 0;

	/** @private
	 * 获取事件回调对象
	 */
	virtual const Callback& GetCallback(void) const = 0;

	/** @private
	 * 执行事件监听器事件回调函数
	 */
	virtual void Execute(CEvent& _event) = 0;
};


#define W2X_CREATE_EVENT_LISTENER(__listener_id__, __selector__, __target__) \
    w2x::events::IEventListener::Create(__listener_id__, \
        W2X_CALLBACK_1(__selector__, __target__))


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_LISTENER_H__ */
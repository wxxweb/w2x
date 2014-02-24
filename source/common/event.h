/******************************************************************************
 * 文件：	event.h
 * 描述：	CEvent 类作为创建事件对象的基类，当事件发生时，CEvent 对象将作为参数传递给
 *			事件侦听器。CEvent 类的属性中包含事件的基本信息，例如事件的类型名称、触发事
 *			件的事件源和产生事件的时间目标。您可以通过扩展 CEvent 类（比如派生出鼠标事件
 *			CMouseEvent）将事件的其他信息传递给事件侦听器。
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-24
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_H__
#define __W2X_COMMON_EVENT_H__

#include "exports.h"
#include "base.h"
#include "ref_ptr.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class W2X_COMMON_API CEvent: public IBase
{
public:
	/*
	 * 通过指定事件类型名称，创建一个作为参数传递给事件侦听器的 CEvent 对象。
	 * 注意，考虑到模块间的解耦合，使用名称字符串来指定事件类型，这些事件类型
	 * 名称务必定义在逻辑模块中，本模块不处理逻辑，只提供一套事件机制。
	 */
	CEvent(LPCTSTR _type_name);

	virtual ~CEvent(void);

W2X_IMPLEMENT_REFCOUNTING(CEvent)

public:
	/*
	 * 获取事件类型名称。
	 */
	virtual LPCTSTR GetTypeName(void) const;

	/*
	 * 获取触发事件的源对象，源对象（事件源）是指主动触发事件的对象，而不是
	 * 被动产生事件的对象。比如某人咬了一口苹果，那么这个人就是事件源，他主
	 * 动触发了咬苹果这个动作事件，苹果就是这个动作事件的目标，被咬的对象。
	 */
	virtual CRefPtr<IBase>& GetSource(void) const;

	/*
	 * 设置触发事件的源对象，源对象（事件源）是指主动触发事件的对象，而不是
	 * 被动产生事件的对象。比如某人咬了一口苹果，那么这个人就是事件源，他主
	 * 动触发了咬苹果这个动作事件，苹果就是这个动作事件的目标，被咬的对象。
	 */
	virtual void SetSource(const CRefPtr<IBase>& _source_ptr);

	/*
	 * 获取产生事件的目标对象，目标对象（事件目标）是指被动产生事件的对象，而
	 * 不是主动触发事件的对象。比如某人咬了一口苹果，那么苹果就是这个动作事件
	 * 的目标，被咬的对象，这个人就是事件源，他主动触发了咬苹果这个动作事件。
	 */
	virtual CRefPtr<IBase>& GetTarget(void) const;

	/*
	 * 设置产生事件的目标对象，目标对象（事件目标）是指被动产生事件的对象，而
	 * 不是主动触发事件的对象。比如某人咬了一口苹果，那么苹果就是这个动作事件
	 * 的目标，被咬的对象，这个人就是事件源，他主动触发了咬苹果这个动作事件。
	 */
	virtual void SetTarget(const CRefPtr<IBase>& _target_ptr);

private:
	class CEventImpl;
	CEventImpl* const m_event_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_H__ */
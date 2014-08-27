/******************************************************************************
文件:	event.h
描述:	CEvent 类作为创建事件对象的基类，当事件发生时，CEvent 对象将作为参数传递给
		事件侦听器。CEvent 类的属性中包含事件的基本信息，例如事件的类型名称、触发事
		件的事件源和产生事件的时间目标。您可以通过扩展 CEvent 类（比如派生出鼠标事件
		CMouseEvent）将事件的其他信息传递给事件侦听器。
作者:	wu.xiongxing
邮箱:	wxxweb@gmail.com
日期:	2014-02-24
修改:	2014-05-21
 ******************************************************************************/

#ifndef __W2X_COMMON_EVENT_H__
#define __W2X_COMMON_EVENT_H__

#include "macros.h"
#include "base.h"
#include "ref_ptr.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class W2X_COMMON_API CEvent: public IBase
{
public:
	static const TCHAR EVENT_UNKNOW[];		///< 未知事件类型
	static const TCHAR TARGET_UNNAMED[];	///< 目标对象未命名

public:
	/**
	 * 通过指定事件类型名称，创建一个作为参数传递给事件侦听器的 CEvent 对象。
	 * 注意，考虑到模块间的解耦合，使用名称字符串来指定事件类型，这些事件类型
	 * 名称务必定义在逻辑模块中，本模块不处理逻辑，只提供一套事件机制。
	 */
	CEvent(LPCTSTR _type_name);

	virtual ~CEvent(void);

W2X_IMPLEMENT_REFCOUNTING(CEvent)

public:
	/** 重置事件所有内部信息 */
	virtual void Reset(void);

	/** 获取事件类型名称 */
	virtual LPCTSTR GetTypeName(void) const;

	/** 设置事件类型名称 */
	virtual void SetTypeName(LPCTSTR _type_name);

	/** 判断是否为指定类型事件 */
	virtual bool IsTypeOf(LPCTSTR _type_name) const;

	/** 判断是否为相同事件类型 */
	virtual bool IsTypeOf(const CEvent& _event) const;

	/** 判断是否为指定事件类型 */
	bool operator ==(LPCTSTR _type_name) const;

	/** 判断是否为相同事件类型 */
	bool operator ==(const CEvent& _event) const;

	/** 判断是否不为指定事件类型 */
	bool operator !=(LPCTSTR _type_name) const;

	/** 判断是否不为相同事件类型 */
	bool operator !=(const CEvent& _event) const;

	/**
	 * 获取产生事件的目标对象名称，目标对象（事件目标）是指被动产生事件的对象，而
	 * 不是主动触发事件的对象。比如某人咬了一口苹果，那么苹果就是这个动作事件
	 * 的目标，被咬的对象，这个人就是事件源，他主动触发了咬苹果这个动作事件。
	 */
	virtual LPCTSTR GetTargetName(void) const;

	/**
	 * @private
	 * 设置产生事件的目标对象名称，目标对象（事件目标）是指被动产生事件的对象，而
	 * 不是主动触发事件的对象。比如某人咬了一口苹果，那么苹果就是这个动作事件
	 * 的目标，被咬的对象，这个人就是事件源，他主动触发了咬苹果这个动作事件。
	 * @note 该函数被事件分发器 CEventDispatcher 调用，外部请勿使用！
	 */
	virtual void SetTargetName(LPCTSTR _name);

private:
	class CEventImpl;
	CEventImpl* const m_event_impl_ptr;
};


/** 判断是否为指定事件类型 */
inline bool operator ==(LPCTSTR _type_name, const CEvent& _event)
{
	return _event.IsTypeOf(_type_name);
}


/** 判断是否不为指定事件类型 */
inline bool operator !=(LPCTSTR _type_name, const CEvent& _event)
{
	return (false == _event.IsTypeOf(_type_name));
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_H__ */
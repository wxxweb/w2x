/*****************************************************************************
文件名：		thread.h
描述：		将该文件中定义了线程相关的一些帮助器，如互斥、同步等.
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2013.12.10
******************************************************************************/

#ifndef __W2X_COMMON_THREAD_H__
#define __W2X_COMMON_THREAD_H__

#include "exports.h"
#include "utility.h"

W2X_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
// 这是个临界区对象包装类, 当类对象被构造时初始化临界区对象, 类对象被析构时删除临界区对象.
// 通过调用 enter() 进入临界区, 调用 leave() 离开临界区.
//-----------------------------------------------------------------------------
class W2X_COMMON_API CCriticalSection
{
public:
	CCriticalSection(void);
	virtual ~CCriticalSection(void);

	W2X_DISALLOW_COPY_AND_ASSIGN(CCriticalSection)

public:
	void enter(void);
	void leave(void);

private:
	class CImpl;
	CImpl* const m_impl;
};

W2X_NAMESPACE_END

//-----------------------------------------------------------------------------
// 该宏提供了一个互斥锁的实现过程.通过使用 lock() 和 unlock() 方法来达到在多线程同步访问
// 时保护一段代码的目的. class_lock 类是一个帮助器,用于在作用域内持有互斥锁.
//-----------------------------------------------------------------------------
#define W2X_IMPLEMENT_LOCKING(class_parent, class_lock)                      \
protected:                                                                   \
	class class_lock {                                                       \
	public:                                                                  \
		explicit class_lock(class_parent* parent) : m_parent(parent)         \
		{ m_parent->lock(); }                                                \
		~class_lock(void) { m_parent->unlock(); }							 \
	private:                                                                 \
		class_parent* m_parent;                                              \
	};                                                                       \
	void lock() { m_critical_section_##class_lock.enter(); }                 \
	void unlock() { m_critical_section_##class_lock.leave(); }               \
private:                                                                     \
	w2x::CCriticalSection m_critical_section_##class_lock;

#endif /* __W2X_COMMON_THREAD_H__ */
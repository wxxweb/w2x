/******************************************************************************
文件:	mutex.h
描述:	提供线程同步的相关帮助器，如互斥体、临界代码段保护等.
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2013-12-10
修改:	2014-07-02
*******************************************************************************/

#ifndef __W2X_COMMON_MUTEX_H__
#define __W2X_COMMON_MUTEX_H__

#include "macros.h"

/** 对一个 32 位的变量进行原子递增及递减 */
#define W2X_ATOMIC_INCREMENT(ptr) InterlockedIncrement(ptr)
#define W2X_ATOMIC_DECREMENT(ptr) InterlockedDecrement(ptr)


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(mutex)

//-----------------------------------------------------------------------------
// 线程互斥体，对临界区代码段保护进行 RAII 封装, 当线程互斥体对象被构造时初始化临界区对象，
// 对象被析构时删除临界区对象。通过调用 Lock() 进入临界区，调用 Unlock() 离开临界区。
//-----------------------------------------------------------------------------
class W2X_COMMON_API CThreadMutex
{
public:
	CThreadMutex(void);
	~CThreadMutex(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CThreadMutex)

public:
	inline void Lock(void) {
		::EnterCriticalSection(&m_cs);
	}
	inline void Unlock(void) {
		::LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};


//-----------------------------------------------------------------------------
// 作用域锁，对线程互斥体进行 RAII 封装，当作用域锁对象构造时对线程加锁，析构时对线程解锁。
//-----------------------------------------------------------------------------
class W2X_COMMON_API CScopedLock
{
public:
	CScopedLock(CThreadMutex& _mutex);
	~CScopedLock(void);

private:
	CThreadMutex& m_mutex;
};


W2X_DEFINE_NAME_SPACE_END(mutex)
W2X_NAME_SPACE_END

//-----------------------------------------------------------------------------
// 该宏提供了一个互斥锁的实现过程。class_parent 是拥有互斥锁的父类，它将拥有两个互斥锁，一
// 个为 class_parent 的实例成员变量，另一个为 class_parent 的静态成员变量，分别对实例及
// 类本身进行加锁保护。class_lock 类是一个帮助器，你需要给它取个名字，比如叫 CAutoLock，
// 它的实例用于在作用域内持有互斥锁，它通过调用函数 LockThis() 和 UnlockThis() 来实现在
// 多线程同步访问时，保护类 class_parent 实例的一段代码。通过调用函数 LockClass() 和
// UnlockClass() 来实现在多线程同步访问时，保护类 class_parent 本身的一段代码。
//-----------------------------------------------------------------------------
#define W2X_IMPLEMENT_LOCKING(_class_parent, _class_lock)                      \
protected:                                                                     \
	class _class_lock {                                                        \
	public:                                                                    \
		explicit _class_lock(_class_parent* _parent)                           \
			: m_parent(_parent) {                                              \
			NULL != _parent ? _parent->LockThis()                              \
				: _class_parent::LockClass();                                  \
        }                                                                      \
		~_class_lock(void) {                                                   \
			NULL != m_parent ? m_parent->UnlockThis()                          \
				: _class_parent::UnlockClass();                                \
        }                                                                      \
	private:                                                                   \
		_class_parent* const m_parent;										   \
	};                                                                         \
	void LockThis() { m_mutex_of_this_by_##_class_lock.Lock(); }               \
	void UnlockThis() { m_mutex_of_this_by_##_class_lock.Unlock(); }           \
	static void LockClass() { sm_mutex_of_class_by_##_class_lock.Lock(); }     \
    static void UnlockClass() { sm_mutex_of_class_by_##_class_lock.Unlock(); } \
private:                                                                       \
	w2x::mutex::CThreadMutex m_mutex_of_this_by_##_class_lock;                 \
	static w2x::mutex::CThreadMutex sm_mutex_of_class_by_##_class_lock;

#define W2X_IMPLEMENT_LOCKING_CLASS(_class_parent, _class_lock)                \
	w2x::mutex::CThreadMutex _class_parent::sm_mutex_of_class_by_##_class_lock;


#endif /* __W2X_COMMON_MUTEX_H__ */
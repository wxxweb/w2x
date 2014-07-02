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
class CThreadMutex
{
public:
	CThreadMutex(void) {
		memset(&m_cs, 0, sizeof(CRITICAL_SECTION));
		::InitializeCriticalSection(&m_cs);
	}
	virtual ~CThreadMutex(void) {
		::DeleteCriticalSection(&m_cs);
	}

W2X_DISALLOW_COPY_AND_ASSIGN(CThreadMutex)

public:
	void Lock(void) {
		::EnterCriticalSection(&m_cs);
	}
	void Unlock(void) {
		::LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};


//-----------------------------------------------------------------------------
// 作用域锁，对线程互斥体进行 RAII 封装，当作用域锁对象构造时对线程加锁，析构时对线程解锁。
//-----------------------------------------------------------------------------
class CScopedLock
{
public:
	CScopedLock(CThreadMutex& _mutex) : m_mutex(_mutex) {
		m_mutex.Lock();
	}

	virtual ~CScopedLock(void) {
		m_mutex.Unlock();
	}

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
#define W2X_IMPLEMENT_LOCKING(class_parent, class_lock)                        \
protected:                                                                     \
	class class_lock {                                                         \
	public:                                                                    \
		explicit class_lock(class_parent* _parent, const char* _name = 0)      \
			: m_parent(_parent), m_name(_name) {                               \
            if (NULL != m_name) {                                              \
				::OutputDebugStringA(                                          \
				    NULL != m_parent ? "\n[LockThis]:\t" : "[LockClass]:\t");  \
				::OutputDebugStringA(m_name);                                  \
			}                                                                  \
			NULL != m_parent ? m_parent->LockThis()                            \
				: class_parent::LockClass();                                   \
        }                                                                      \
		~class_lock(void) {                                                    \
			if (NULL != m_name) {                                              \
				::OutputDebugStringA(                                          \
					NULL != m_parent ? "\n[LockThis]:\t" : "[LockClass]:\t");  \
				::OutputDebugStringA(m_name);                                  \
			}                                                                  \
			NULL != m_parent ? m_parent->UnlockThis()                          \
				: class_parent::UnlockClass();                                 \
        }                                                                      \
	private:                                                                   \
		class_parent* const m_parent;										   \
		const char* m_name;												       \
	};                                                                         \
	void LockThis() { m_mutex_of_this_by_##class_lock.Lock(); }                \
	void UnlockThis() { m_mutex_of_this_by_##class_lock.Unlock(); }            \
	static void LockClass() { sm_mutex_of_class_by_##class_lock.Lock(); }      \
    static void UnlockClass() { sm_mutex_of_class_by_##class_lock.Unlock(); }  \
private:                                                                       \
	w2x::mutex::CThreadMutex m_mutex_of_this_by_##class_lock;                  \
	static w2x::mutex::CThreadMutex sm_mutex_of_class_by_##class_lock;

#define W2X_IMPLEMENT_LOCKING_CLASS(class_parent, class_lock)                  \
	w2x::mutex::CThreadMutex class_parent::sm_mutex_of_class_by_##class_lock;


#endif /* __W2X_COMMON_MUTEX_H__ */
/******************************************************************************
�ļ�:	mutex.h
����:	�ṩ�߳�ͬ������ذ��������绥���塢�ٽ����α�����.
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2013-12-10
�޸�:		2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_MUTEX_H__
#define __W2X_COMMON_MUTEX_H__

#include "macros.h"

/** ��һ�� 32 λ�ı�������ԭ�ӵ������ݼ� */
#define W2X_ATOMIC_INCREMENT(ptr) InterlockedIncrement(ptr)
#define W2X_ATOMIC_DECREMENT(ptr) InterlockedDecrement(ptr)


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(mutex)

//-----------------------------------------------------------------------------
// �̻߳����壬���ٽ�������α������� RAII ��װ, ���̻߳�������󱻹���ʱ��ʼ���ٽ�������
// ��������ʱɾ���ٽ�������ͨ������ Lock() �����ٽ��������� Unlock() �뿪�ٽ�����
//-----------------------------------------------------------------------------
class W2X_COMMON_API CThreadMutex
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
// �������������̻߳�������� RAII ��װ������������������ʱ���̼߳���������ʱ���߳̽�����
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
// �ú��ṩ��һ����������ʵ�ֹ��̡�class_parent ��ӵ�л������ĸ��࣬����ӵ��������������һ
// ��Ϊ class_parent ��ʵ����Ա��������һ��Ϊ class_parent �ľ�̬��Ա�������ֱ��ʵ����
// �౾����м���������class_lock ����һ��������������Ҫ����ȡ�����֣������ CAutoLock��
// ����ʵ���������������ڳ��л���������ͨ�����ú��� LockThis() �� UnlockThis() ��ʵ����
// ���߳�ͬ������ʱ�������� class_parent ʵ����һ�δ��롣ͨ�����ú��� LockClass() ��
// UnlockClass() ��ʵ���ڶ��߳�ͬ������ʱ�������� class_parent �����һ�δ��롣
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
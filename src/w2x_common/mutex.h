/******************************************************************************
�ļ�:	mutex.h
����:	�ṩ�߳�ͬ������ذ��������绥���塢�ٽ����α�����.
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2013-12-10
�޸�:	2014-07-02
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
// �������������̻߳�������� RAII ��װ������������������ʱ���̼߳���������ʱ���߳̽�����
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
// �ú��ṩ��һ����������ʵ�ֹ��̡�class_parent ��ӵ�л������ĸ��࣬����ӵ��������������һ
// ��Ϊ class_parent ��ʵ����Ա��������һ��Ϊ class_parent �ľ�̬��Ա�������ֱ��ʵ����
// �౾����м���������class_lock ����һ��������������Ҫ����ȡ�����֣������ CAutoLock��
// ����ʵ���������������ڳ��л���������ͨ�����ú��� LockThis() �� UnlockThis() ��ʵ����
// ���߳�ͬ������ʱ�������� class_parent ʵ����һ�δ��롣ͨ�����ú��� LockClass() ��
// UnlockClass() ��ʵ���ڶ��߳�ͬ������ʱ�������� class_parent �����һ�δ��롣
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
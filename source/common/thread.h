/*****************************************************************************
�ļ���	thread.h
������	�����ļ��ж������߳���ص�һЩ���������绥�⡢ͬ����.
���ߣ�	wu.xiongxing					
���䣺	wxxweb@gmail.com
���ڣ�	2013-12-10
******************************************************************************/

#ifndef __W2X_COMMON_THREAD_H__
#define __W2X_COMMON_THREAD_H__

#include "exports.h"
#include "macros.h"

W2X_NAME_SPACE_BEGIN

//-----------------------------------------------------------------------------
// ���Ǹ��ٽ��������װ��, ������󱻹���ʱ��ʼ���ٽ�������, ���������ʱɾ���ٽ�������.
// ͨ������ enter() �����ٽ���, ���� leave() �뿪�ٽ���.
//-----------------------------------------------------------------------------
class W2X_COMMON_API CCriticalSection
{
public:
	CCriticalSection(void);
	virtual ~CCriticalSection(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CCriticalSection)

public:
	void Enter(void);
	void Leave(void);

private:
	class CImpl;
	CImpl* const m_impl;
};

W2X_NAME_SPACE_END

//-----------------------------------------------------------------------------
// �ú��ṩ��һ����������ʵ�ֹ��̡�class_parent ��ӵ�л������ĸ��࣬����ӵ��������������һ
// ��Ϊ class_parent ��ʵ����Ա��������һ��Ϊ class_parent �ľ�̬��Ա�������ֱ��ʵ����
// �౾����м���������class_lock ����һ��������������Ҫ����ȡ�����֣������ CAutoLock��
// ����ʵ���������������ڳ��л���������ͨ�����ú��� LockThis() �� UnlockThis() ��ʵ����
// ���߳�ͬ������ʱ�������� class_parent ʵ����һ�δ��롣ͨ�����ú��� LockClass() ��
// UnlockClass() ��ʵ���ڶ��߳�ͬ������ʱ�������� class_parent �����һ�δ��롣
//-----------------------------------------------------------------------------
#define W2X_IMPLEMENT_LOCKING(class_parent, class_lock)                      \
protected:                                                                   \
	class class_lock {                                                       \
	public:                                                                  \
		explicit class_lock(class_parent* parent) : m_parent(parent) {       \
			NULL != m_parent ? m_parent->LockThis()                          \
				: class_parent::LockClass();                                 \
        }                                                                    \
		~class_lock(void) {                                                  \
			NULL != m_parent ? m_parent->UnlockThis()                        \
				: class_parent::UnlockClass();                               \
        }                                                                    \
	private:                                                                 \
		class_parent* m_parent;                                              \
	};                                                                       \
	void LockThis() { m_cs_of_this_by_##class_lock.Enter(); }                \
	void UnlockThis() { m_cs_of_this_by_##class_lock.Leave(); }              \
	static void LockClass() { sm_cs_of_class_by_##class_lock.Enter(); }      \
    static void UnlockClass() { sm_cs_of_class_by_##class_lock.Leave(); }    \
private:                                                                     \
	w2x::CCriticalSection m_cs_of_this_by_##class_lock;                      \
	static w2x::CCriticalSection sm_cs_of_class_by_##class_lock;

#define W2X_IMPLEMENT_LOCKING_CLASS(class_parent, class_lock)                \
	w2x::CCriticalSection class_parent::sm_cs_of_class_by_##class_lock;


#endif /* __W2X_COMMON_THREAD_H__ */
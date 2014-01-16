/*****************************************************************************
�ļ�����		thread.h
������		�����ļ��ж������߳���ص�һЩ���������绥�⡢ͬ����.
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2013.12.10
******************************************************************************/

#ifndef __W2X_COMMON_THREAD_H__
#define __W2X_COMMON_THREAD_H__

#include "exports.h"
#include "utility.h"

W2X_NAMESPACE_BEGIN

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
	void enter(void);
	void leave(void);

private:
	class CImpl;
	CImpl* const m_impl;
};

W2X_NAMESPACE_END

//-----------------------------------------------------------------------------
// �ú��ṩ��һ����������ʵ�ֹ���.ͨ��ʹ�� lock() �� unlock() �������ﵽ�ڶ��߳�ͬ������
// ʱ����һ�δ����Ŀ��. class_lock ����һ��������,�������������ڳ��л�����.
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
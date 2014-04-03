/******************************************************************************
 * �ļ���	ref_count.h
 * ������	�� CRefCount ʵ����ԭ�Ӽ�������ü�����
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_REF_COUNT_H__
#define __W2X_COMMON_REF_COUNT_H__

#include "mutex.h"


W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API CRefCount 
{
public:
	CRefCount() : m_ref_count(0) {}

	/*
	 * ԭ�Ӽ�������ü���ֵ������
	 */
	int w2xAddRef() {
		return W2X_ATOMIC_INCREMENT(&m_ref_count);
	}

	/*
	 * ԭ�Ӽ�������ü���ֵ�ݼ��������ٱ�����ʱɾ������
	 */
	int w2xRelease() {
		return W2X_ATOMIC_DECREMENT(&m_ref_count);
	}

	/*
	 * ���ص�ǰ�����ü���ֵ��
	 */
	int w2xGetRefCount() { 
		return m_ref_count; 
	}

private:
	long m_ref_count;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_COUNT_H__ */
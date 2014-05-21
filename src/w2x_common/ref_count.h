/******************************************************************************
 * �ļ�:		ref_count.h
 * ����:		�� CRefCount ʵ����ԭ�Ӽ�������ü�����
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-02-23
 * �޸�:		2014-05-21
 ******************************************************************************/

#ifndef __W2X_COMMON_REF_COUNT_H__
#define __W2X_COMMON_REF_COUNT_H__

#include "macros.h"

W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API CRefCount
{
public:
	CRefCount() : m_ref_count(0) {}

	/** ԭ�Ӽ�������ü���ֵ������*/
	int w2xAddRef() {
		return InterlockedIncrement(&m_ref_count);
	}

	/** ԭ�Ӽ�������ü���ֵ�ݼ��������ٱ�����ʱɾ������*/
	int w2xRelease() {
		return InterlockedIncrement(&m_ref_count);
	}

	/** ���ص�ǰ�����ü���ֵ��*/
	int w2xGetRefCount() { 
		return m_ref_count; 
	}

private:
	long m_ref_count;
};


/**
 * �ú�Ϊ�̳� IBase �����ṩ���ü����Ĵ���ʵ�֡�
 */
#define W2X_IMPLEMENT_REFCOUNTING(class_name)                   \
public:                                                         \
    /** �������øú���������ʹ������ָ�� CRefPtr �� attach() ��� */  \
	int w2xAddRef(void) {                                       \
        return m_ref_count.w2xAddRef();                         \
     }                                                          \
	/** �������øú���������ʹ������ָ�� CRefPtr �� detach() ��� */  \
	int w2xRelease(void) {                                      \
		int retval = m_ref_count.w2xRelease();                  \
		if (0 == retval) { delete this; }                       \
		return retval;                                          \
	}                                                           \
	int w2xGetRefCount(void) {                                  \
        return m_ref_count.w2xGetRefCount();                    \
    }                                                           \
private:                                                        \
    w2x::CRefCount m_ref_count;


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_COUNT_H__ */
/******************************************************************************
 * �ļ���	base.h
 * ������	IBase �ӿ��ඨ�������ü�����ʵ�ַ�����Ϊ�˱��ⲻС�ĵ��µ��ڴ�й¶�����⣬��
 *          ����඼�б�Ҫ�̳иýӿڡ����ͨ���� W2X_IMPLEMENT_REFCOUNTING �� IBase
 *			��������ʵ�����ü�����ؽӿں�����Ӧ���������ֶ����� AddRef() �� Release()
 *			���޸����ü���ֵ����õİ취��ʹ������ָ���� CRefPtr ������ IBase �������
 *			��ָ�롣��Ҫֱ�Ӳ��� IBase ��������ָ�룬���ǽ�ָ�븳�� CRefPtr ����ͨ��
 *			CRefPtr ��������Ӳ�������μ� ref_ptr.h
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_BASE_H__
#define __W2X_COMMON_BASE_H__

#include "exports.h"
#include "ref_count.h"


W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API IBase
{
public:
	IBase(void);

protected:
	/*
	 * ���Ǽ̳��˸ýӿڵ�����󣬶��޷�ֱ��ͨ�� delete ���ֶ��ͷŶ����ڴ棬ʹ��
	 * delete ������ֱ��������Ҫ�ͷŶ����ڴ���ʹ�ö���� w2xRelease() ������ʵ�֡�
	 */
	virtual ~IBase(void);

public:
	/**
	 * ʹ��������ü���ֵ������
	 * @return �������ü����Ľ��ֵ����ֵӦ��ֻ������ϻ���ԡ�
	 * @note ÿ����ȡ����ָ����¸�������������øú�����
	 */
	virtual int w2xAddRef(void) = 0;

	/**
	 * ʹ��������ü���ֵ�ݼ���
	 * @return ���ü����Ľ��ֵ�������أ���ֵӦ��ֻ������ϻ���ԡ�
	 * @note ����������ü���ֵ���� 0 ʱ�����뽫������ڴ����ͷŵ���
	 */
	virtual int w2xRelease(void) = 0;

	/**
	 * @return ���ض���ǰ���ü���ֵ��
	 */
	virtual int w2xGetRefCount(void) = 0;
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


#endif /* __W2X_COMMON_BASE_H__ */
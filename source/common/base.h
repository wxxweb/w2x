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
protected:
	/*
	 * ���Ǽ̳��˸ýӿڵ�����󣬶��޷�ֱ��ͨ�� delete ���ֶ��ͷŶ����ڴ棬ʹ��
	 * delete ������ֱ��������Ҫ�ͷŶ����ڴ���ʹ�ö���� Release() ������ʵ�֡�
	 */
	virtual ~IBase(void) = 0 {}

public:
	/*
	 * ���� AddRef() ʹ��������ü���ֵ������ÿ����ȡ����ָ����¸�������������øú�����
	 * ���ü����Ľ��ֵ�������أ���ֵӦ��ֻ������ϻ���ԡ�
	 */
	virtual int AddRef(void) = 0;

	/*
	 * ���� Release() ʹ��������ü���ֵ�ݼ�������������ü���ֵ���� 0 ʱ�����뽫�����
	 * �ڴ����ͷŵ������ü����Ľ��ֵ�������أ���ֵӦ��ֻ������ϻ���ԡ�
	 */
	virtual int Release(void) = 0;

	/*
	 * ���ض���ǰ���ü���ֵ��
	 */
	virtual int GetRefCount(void) = 0;
};


/*
 * �ú�Ϊ�̳� IBase �����ṩ���ü����Ĵ���ʵ�֡�
 */
#define W2X_IMPLEMENT_REFCOUNTING(class_name)                   \
public:                                                         \
	int AddRef(void) { return m_ref_count.AddRef(); }           \
	int Release(void) {                                         \
		int retval = m_ref_count.Release();                     \
		if (0 == retval) { delete this; }                       \
		return retval;                                          \
	}                                                           \
	int GetRefCount(void) { return m_ref_count.GetRefCount(); } \
private:                                                        \
  CRefCount m_ref_count;


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_BASE_H__ */
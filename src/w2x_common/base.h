/******************************************************************************
 * �ļ�:		base.h
 * ����:		IBase �ӿ��ඨ�������ü�����ʵ�ַ�����Ϊ�˱��ⲻС�ĵ��µ��ڴ�й¶�����⣬��
 *          ����඼�б�Ҫ�̳иýӿڡ����ͨ���� W2X_IMPLEMENT_REFCOUNTING �� IBase
 *			��������ʵ�����ü�����ؽӿں�����Ӧ���������ֶ����� AddRef() �� Release()
 *			���޸����ü���ֵ����õİ취��ʹ������ָ���� CRefPtr ������ IBase �������
 *			��ָ�롣��Ҫֱ�Ӳ��� IBase ��������ָ�룬���ǽ�ָ�븳�� CRefPtr ����ͨ��
 *			CRefPtr ��������Ӳ�������μ� ref_ptr.h
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-02-23
 * �޸�:		2014-04-19
 ******************************************************************************/

#ifndef __W2X_COMMON_BASE_H__
#define __W2X_COMMON_BASE_H__

#include "macros.h"
#include "ref_count.h"
#include "ref_ptr.h"

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


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_BASE_H__ */
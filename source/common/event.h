/******************************************************************************
 * �ļ���	event.h
 * ������	CEvent ����Ϊ�����¼�����Ļ��࣬���¼�����ʱ��CEvent ������Ϊ�������ݸ�
 *			�¼���������CEvent ��������а����¼��Ļ�����Ϣ�������¼����������ơ�������
 *			�����¼�Դ�Ͳ����¼���ʱ��Ŀ�ꡣ������ͨ����չ CEvent �ࣨ��������������¼�
 *			CMouseEvent�����¼���������Ϣ���ݸ��¼���������
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-24
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_H__
#define __W2X_COMMON_EVENT_H__

#include "exports.h"
#include "base.h"
#include "ref_ptr.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class W2X_COMMON_API CEvent: public IBase
{
public:
	/*
	 * ͨ��ָ���¼��������ƣ�����һ����Ϊ�������ݸ��¼��������� CEvent ����
	 * ע�⣬���ǵ�ģ���Ľ���ϣ�ʹ�������ַ�����ָ���¼����ͣ���Щ�¼�����
	 * ������ض������߼�ģ���У���ģ�鲻�����߼���ֻ�ṩһ���¼����ơ�
	 */
	CEvent(LPCTSTR _type_name);

	virtual ~CEvent(void);

W2X_IMPLEMENT_REFCOUNTING(CEvent)

public:
	/*
	 * ��ȡ�¼��������ơ�
	 */
	virtual LPCTSTR GetTypeName(void) const;

	/*
	 * ��ȡ�����¼���Ŀ�����Ŀ������¼�Ŀ�꣩��ָ���������¼��Ķ��󣬶�
	 * �������������¼��Ķ��󡣱���ĳ��ҧ��һ��ƻ������ôƻ��������������¼�
	 * ��Ŀ�꣬��ҧ�Ķ�������˾����¼�Դ��������������ҧƻ����������¼���
	 */
	virtual CRefPtr<IBase>& GetTarget(void) const;

	/*
	 * ���ò����¼���Ŀ�����Ŀ������¼�Ŀ�꣩��ָ���������¼��Ķ��󣬶�
	 * �������������¼��Ķ��󡣱���ĳ��ҧ��һ��ƻ������ôƻ��������������¼�
	 * ��Ŀ�꣬��ҧ�Ķ�������˾����¼�Դ��������������ҧƻ����������¼���
	 */
	virtual void SetTarget(const CRefPtr<IBase>& _target_ptr);

private:
	class CEventImpl;
	CEventImpl* const m_event_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_H__ */
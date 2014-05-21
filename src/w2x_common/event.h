/******************************************************************************
�ļ�:	event.h
����:	CEvent ����Ϊ�����¼�����Ļ��࣬���¼�����ʱ��CEvent ������Ϊ�������ݸ�
		�¼���������CEvent ��������а����¼��Ļ�����Ϣ�������¼����������ơ�������
		�����¼�Դ�Ͳ����¼���ʱ��Ŀ�ꡣ������ͨ����չ CEvent �ࣨ��������������¼�
		CMouseEvent�����¼���������Ϣ���ݸ��¼���������
����:	wu.xiongxing
����:	wxxweb@gmail.com
����:	2014-02-24
�޸�:	2014-05-21
 ******************************************************************************/

#ifndef __W2X_COMMON_EVENT_H__
#define __W2X_COMMON_EVENT_H__

#include "macros.h"
#include "base.h"
#include "ref_ptr.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class W2X_COMMON_API CEvent: public IBase
{
public:
	static const TCHAR EVENT_UNKNOW[];		///< δ֪�¼�����
	static const TCHAR TARGET_UNNAMED[];	///< Ŀ�����δ����

public:
	/**
	 * ͨ��ָ���¼��������ƣ�����һ����Ϊ�������ݸ��¼��������� CEvent ����
	 * ע�⣬���ǵ�ģ���Ľ���ϣ�ʹ�������ַ�����ָ���¼����ͣ���Щ�¼�����
	 * ������ض������߼�ģ���У���ģ�鲻�����߼���ֻ�ṩһ���¼����ơ�
	 */
	CEvent(LPCTSTR _type_name);

	virtual ~CEvent(void);

W2X_IMPLEMENT_REFCOUNTING(CEvent)

public:
	/** �����¼������ڲ���Ϣ */
	virtual void Reset(void);

	/** ��ȡ�¼��������� */
	virtual LPCTSTR GetTypeName(void) const;

	/** �����¼��������� */
	virtual void SetTypeName(LPCTSTR _type_name);

	/** �ж��Ƿ�Ϊָ�������¼� */
	virtual bool IsTypeOf(LPCTSTR _type_name) const;

	/** �ж��Ƿ�Ϊ��ͬ�¼����� */
	virtual bool IsTypeOf(const CEvent& _event) const;

	/** �ж��Ƿ�Ϊָ���¼����� */
	bool operator ==(LPCTSTR _type_name) const;

	/** �ж��Ƿ�Ϊ��ͬ�¼����� */
	bool operator ==(const CEvent& _event) const;

	/**
	 * ��ȡ�����¼���Ŀ��������ƣ�Ŀ������¼�Ŀ�꣩��ָ���������¼��Ķ��󣬶�
	 * �������������¼��Ķ��󡣱���ĳ��ҧ��һ��ƻ������ôƻ��������������¼�
	 * ��Ŀ�꣬��ҧ�Ķ�������˾����¼�Դ��������������ҧƻ����������¼���
	 */
	virtual LPCTSTR GetTargetName(void) const;

	/**
	 * @private
	 * ���ò����¼���Ŀ��������ƣ�Ŀ������¼�Ŀ�꣩��ָ���������¼��Ķ��󣬶�
	 * �������������¼��Ķ��󡣱���ĳ��ҧ��һ��ƻ������ôƻ��������������¼�
	 * ��Ŀ�꣬��ҧ�Ķ�������˾����¼�Դ��������������ҧƻ����������¼���
	 * @note �ú������¼��ַ��� CEventDispatcher ���ã��ⲿ����ʹ�ã�
	 */
	virtual void SetTargetName(LPCTSTR _name);

private:
	class CEventImpl;
	CEventImpl* const m_event_impl_ptr;
};


/** �ж��Ƿ�Ϊָ���¼����� */
inline bool operator ==(LPCTSTR _type_name, const CEvent& _event)
{
	return _event.IsTypeOf(_type_name);
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_H__ */
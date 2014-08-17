/*******************************************************************************
�ļ�:	event_dispathcer.h
����:	�� CEventDispatcher ���ڹ����¼����������¼��Ķ��ģ��Լ��ַ������¼��ķַ���
		ͨ�������û��Զ�������ܹ������¼�����򵥷����Ǽ̳л�ۺ� CEventDispatcher��
		���ǣ���������߼���ϵ��̳й�ϵ��Ե�ʣ��޷��򲻱�ֱ�Ӽ̳� CEventDispatcher��
		Ҳ����ͨ��ʵ�� IEventDispatcher �ӿڣ����� CEventDispatcher ��Ա������д
		һЩ�򵥵İ�װ���������������ӵ��ۺϵ� CEventDispatcher �С�
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2014-02-23
�޸�:	2014-08-14
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_DISPATCHER_H__
#define __W2X_COMMON_EVENT_DISPATCHER_H__


#include "base.h"
#include "event.h"
#include "event_listener.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEvent;


class W2X_COMMON_API IEventDispatcher: public IBase
{
public:
	virtual ~IEventDispatcher(void) = 0 {}

W2X_IMPLEMENT_REFCOUNTING(IEventDispatcher)

public:
	/** 
	 * ע���¼������������ڶ����ض��¼�֪ͨ��
	 * @note ��ʹ�� CEventDispatcher ����ע���¼���������������ʹ�������ܹ������¼�֪ͨ��
	 * @param _event_type ָ����Ҫ���ĵ��¼��������ơ�
	 * @param _listener �¼�������������������ָ�롣
	 * @see IEventListener
	 */
	virtual bool AddEventListener(
		LPCTSTR _event_type,
		EventListenerPtr& _listener
	) = 0;

	/** ���¼��ַ����¼������� */
	virtual bool DispatchEvent(CEvent& _event) const = 0;

	/** ����Ƿ�Ϊ�ض��¼�����ע�����¼������� */
	virtual bool HasEventListener(LPCTSTR _event_type) const = 0;

	/** �����¼�����������ָ�룬�Ƴ���ע����¼������� */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		const EventListenerPtr& _listener
		) = 0;

	/** �����¼������� ID���Ƴ���ע����¼������� */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		LPCTSTR _listener_id
		) = 0;
};


typedef CRefPtr<IEventDispatcher> EventDispatcherPtr;


class W2X_COMMON_API CEventDispatcher: public IEventDispatcher
{
public:
	/**
	 * CEventDispatcher ��ͨ����Ϊ���࣬����ζ�Ŵ�������������ʹ�ô˹��캯����
	 * ���ǣ���������ڼ̳� CEventDispatcher ������Ҫʵ�� IEventDispatcher �ӿڣ�
	 * ��ô��ʹ�ô˹��캯�����ۺ� CEventDispatcher ���ʵ����
	 * _target_name: �����¼���Ŀ��������ƣ����¼�Ŀ�ꡣ
	 */
	CEventDispatcher(LPCTSTR _target_name);

	virtual ~CEventDispatcher(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CEventDispatcher)

public:
	/** ע���¼���������������ʹ�������ܹ������¼�֪ͨ */
	virtual bool AddEventListener(
		LPCTSTR _event_type,
		EventListenerPtr& _listener
	);

	/** ���¼��ַ����¼������� */
	virtual bool DispatchEvent(CEvent& _event) const;

	/** ����Ƿ�Ϊ�ض��¼�����ע�����¼������� */
	virtual bool HasEventListener(LPCTSTR _event_type) const;

	/** �����¼�����������ָ�룬�Ƴ���ע����¼������� */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		const EventListenerPtr& _listener
		);

	/** �����¼������� ID���Ƴ���ע����¼������� */
	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		LPCTSTR _listener_id
		);

private:
	class CDispImpl;
	CDispImpl* const m_disp_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_DISPATCHER_H__ */
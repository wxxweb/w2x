/*******************************************************************************
�ļ�:	event_listener.h
����:	�����¼��������ӿ��� IEventListener��������Ҫʹ�ò�ͬ���͵Ļص������������¼�����
		��ʱ������Ҫ�̳�����ӿ��ࡣͨ����������Խ�ȫ�ֺ���/�ྲ̬��Ա���������Ա������Ϊ
		�¼��ص�������
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2014-08-13
�޸�:
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_LISTENER_H__
#define __W2X_COMMON_EVENT_LISTENER_H__

#include "macros.h"
#include "base.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)

class CEvent;
class IEventListener;
typedef CRefPtr<IEventListener> EventListenerPtr;

class W2X_COMMON_API IEventListener: public IBase
{
public:
	typedef void (CALLBACK *Function)(PVOID, const CEvent&);
	typedef void (CALLBACK IBase::*Selector)(const CEvent&);
	
public:
	virtual ~IEventListener(void) = 0;

W2X_IMPLEMENT_REFCOUNTING(IEventListener)

public:
	/** ʹ��ȫ�ֺ���/�ྲ̬���������¼������� */
	static EventListenerPtr Create(
		LPCTSTR _listener_id, 
		PVOID _target,
		Function _function
		);

	/** ʹ�����Ա���������¼������� */
	static EventListenerPtr Create(
		LPCTSTR _listener_id, 
		IBase* _target,
		Selector _selector
		);

public:
	/** ����¼��������Ƿ���Ч */
	virtual bool IsValid(void) const = 0;

	/** ���û��߽����¼��������������¼�������Ϊ����״̬ʱ���ܽ��յ��¼�֪ͨ��*/
    virtual void SetEnabled(bool _enabled) = 0;

    /** ����¼��������Ƿ����� */
    virtual bool IsEnabled(void) const = 0;

	 /** ��Ǹ��¼��������Ѿ����¼��ַ���ע��� */
	virtual void SetRegistered(bool _registered);

	/** �����¼��������Ƿ��¼��ַ���ע��� */
	virtual bool IsRegistered() const = 0;

	/** ��ȡ�¼������� ID����ͨ����� ID �������¼������� */
    virtual LPCTSTR GetListenerId(void) const = 0;

	/** ִ���¼��������¼��ص����� */
	virtual void Execute(const CEvent& _event) = 0;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_LISTENER_H__ */
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
#include <functional>


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)

class CEvent;
class IEventListener;
typedef CRefPtr<IEventListener> EventListenerPtr;

class W2X_COMMON_API IEventListener: public IBase
{
public:
	typedef std::function<void(CEvent&)> Callback;
	
public:
	virtual ~IEventListener(void) = 0;

W2X_IMPLEMENT_REFCOUNTING(IEventListener)

public:
	/** 
	 * �����¼���������
	 * @param _listener_id �¼�������Ψһ��ʶ��
	 * @param _callback �ຯ�����������¼��ص�����ͨ���� W2X_CALLBACK_1 ��������
	 * @see Callback
	 */
	static EventListenerPtr Create(
		LPCTSTR _listener_id,
		const Callback& _callback
		);

public:
	/** ����¼��������Ƿ���Ч */
	virtual bool IsValid(void) const = 0;

	/** ���û��߽����¼��������������¼�������Ϊ����״̬ʱ���ܽ��յ��¼�֪ͨ��*/
    virtual void SetEnabled(bool _enabled) = 0;

    /** ����¼��������Ƿ����� */
    virtual bool IsEnabled(void) const = 0;

	 /** ��Ǹ��¼��������Ѿ����¼��ַ���ע��� */
	virtual void SetRegistered(bool _registered) = 0;

	/** �����¼��������Ƿ��¼��ַ���ע��� */
	virtual bool IsRegistered() const = 0;

	/** ��ȡ�¼������� ID����ͨ����� ID �������¼������� */
    virtual LPCTSTR GetListenerId(void) const = 0;

	/** ִ���¼��������¼��ص����� */
	virtual void Execute(CEvent& _event) = 0;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_LISTENER_H__ */
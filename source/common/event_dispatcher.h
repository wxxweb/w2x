/******************************************************************************
 * �ļ���	event_dispathcer.h
 * ������	IEventDispatcher �ӿڶ���������ӻ�ɾ���¼��������ķ���������Ƿ���ע���ض�
 *          ���͵��¼����������������¼���
 * 			ͨ�������û��Զ�������ܹ������¼�����򵥷����Ǽ̳л�ۺ� CEventDispatcher��
 * 			���ǣ���������߼���ϵ��̳й�ϵ��Ե�ʣ��޷��򲻱�ֱ�Ӽ̳� CEventDispatcher��
 * 			Ҳ����ͨ��ʵ�� IEventDispatcher �ӿڣ����� CEventDispatcher ��Ա������д
 * 			һЩ�򵥵İ�װ���������������ӵ��ۺϵ� CEventDispatcher �С�
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_EVENT_DISPATCHER_H__
#define __W2X_COMMON_EVENT_DISPATCHER_H__

#include "exports.h"
#include "macros.h"
#include "base.h"
#include "ref_ptr.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEvent;

/*
 * �¼��������������͡�
 */
typedef void (*FEventListener)(const CEvent& _event_ref);

class W2X_COMMON_API IEventDispatcher: public IBase
{
public:
	virtual ~IEventDispatcher(void) = 0 {}

W2X_IMPLEMENT_REFCOUNTING(IEventDispatcher)

public:
	/*
	 * ��ʹ�� CEventDispatcher ����ע���¼���������������ʹ�������ܹ������¼�֪ͨ��
	 */
	virtual bool AddEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener
	) = 0;

	/*
	 * ���¼��ַ����¼���������
	 */
	virtual bool DispatchEvent(const CEvent& _event_ref) const = 0;

	/*
	 * ��� CEventDispatcher �����Ƿ�Ϊ�ض��¼�����ע�����¼���������
	 */
	virtual bool HasEventListener(LPCTSTR _type_name) const = 0;

	/*
	 * �� CEventDispatcher ������ɾ���¼���������
	 */
	virtual bool RemoveEventListener(
		LPCTSTR _type_name, 
		FEventListener _listener
	) = 0;
};


class W2X_COMMON_API CEventDispatcher: public IEventDispatcher
{
public:
	/*
	 * CEventDispatcher ��ͨ����Ϊ���࣬����ζ�Ŵ�������������ʹ�ô˹��캯����
	 * ���ǣ���������ڼ̳� CEventDispatcher ������Ҫʵ�� IEventDispatcher �ӿڣ�
	 * ��ô��ʹ�ô˹��캯�����ۺ� CEventDispatcher ���ʵ����
	 * _target_ptr: �����¼���Ŀ�����ָ�룬���¼�Ŀ�ꡣ
	 */
	CEventDispatcher(const CRefPtr<IBase>& _target_ptr);

	virtual ~CEventDispatcher(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CEventDispatcher)

public:
	/*
	 * ע���¼���������������ʹ�������ܹ������¼�֪ͨ��
	 */
	virtual bool AddEventListener(LPCTSTR _type_name, FEventListener _listener);

	/*
	 * ���¼��ַ����¼���������
	 */
	virtual bool DispatchEvent(const CEvent& _event_ref) const;

	/*
	 * ����Ƿ�Ϊ�ض��¼�����ע�����¼���������
	 */
	virtual bool HasEventListener(LPCTSTR _type_name) const;

	/*
	 * ɾ���¼���������
	 */
	virtual bool RemoveEventListener(LPCTSTR _type_name, FEventListener _listener);

private:
	class CDispImpl;
	CDispImpl* const m_disp_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_EVENT_DISPATCHER_H__ */
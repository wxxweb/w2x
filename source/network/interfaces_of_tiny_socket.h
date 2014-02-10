/*******************************************************************************
 * �ļ���	interfacess_of_tiny_socket.h
 * ������	���� CTinySocket �ļ����ӿ��࣬����Ϣ�ӿڡ���Ϣ�ַ����ӿڡ���Ϣ�������ӿڡ�
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-02-08
 ******************************************************************************/

#ifndef __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__
#define __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__

#include "exports.h"
#include "..\common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class ITinySocketListener;

/*
 * CTinySocket ��Ϣ�ַ����ӿ��� ITinySocketDispatcher��������������󱻹���ʱ��
 * �����Զ��� CTinySocket ����ע�ᣨ���ú��� RegisterDispatcher()��������������
 * ʱ�������Զ�����ע�������ú��� UnregisterDispatcher()����CTinySocket �����
 * �յ���Ϣ�󣬻����Ϣ�ŵ������У����ŵ��÷ַ����ĺ��� DispatchReceviedMessage()
 * ��ԭʼ�Ķ�������Ϣ���ݸ��ַ������ַ����轫ԭʼ�Ķ�������Ϣ��װ�� ITinySocketMessage
 * �������Ȼ���ٷַ�����Ӧ����Ϣ����������Ȼ����������Ҫ���յ���Ϣ֪ͨ��ǰ���ǣ�
 * �ȵ��÷ַ����ĺ��� RegisterListener() ����ע�ᡣ����������ʱ����ͨ�����ú��� 
 * UnregisterListener() ע����Ϣ֪ͨ��
 */
class W2X_NETWORK_API ITinySocketDispatcher
{
public:
	ITinySocketDispatcher(void);
	virtual ~ITinySocketDispatcher(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketDispatcher)

public:
	/*
	 * �����յ�����Ϣ������װ��ַ�������Ϣ��������
	 * _msg_ptr ָ��һ��ԭʼ�Ķ�������Ϣ���ݻ�������
	 * _msg_bytes Ϊԭʼ��������Ϣ���ֽ�����
	 */
	virtual void DispatchReceviedMessage(
		const BYTE* _msg_ptr, 
		size_t _msg_bytes
	) = 0;

	/* 
	 * ע����Ϣ��������ֻ��ע�������������ܽ��յ���Ϣ֪ͨ��
	 */
	virtual bool RegisterListener(
		ITinySocketListener* _listener_ptr
	) = 0;

	/* 
	 * ע����Ϣ�����������ٽ�����Ϣ֪ͨ������ؽ�����ʹ�õ�������ע������
	 */
	virtual bool UnregisterListener(
		ITinySocketListener* _listener_ptr
	) = 0;
};

/*
 * Socket ��Ϣ��װ�ӿ��� ITinySocketMessage����������Ϣ��Э��ṹ��
 */
class W2X_NETWORK_API ITinySocketMessage
{
public:
	enum {
		MAX_MSG_DATA = 1024,	// ��Ϣ��������ֽ���
		INVALID_MSG_ID = 0,		// ��Ч����Ϣ ID
	};

public:
	ITinySocketMessage(void);
	virtual ~ITinySocketMessage(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketMessage)

public:
	virtual void Handle(const BYTE* _msg_data) = 0;

	/*
	 * ������Ϣ���ݽṹ�������ɹ����� true�����򷵻� false��
	 */
	virtual bool Create(
		WORD _msg_id, 
		const BYTE* _msg_data_ptr, 
		WORD _data_bytes
	);

	/*
	 * ��Ϣ���ݽṹ��Ч���� true�����򷵻� false��
	 */
	virtual bool IsValid(void) const;

	/*
	 * ��Զ���������� UDP ��Ϣ��
	 * _is_broadcast ��ʾ�Ƿ��͹㲥��Ϣ�����Ҫ���͹㲥��Ϣ���� _host �� _port 
	 * ʹ��Ĭ��ֵ���� _host ֵΪ NULL��_port ֵΪ 0����������͹㲥��Ϣ���� _host
	 * �� _port ��Ҫ���ã�_host ��Ϊ IP ��ַ�ַ������� "192.168.1.121"��
	 * Ҳ����Ϊ���������������� "www.baidu.com"��_port ΪԶ�������˿ںš�
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	virtual bool SendUdp(
		bool _is_broadcast = false,
		LPCTSTR _host = NULL,
		WORD _port = 0
	) const;
	
	
	WORD GetId(void) const;
	WORD GetSize(void) const;
	const BYTE* GetData(void) const;
	bool GetData(OUT PBYTE _data_bufer_ptr, OUT size_t& _data_bytes) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

/*
 * CTinySocket ��Ϣ�������ӿ��� ITinySocketListener������������������ CTinySocket
 * ��Ϣ�ַ�����ITinySocketDispatcher ������󣩽���ע�ᣨͨ��������Ϣ�ַ����ĺ���
 * RegisterListener()����ֻ��ע�����ܽ��յ���Ӧ����Ϣ֪ͨ������������������ʱ���Զ�ע��
 * ��Ϣ֪ͨ��ͨ��������Ϣ�ַ����ĺ��� UnregisterListener()������Ϣ�ַ�������װ�����Ϣ
 * ת�ݸ����������� HandleReceivedMessage()�����ڴ�����յ�����Ϣ��
 */
class W2X_NETWORK_API ITinySocketListener
{
public:
	ITinySocketListener(ITinySocketDispatcher* _dispatcher_ptr);
	virtual ~ITinySocketListener(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketListener)

public:
	/*
	 * ���ڹ�����Ϣ���ж϶�Ӧ ID ����Ϣ�Ƿ���Ҫ���д���
	 */
	virtual bool IsMessageNeedToHandle(UINT _msg_id) = 0;

	/*
	 * ������յ�����Ϣ��
	 */
	virtual void HandleReceivedMessage(const ITinySocketMessage* _msg_ptr) = 0;

private:
	ITinySocketDispatcher* const m_tiny_socket_dispatcher_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__ */
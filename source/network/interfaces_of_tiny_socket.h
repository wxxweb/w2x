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


struct MsgHeader {
	WORD check_sum;
	WORD version;
	WORD msg_id;
	WORD msg_data_bytes;
};


/*
 * CTinySocket ��Ϣ�������ӿ��� IMessageListener��������������󱻹���ʱ�������Զ���
 * CTinySocket ����ע�ᣨ���ú��� CTinySocket::RegisterListener()��������������ʱ����
 * ���Զ�����ע�������ú��� CTinySocket::UnregisterListener()����CTinySocket ����
 * ���յ���Ϣ�󣬻����Ϣ�ŵ������У����Ӻ����װ�� ITinySocketMessage ������󣬽��ŵ�
 * ������������ IsMessageNeedToHandle() ������Ϣ�Ƿ���Ҫ�������账���������������
 * �� HandleReceivedMessage() ���ݸ���������
 */
class W2X_NETWORK_API IMessageListener
{
public:
	IMessageListener(void);
	virtual ~IMessageListener(void);

W2X_DISALLOW_COPY_AND_ASSIGN(IMessageListener)

public:
	/*
	 * ���ڹ�����Ϣ���ж϶�Ӧ ID ����Ϣ�Ƿ���Ҫ���д���
	 */
	virtual bool IsMessageNeedToHandle(UINT _msg_id) = 0;

	/*
	 * ������յ�����Ϣ��
	 */
	virtual void HandleMessage(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header_ref,
		const BYTE* _msg_data_ptr,
		WORD _data_bytes
	) = 0;
};

/*
 * CTinySocket ��Ϣ��װ�ӿ��� ITinySocketMessage���������������Դ�����������Ϣ��
 * Ҳ���Խ��ղ������Ӧ��Ϣ�����͵���Ϣ��һ����Ϣ ID����Ӧ����ϢҲ��һ����Ϣ ID��������
 * ID ֵ��ͬ����Ӧ��Ϣ������ HandleEchoMsg() ֻ����ָ�� ID �Ļ�Ӧ��Ϣ��������Ϣ��
 * ��Ӧ��Ϣ����Ϊһ��ͨ����Ϣ��ÿ��ͨ�Ŷ���һ���汾�ţ�����ͬ�Ŀͻ��˰汾��ͬʱ�����յ�
 * һ����Ϣ�ɾ��������ڻ�Ӧ��
 */
class W2X_NETWORK_API ITinySocketMessage
{
public:
	enum {
		MAX_MSG_DATA = 1024,	// ��Ϣ��������ֽ���
		INVALID_MSG_ID = 0,		// ��Ч����Ϣ ID
	};

	struct MsgInfo {
		MsgHeader msg_header;
		BYTE msg_data[ITinySocketMessage::MAX_MSG_DATA];
	};

public:
	ITinySocketMessage(
		WORD _send_msg_id,	// ������Ϣ�� ID
		WORD _echo_msg_id,	// ��Ӧ��Ϣ�� ID
		WORD _version		// һ����Ϣͨ�ŵİ汾��
	);

	virtual ~ITinySocketMessage(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketMessage)

public:
	/*
	 * һ������£�������һ����Ϣ��ȥ�󣬽�����յ�һ����Ӧ��Ϣ��
	 * CTinySocket ��Ϣѭ��������øú����������Ӧ��Ϣ��
	 */
	virtual void HandleEchoMsg(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		WORD _data_bytes
	) = 0;

	/*
	 * ����һ����Ҫ���͵���Ϣ���ݽṹ�������ɹ����� true�����򷵻� false��
	 */
	bool CreateSendMsg(
		const BYTE* _msg_data_ptr, 
		WORD _data_bytes
	);

	/*
	 * ������Ϣ���ݽṹ��Ч���� true�����򷵻� false��
	 */
	bool IsValid(void) const;

	/*
	 * ��Զ������������Ϣ��
	 * _remote_addr_str ��Ϊ IP ��ַ�ַ������� "192.168.1.121"�� Ҳ����
	 * Ϊ���������������� "www.baidu.com"��_reomte_port ΪԶ�������˿ںš�
	 * ��� _remote_addr_str ֵΪ NULL ���;������㲥��Ϣ��
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	int SendMsg(
		LPCTSTR _remote_addr_str,
		WORD _reomte_port,
		bool _is_udp
	) const;

	WORD GetSendId(void) const;
	WORD GetEchoId(void) const;
	WORD GetSize(void) const;
	const BYTE* GetData(void) const;
	bool GetData(OUT PBYTE _data_bufer_ptr, OUT size_t& _data_bytes) const;

	static bool InitializeUdp(WORD _local_port);

	/* 
	 * ע����Ϣ�����������յ�������ָ������Ϣʱ���ᴫ�ݸ�����������
	 */
	static bool RegisterListener(IMessageListener* _listener_ptr);

	/* 
	 * ע����Ϣ�����������ٽ�����Ϣ֪ͨ��
	 */
	static bool UnregisterListener(IMessageListener* _listener_ptr);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__ */
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
	virtual void Handle(DWORD _remote_ip_addr, const BYTE* _msg_data) = 0;

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
	 * ��Զ������������Ϣ��
	 * _remote_addr_str ��Ϊ IP ��ַ�ַ������� "192.168.1.121"�� Ҳ����
	 * Ϊ���������������� "www.baidu.com"��_reomte_port ΪԶ�������˿ںš�
	 * ��� _remote_addr_str ֵΪ NULL ���;������㲥��Ϣ��
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	virtual int Send(
		LPCTSTR _remote_addr_str,
		WORD _reomte_port
	) const;
	
	
	WORD GetId(void) const;
	WORD GetSize(void) const;
	const BYTE* GetData(void) const;
	bool GetData(OUT PBYTE _data_bufer_ptr, OUT size_t& _data_bytes) const;

	static bool InitializeUdp(WORD _local_port);

	static int Send(
		LPCTSTR _remote_addr_str,
		WORD _remote_port,
		const BYTE* _packet_buffer,
		DWORD _size_in_bytes);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

/*
 * CTinySocket ��Ϣ�������ӿ��� ITinySocketListener��������������󱻹���ʱ�������Զ���
 * CTinySocket ����ע�ᣨ���ú��� CTinySocket::RegisterListener()��������������ʱ����
 * ���Զ�����ע�������ú��� CTinySocket::UnregisterListener()����CTinySocket ����
 * ���յ���Ϣ�󣬻����Ϣ�ŵ������У����Ӻ����װ�� ITinySocketMessage ������󣬽��ŵ�
 * ������������ IsMessageNeedToHandle() ������Ϣ�Ƿ���Ҫ�������账���������������
 * �� HandleReceivedMessage() ���ݸ���������
 */
class W2X_NETWORK_API ITinySocketListener
{
public:
	ITinySocketListener(void);
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
	virtual void HandleReceivedMessage(
		DWORD _remote_ip_addr,
		UINT _msg_id, 
		const BYTE* _msg_data, 
		WORD _data_bytes
	) = 0;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__ */
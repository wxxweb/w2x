/*******************************************************************************
 * �ļ���	tiny_socket.h
 * ������	�� socket ���õ�һ��СС�ķ�װ��
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-04-05
 ******************************************************************************/

#ifndef __W2X_NETWORK_TINY_SOCKET_H__
#define __W2X_NETWORK_TINY_SOCKET_H__

#include "exports.h"
#include "../common/macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)

/*
 * �������ӿ��࣬������һ�������������ӿڣ��� CTinySocket �����첽������Ϣ��
 * �����Ϣ�ŵ���Ϣ�����У����Ŵ������������첽�ص������ڴ�����յ�����Ϣ��
 * �� ITinySocketListener ���������ʱ�������Զ�ע���������������Զ�ע����
 */
class W2X_NETWORK_API ITinySocketListener
{
public:
	ITinySocketListener(void);
	virtual ~ITinySocketListener(void);

public:
	virtual void HandleReceivedMessage(const BYTE* _msg_ptr, size_t _msg_bytes) = 0;
};

class W2X_NETWORK_API CTinySocket
{
	friend class W2X_NETWORK_API ITinySocketListener;

public:
	enum {
		MAX_MSG_BUF_SIZE = 0x2000,	// ���ݰ���Ϣ�����С
	};
	/*
	 * �첽�������ݵķ���״̬
	 */
	enum ERecvStatus {
		kAsyncRecvError		= -1,	// ���չ����г���
		kAsyncRecvPending	= 0,	// ���ڽ����У�δ���
		kAsyncRecvComplete	= 1,	// �����������
	};

public:
	CTinySocket(void);
	virtual ~CTinySocket(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CTinySocket)

public:
	/* 
	 * ���̵��øú�����ʼ�� WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 */
	static bool Initialize(void);

	/* 
	 * ���̵��øú����ͷ�WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 */
	static bool Uninitialize(void);

	/* 
	 * ���������շ� UDP ���ݰ��� Socket���������� IP ��ַ�󶨵� Socket��
	 * _local_port Ϊ���ض˿ںţ����ֵΪ 0 ���Զ����� 1024-5000 ��˿ںš�
	 * _is_enable_broadcast ���������Ƿ��͹㲥��Ϣ��Ĭ��Ϊ false ������
	 * ���ɹ��򷵻� true, �����ͷŴ��������е���Դ������ false��
	 */
	bool CreateUdp(WORD _local_port = 0, bool _is_broadcast = false);

	/*
	 * �����Ѵ����� Socket ��Դ�� ���ɹ��򷵻� true, ���򷵻� false��
	 */
	bool Destory(void);

	/* 
	 * ͬ������ UDP ���ݰ���
	 * _remote_addr_ptr ���Զ��������ַ�� 
	 * _packet_buffer ��Ž��յ������ݣ�
	 * _size_in_bytes ���ݻ������ֽ�����
	 * �����յ����ݰ����򷵻������ֽ����������չ����г����򷵻� SOCKET_ERROR(-1)��
	 */
	int SyncRecvUdpPacket(
		OUT	PSOCKADDR_IN _remote_addr_ptr,
		OUT	PBYTE _packet_buffer,
		const DWORD _size_in_bytes
	);

	/*
	 * ���� UDP ���ݰ���
	 * _remote_addr_ptr Զ��������ַ��
	 * _packet_buffer Ҫ�������ݵĻ�������
	 * _size_in_bytes Ҫ���͵��ֽ�����
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	int SendUdpPacket(
		const PSOCKADDR_IN _remote_addr_ptr,
		const PBYTE _packet_buffer,
		const DWORD _size_in_bytes
	);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_TINY_COMMON_H__ */
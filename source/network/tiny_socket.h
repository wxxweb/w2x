/*******************************************************************************
 * �ļ���	tiny_socket.h
 * ������	�������ķ�װ�࣬ʵ�ֶ��ص��첽 socket �ķ�װ��
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-02-05
 ******************************************************************************/

#ifndef __W2X_NETWORK_TINY_SOCKET_H__
#define __W2X_NETWORK_TINY_SOCKET_H__

#include "exports.h"
#include "../common/macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class ITinySocketDispatcher;


class W2X_NETWORK_API CTinySocket
{
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

	static bool RegisterDispatcher(ITinySocketDispatcher* _dispatcher_ptr);

	static bool UnregisterDispatcher(ITinySocketDispatcher* _dispatcher_ptr);

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
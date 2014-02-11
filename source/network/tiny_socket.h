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


class ITinySocketListener;


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

	typedef bool (CALLBACK *FPacketParser)(
		ITinySocketListener* _listener_ptr,
		const BYTE* _packet_ptr,
		size_t _packet_bytes,
		DWORD _remote_ip_addr
	);

public:
	CTinySocket(void);
	virtual ~CTinySocket(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CTinySocket)

public:
	/* 
	 * ���̵��øú�����ʼ�� WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 * ֻ������һ�Σ������ظ����ò�����Ӱ�죬�ڲ��������������ظ���ʼ����
	 */
	static bool Initialize(void);

	/* 
	 * ���̵��øú����ͷ�WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 */
	static bool Uninitialize(void);

	static bool SetPacketParser(FPacketParser _packet_parser_fn_ptr);

	static bool RegisterListener(ITinySocketListener* _listener_ptr);

	static bool UnregisterListener(ITinySocketListener* _listener_ptr);

	/* 
	 * ���������շ� UDP ���ݰ��� Socket���������� IP ��ַ�󶨵� Socket��
	 * _local_port Ϊ���ض˿ںţ����ֵΪ 0 ���Զ����� 1024-5000 ��˿ںš�
	 * ���ɹ��򷵻� true, �����ͷŴ��������е���Դ������ false��
	 */
	bool CreateUdp(WORD _local_port = 0);

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
	 * �첽���� UDP ���ݰ�������ֵ�� ERecvStatus��
	 */
	ERecvStatus RecvUdpPacket(void);

	/*
	 * ���� UDP ���ݰ���
	 * _remote_addr_str Զ��������ַ�������� IP ��ַ���� "192.168.1.121"��Ҳ������
	 * �������� "www.baidu.com"����ֵΪ�գ�NULL���Ƿ��;������㲥��Ϣ��
	 * _port Զ�������˿ںţ�
	 * _packet_buffer Ҫ�������ݵĻ�������
	 * _size_in_bytes Ҫ���͵��ֽ�����
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	int SendUdpPacket(
		LPCTSTR _remote_addr_str,
		WORD _port,
		const BYTE* _packet_buffer,
		DWORD _size_in_bytes
	);

	/*
	 * ���û���� Socket ���͹㲥���ݰ����ܡ�
	 * _is_enable Ϊ true ��ʾ���ã�Ϊ false ��ʾ���á�
	 * ���ɹ����� true, ���򷵻� false��
	 */
	bool EnableBroadcast(bool _is_enable);

	/*
	 * ��� Socket ���͹㲥���ݰ������Ƿ�����
	 * ���� true ��ʾ���ã�false ��ʾ���á�
	 */
	bool IsBroadcastEnable(void) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_TINY_COMMON_H__ */
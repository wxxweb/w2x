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
		/*
		 * ��̫����Ethernet���� MTU������䵥Ԫ��Ϊ 1500 �ֽڣ�
		 * IP ���ݱ����ײ�Ϊ 20 �ֽڣ�UDP���ݱ����ײ� 8 �ֽڣ�
		 * ���� UDP ���ݱ�����������󳤶�Ϊ 1472 �ֽڡ�
		 * Internet�ϵı�׼ MTU ֵΪ 576 �ֽ�
		 * ��ʱ UDP �����ݱ�����󳤶ȳ���Ϊ 548 �ֽڡ�
		 */
		MAX_UDP_MSG_SIZE = 512,
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
	static bool InitWinSock(void);

	/* 
	 * ���̵��øú����ͷ� WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 */
	static bool UninitWinSock(void);

	/* 
	 * �������ݰ������������ݰ����������ڽ���Ϣ���ݷֲ������
	 */
	static bool SetPacketParser(FPacketParser _packet_parser_fn_ptr);

	/* 
	 * ע����Ϣ�����������յ�������ָ������Ϣʱ���ᴫ�ݸ�����������
	 */
	static bool RegisterListener(ITinySocketListener* _listener_ptr);

	/* 
	 * ע����Ϣ�����������ٽ�����Ϣ֪ͨ��
	 */
	static bool UnregisterListener(ITinySocketListener* _listener_ptr);

	/* 
	 * ���������շ����ݰ��� Socket���������� IP ��ַ�󶨵� Socket��
	 * _local_port Ϊ���ض˿ںţ����ֵΪ 0 ���Զ����� 1024-5000 ��˿ںš�
	 * _is_udp ����ָ�� Socket �Ĵ��䷽ʽ�Ƿ�Ϊ UDP��Ĭ�� false Ϊ TCP��
	 * �������ɹ��򷵻� true, �����ͷŴ��������е���Դ������ false��
	 */
	bool Create(WORD _local_port = 0, bool _is_udp = false);

	/* 
	 * ��� Socket �Ƿ񴴽��ɹ�����Ч���ã���Ч���� true�����򷵻� false��
	 */
	bool IsValid(void) const;

	/*
	 * �����Ѵ����� Socket ��Դ�� ���ɹ��򷵻� true, ���򷵻� false��
	 */
	bool Destory(void);

	/* 
	 * �첽�������ݰ�������ֵ�� ERecvStatus��
	 */
	ERecvStatus RecvPacket(void);

	/*
	 * �������ݰ���
	 * _remote_addr_str Զ��������ַ�������� IP ��ַ���� "192.168.1.121"��Ҳ������
	 * �������� "www.baidu.com"����ֵΪ�գ�NULL���Ƿ��;������㲥��Ϣ��
	 * _port Զ�������˿ںţ�
	 * _packet_buffer Ҫ�������ݵĻ�������
	 * _size_in_bytes Ҫ���͵��ֽ�����
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	int SendPacket(
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
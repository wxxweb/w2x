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
#include "w2x_common/macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)


class W2X_NETWORK_API CTinySocket
{
public:
	enum {
		/**
		 * ��̫����Ethernet���� MTU������䵥Ԫ��Ϊ 1500 �ֽڣ�
		 * IP ���ݱ����ײ�Ϊ 20 �ֽڣ�UDP���ݱ����ײ� 8 �ֽڣ�
		 * ���� UDP ���ݱ�����������󳤶�Ϊ 1472 �ֽڡ�
		 * Internet�ϵı�׼ MTU ֵΪ 576 �ֽ�
		 * ��ʱ UDP �����ݱ�����󳤶ȳ���Ϊ 548 �ֽڡ�
		 */
		MAX_UDP_MSG_SIZE = 512,
	};

	typedef bool (CALLBACK *FnPacketDispatcher)(
		DWORD _remote_ip_addr,	// �����ֽ���
		WORD _remote_port,		// �����ֽ���
		const BYTE* _packet_data_ptr,
		WORD _data_bytes
	); 

public:
	CTinySocket(void);
	virtual ~CTinySocket(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CTinySocket)

public:
	/**
	 * ���������շ����ݰ��� Socket���������� IP ��ַ�󶨵� Socket��
	 * _local_port Ϊ���ض˿ںţ����ֵΪ 0 ���Զ����� 1024-5000 ��˿ںš�
	 * _is_udp ����ָ�� Socket �Ĵ��䷽ʽ�Ƿ�Ϊ UDP��Ĭ�� false Ϊ TCP��
	 * �������ɹ��򷵻� true, �����ͷŴ��������е���Դ������ false��
	 */
	bool CreateUdp(
		WORD _local_port = 0, 
		LPCTSTR _mcast_addr_str = NULL, 
		WORD _mcast_port = 0
	);

	/** ��� Socket �Ƿ񴴽��ɹ�����Ч���ã���Ч���� true�����򷵻� false��*/
	bool IsValid(void) const;

	/** �����Ѵ����� Socket ��Դ�� ���ɹ��򷵻� true, ���򷵻� false��*/
	void Destroy(void);

	/** ����һ���ಥ�� */
	bool JoinMulticast();

	/**
	 * �������ݰ���
	 * _remote_addr_str Զ��������ַ�������� IP ��ַ���� "192.168.1.121"��Ҳ������
	 * �������� "www.baidu.com"����ֵΪ�գ�NULL���Ƿ��;������㲥��Ϣ��
	 * _port Զ�������˿ںţ�
	 * _packet_buffer Ҫ�������ݵĻ�������
	 * _size_in_bytes Ҫ���͵��ֽ�����
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	bool SendPacket(
		LPCTSTR _remote_addr_str,
		WORD _remote_port,
		const BYTE* _packet_buffer,
		DWORD _size_in_bytes
	);

	bool SetRecvDispatcher(FnPacketDispatcher _dispatcher);

	bool SetSendDispatcher(FnPacketDispatcher _dispatcher);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_TINY_COMMON_H__ */
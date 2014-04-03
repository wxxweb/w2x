/*******************************************************************************
 * �ļ���	tiny_socket_msg.h
 * ������	���� CTinySocket ��Ϣ����ļ����ӿ��࣬����Ϣ����ӿڡ���Ϣ�������ӿڡ�
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-02-08
 ******************************************************************************/

#ifndef __W2X_NETWORK_tiny_socket_msg_H__
#define __W2X_NETWORK_tiny_socket_msg_H__

#include "exports.h"
#include "w2x_common/base.h"
#include "w2x_common/macros.h"
#include "w2x_common/encode.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)



struct MsgHeader {
	WORD	auth_code;		// 2�ֽ�, ��֤��
	BYTE	version;		// 1�ֽ�, Э��\��Ϣ�汾��
	BYTE	header_bytes;	// 1�ֽ�, Э��ͷ���ֽ���
	WORD	msg_id;			// 2�ֽ�, ��ϢID
	WORD	check_sum;		// 2�ֽ�, CRC16У���
	DWORD	data_bytes;		// 4�ֽ�, ��Ϣ�����ֽ���
	DWORD	reserve;		// 4�ֽ�, �������ڶ��룬��������ʹ��
};


/*
 * ITinySocketMsg ����Ϣ�������ӿ��� IMsgListener��������������󱻹���ʱ��
 * �����Զ���ӵ� ITinySocketMsg �������������У����ú��� RegisterListener()����
 * ����������ʱ�������������������ɾ�������ú��� UnregisterListener()���������յ���Ϣ
 * ������������ IsMsgNeedToHandle() �ᱻ���ã����ڼ�����Ϣ�Ƿ���Ҫ��������
 * ������������������� HandleMsg() ����Ϣ���ݸ����������д���
 */
class W2X_NETWORK_API IMsgListener
{
public:
	IMsgListener(void);
	virtual ~IMsgListener(void);

W2X_DISALLOW_COPY_AND_ASSIGN(IMsgListener)

public:
	/*
	 * ���ڹ�����Ϣ���ж϶�Ӧ ID ����Ϣ�Ƿ���Ҫ���д���
	 */
	virtual bool IsMsgNeedToHandle(UINT _msg_id) = 0;

	/*
	 * ������յ�����Ϣ��
	 */
	virtual void HandleRecvMsg(
		DWORD _remote_ip_addr,
		WORD _remote_port,
		const MsgHeader& _msg_header_ref,
		const BYTE* _msg_data_ptr,
		DWORD _data_bytes
	) = 0;

	virtual void HandleSendMsg(
		DWORD _remote_ip_addr,
		WORD _remote_port,
		const MsgHeader& _msg_header_ref,
		const BYTE* _msg_data_ptr,
		DWORD _data_bytes
		) = 0;
};

/*
 * CTinySocket ��Ϣ��װ�ӿ��� ITinySocketMsg���������������Դ�����������Ϣ��
 * Ҳ���Խ��ղ������Ӧ��Ϣ�����͵���Ϣ��һ����Ϣ ID����Ӧ����ϢҲ��һ����Ϣ ID��������
 * ID ֵ��ͬ����Ӧ��Ϣ������ HandleRecvMsg() ֻ����ָ�� ID �Ļ�Ӧ��Ϣ��������Ϣ��
 * ��Ӧ��Ϣ����Ϊһ��ͨ����Ϣ��ÿ��ͨ�Ŷ���һ���汾�ţ�����ͬ�Ŀͻ��˰汾��ͬʱ�����յ�
 * һ����Ϣ�ɾ��������ڻ�Ӧ��
 */
class W2X_NETWORK_API ITinySocketMsg: public IBase
{
public:
	enum {
		MAX_MSG_DATA = 1024,	// ��Ϣ��������ֽ���
		INVALID_MSG_ID = 0,		// ��Ч����Ϣ ID
	};

	struct MsgInfo {
		MsgHeader msg_header;
		BYTE msg_data[ITinySocketMsg::MAX_MSG_DATA];
	};

public:
	ITinySocketMsg(
		WORD _send_msg_id,	// ���ڷ��͵���Ϣ ID��Ϊ 0 ��ʾ���ɷ���
		WORD _recv_msg_id,	// �ڴ����յ���Ϣ ID�����յ���Ϣ�� HandleRecvMsg() ������
		BYTE _version = 1	// һ����Ϣͨ�ŵİ汾��
	);

	virtual ~ITinySocketMsg(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketMsg)
W2X_IMPLEMENT_REFCOUNTING(ITinySocketMsg)

public:
	/**
	 * һ������£�������һ����Ϣ��ȥ�󣬽�����յ�һ����Ӧ��Ϣ��
	 * CTinySocket ��Ϣѭ��������øú�����������յ�����Ϣ��
	 */
	virtual void HandleRecvMsg(
		LPCTSTR _remote_ip_addr,
		WORD _remote_port,	// �����ֽ���
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		DWORD _data_bytes
	) = 0;

	/**
	 * ������һ����Ϣʱ����δ�������ͣ����ǽ��뷢�Ͷ��У��ڷ���ǰ��
	 * CTinySocket ��Ϣѭ��������øú������Ա��ڶ��δ�������Ϣ��
	 */
	virtual void HandleSendMsg(
		LPCTSTR _remote_ip_addr,
		WORD _remote_port,	// �����ֽ���
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		DWORD _data_bytes
	) = 0;

	/**
	 * ����һ����Ҫ���͵���Ϣ���ݽṹ�������ɹ����� true�����򷵻� false��
	 */
	bool CreateMsg(
		const BYTE* _msg_data_ptr = NULL,
		DWORD _data_bytes = 0,
		DWORD _fill_bytes = 0,
		BYTE _filler = 0x00
	);

	/**
	 * ����һ��UTF-8�ַ�����Ϣ���ڷ��ͣ������ɹ����� true�����򷵻� false��
	 */
	bool CreateUtf8Msg(
		LPCTSTR _t_str_ptr,
		DWORD _fill_bytes = 0,
		BYTE _filler = 0x00
	);

	/**
	 * ������Ϣ���ݽṹ��Ч���� true�����򷵻� false��
	 */
	bool IsValid(void) const;

	/**
	 * ��Զ������������Ϣ��
	 * _remote_addr_str ��Ϊ IP ��ַ�ַ������� "192.168.1.121"�� Ҳ����
	 * Ϊ���������������� "www.baidu.com"��_reomte_port ΪԶ�������˿ںš�
	 * ��� _remote_addr_str ֵΪ NULL ���;������㲥��Ϣ��
	 * ���ɹ��򷵻ط����ֽ�������ʧ���򲢷��� SOCKET_ERROR(-1)��
	 */
	bool SendMsg(
		LPCTSTR _remote_addr_str,
		WORD _reomte_port,	// �����ֽ���
		bool _is_udp
	) const;

	WORD GetSendId(void) const;

	WORD GetRecvId(void) const;

	DWORD GetDataSize(void) const;

	const BYTE* GetData(void) const;

	bool CopyData(OUT PBYTE _data_bufer_ptr, OUT size_t& _data_bytes) const;

	static bool InitUdp(
		WORD _local_port, 
		LPCTSTR _mcast_addr_str = NULL, 
		WORD _mcast_port = 0
	);

	/** ע����Ϣ�����������յ�������ָ������Ϣʱ���ᴫ�ݸ�����������*/
	static bool RegisterListener(IMsgListener* _listener_ptr);

	/** ���ٽ�����Ϣ֪ͨ��*/
	static bool UnregisterListener(IMsgListener* _listener_ptr);

	/** ������ϢЭ����֤�� */
	static void SetAuthCode(WORD dwCode);

	/** �Ƿ���˵��ᷢ��Ϣ */
	static void FilterOutLoopback(bool _is_filter_out);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_tiny_socket_msg_H__ */
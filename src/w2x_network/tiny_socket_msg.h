/*******************************************************************************
 * 文件：	tiny_socket_msg.h
 * 描述：	定义 CTinySocket 消息处理的几个接口类，如消息基类接口、消息监听器接口。
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-08
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
	WORD	auth_code;		// 2字节, 验证码
	BYTE	version;		// 1字节, 协议\消息版本号
	BYTE	header_bytes;	// 1字节, 协议头部字节数
	WORD	msg_id;			// 2字节, 消息ID
	WORD	check_sum;		// 2字节, CRC16校验和
	DWORD	data_bytes;		// 4字节, 消息数据字节数
	DWORD	reserve;		// 4字节, 保留用于对齐，留待后面使用
};


/*
 * ITinySocketMsg 的消息侦听器接口类 IMsgListener。当它的子类对象被构造时，
 * 将会自动添加到 ITinySocketMsg 的侦听器容器中（调用函数 RegisterListener()），
 * 而当被析构时，将会从侦听器容器中删除（调用函数 UnregisterListener()）。当接收到消息
 * 后，侦听器函数 IsMsgNeedToHandle() 会被调用，用于检查该消息是否需要处理，若需
 * 处理，则调用侦听器函数 HandleMsg() 将消息传递给侦听器进行处理。
 */
class W2X_NETWORK_API IMsgListener
{
public:
	IMsgListener(void);
	virtual ~IMsgListener(void);

W2X_DISALLOW_COPY_AND_ASSIGN(IMsgListener)

public:
	/*
	 * 用于过滤消息，判断对应 ID 的消息是否需要进行处理。
	 */
	virtual bool IsMsgNeedToHandle(UINT _msg_id) = 0;

	/*
	 * 处理接收到的消息。
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
 * CTinySocket 消息封装接口类 ITinySocketMsg，它的子类对象可以创建并发送消息，
 * 也可以接收并处理回应消息。发送的消息有一个消息 ID，回应的消息也有一个消息 ID，这两个
 * ID 值不同。回应消息处理函数 HandleRecvMsg() 只处理指定 ID 的回应消息。发送消息和
 * 回应消息被称为一对通信消息，每个通信都有一个版本号，当不同的客户端版本不同时，接收到
 * 一个消息可决定不给于回应。
 */
class W2X_NETWORK_API ITinySocketMsg: public IBase
{
public:
	enum {
		MAX_MSG_DATA = 1024,	// 消息数据最大字节数
		INVALID_MSG_ID = 0,		// 无效的消息 ID
	};

	struct MsgInfo {
		MsgHeader msg_header;
		BYTE msg_data[ITinySocketMsg::MAX_MSG_DATA];
	};

public:
	ITinySocketMsg(
		WORD _send_msg_id,	// 用于发送的消息 ID，为 0 表示不可发送
		WORD _recv_msg_id,	// 期待接收的消息 ID，当收到消息后， HandleRecvMsg() 被调用
		BYTE _version = 1	// 一对消息通信的版本号
	);

	virtual ~ITinySocketMsg(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketMsg)
W2X_IMPLEMENT_REFCOUNTING(ITinySocketMsg)

public:
	/**
	 * 一般情况下，当发送一个消息出去后，将会接收到一个回应消息，
	 * CTinySocket 消息循环将会调用该函数来处理接收到的消息。
	 */
	virtual void HandleRecvMsg(
		LPCTSTR _remote_ip_addr,
		WORD _remote_port,	// 本地字节序
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		DWORD _data_bytes
	) = 0;

	/**
	 * 当发送一个消息时，并未立即发送，而是进入发送队列，在发送前，
	 * CTinySocket 消息循环将会调用该函数，以便于二次处理发送消息。
	 */
	virtual void HandleSendMsg(
		LPCTSTR _remote_ip_addr,
		WORD _remote_port,	// 本地字节序
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		DWORD _data_bytes
	) = 0;

	/**
	 * 构建一条将要发送的消息数据结构，构建成功返回 true，否则返回 false。
	 */
	bool CreateMsg(
		const BYTE* _msg_data_ptr = NULL,
		DWORD _data_bytes = 0,
		DWORD _fill_bytes = 0,
		BYTE _filler = 0x00
	);

	/**
	 * 构建一条UTF-8字符串消息用于发送，构建成功返回 true，否则返回 false。
	 */
	bool CreateUtf8Msg(
		LPCTSTR _t_str_ptr,
		DWORD _fill_bytes = 0,
		BYTE _filler = 0x00
	);

	/**
	 * 发送消息数据结构有效返回 true，否则返回 false。
	 */
	bool IsValid(void) const;

	/**
	 * 向远程主机发送消息。
	 * _remote_addr_str 可为 IP 地址字符串，如 "192.168.1.121"， 也可以
	 * 为主机名或域名，如 "www.baidu.com"；_reomte_port 为远程主机端口号。
	 * 如果 _remote_addr_str 值为 NULL 则发送局域网广播消息。
	 * 若成功则返回发送字节数，若失败则并返回 SOCKET_ERROR(-1)。
	 */
	bool SendMsg(
		LPCTSTR _remote_addr_str,
		WORD _reomte_port,	// 本地字节序
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

	/** 注册消息侦听器，当收到侦听器指定的消息时将会传递给侦听器处理。*/
	static bool RegisterListener(IMsgListener* _listener_ptr);

	/** 不再接收消息通知。*/
	static bool UnregisterListener(IMsgListener* _listener_ptr);

	/** 设置消息协议验证码 */
	static void SetAuthCode(WORD dwCode);

	/** 是否过滤掉会发消息 */
	static void FilterOutLoopback(bool _is_filter_out);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_tiny_socket_msg_H__ */
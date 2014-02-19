/*******************************************************************************
 * 文件：	interfacess_of_tiny_socket.h
 * 描述：	定义 CTinySocket 的几个接口类，如消息接口、消息分发器接口、消息监听器接口。
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-08
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
 * CTinySocket 消息侦听器接口类 IMessageListener。当它的子类对象被构造时，将会自动向
 * CTinySocket 进行注册（调用函数 CTinySocket::RegisterListener()），而当被析构时，将
 * 会自动进行注销（调用函数 CTinySocket::UnregisterListener()）。CTinySocket 对象
 * 接收到消息后，会把消息放到队列中，出队后将其封装成 ITinySocketMessage 子类对象，接着调
 * 用侦听器函数 IsMessageNeedToHandle() 检查该消息是否需要处理，若需处理，则调用侦听器函
 * 数 HandleReceivedMessage() 传递给侦听器。
 */
class W2X_NETWORK_API IMessageListener
{
public:
	IMessageListener(void);
	virtual ~IMessageListener(void);

W2X_DISALLOW_COPY_AND_ASSIGN(IMessageListener)

public:
	/*
	 * 用于过滤消息，判断对应 ID 的消息是否需要进行处理。
	 */
	virtual bool IsMessageNeedToHandle(UINT _msg_id) = 0;

	/*
	 * 处理接收到的消息。
	 */
	virtual void HandleMessage(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header_ref,
		const BYTE* _msg_data_ptr,
		WORD _data_bytes
	) = 0;
};

/*
 * CTinySocket 消息封装接口类 ITinySocketMessage，它的子类对象可以创建并发送消息，
 * 也可以接收并处理回应消息。发送的消息有一个消息 ID，回应的消息也有一个消息 ID，这两个
 * ID 值不同。回应消息处理函数 HandleEchoMsg() 只处理指定 ID 的回应消息。发送消息和
 * 回应消息被称为一对通信消息，每个通信都有一个版本号，当不同的客户端版本不同时，接收到
 * 一个消息可决定不给于回应。
 */
class W2X_NETWORK_API ITinySocketMessage
{
public:
	enum {
		MAX_MSG_DATA = 1024,	// 消息数据最大字节数
		INVALID_MSG_ID = 0,		// 无效的消息 ID
	};

	struct MsgInfo {
		MsgHeader msg_header;
		BYTE msg_data[ITinySocketMessage::MAX_MSG_DATA];
	};

public:
	ITinySocketMessage(
		WORD _send_msg_id,	// 发送消息的 ID
		WORD _echo_msg_id,	// 回应消息的 ID
		WORD _version		// 一对消息通信的版本号
	);

	virtual ~ITinySocketMessage(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketMessage)

public:
	/*
	 * 一般情况下，当发送一个消息出去后，将会接收到一个回应消息，
	 * CTinySocket 消息循环将会调用该函数来处理回应消息。
	 */
	virtual void HandleEchoMsg(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		WORD _data_bytes
	) = 0;

	/*
	 * 构建一条将要发送的消息数据结构，构建成功返回 true，否则返回 false。
	 */
	bool CreateSendMsg(
		const BYTE* _msg_data_ptr, 
		WORD _data_bytes
	);

	/*
	 * 发送消息数据结构有效返回 true，否则返回 false。
	 */
	bool IsValid(void) const;

	/*
	 * 向远程主机发送消息。
	 * _remote_addr_str 可为 IP 地址字符串，如 "192.168.1.121"， 也可以
	 * 为主机名或域名，如 "www.baidu.com"；_reomte_port 为远程主机端口号。
	 * 如果 _remote_addr_str 值为 NULL 则发送局域网广播消息。
	 * 若成功则返回发送字节数，若失败则并返回 SOCKET_ERROR(-1)。
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
	 * 注册消息侦听器，当收到侦听器指定的消息时将会传递给侦听器处理。
	 */
	static bool RegisterListener(IMessageListener* _listener_ptr);

	/* 
	 * 注销消息侦听器，不再接收消息通知。
	 */
	static bool UnregisterListener(IMessageListener* _listener_ptr);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__ */
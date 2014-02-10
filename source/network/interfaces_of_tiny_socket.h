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


class ITinySocketListener;

/*
 * CTinySocket 消息分发器接口类 ITinySocketDispatcher。当它的子类对象被构造时，
 * 将会自动向 CTinySocket 进行注册（调用函数 RegisterDispatcher()），而当被析构
 * 时，将会自动进行注销（调用函数 UnregisterDispatcher()）。CTinySocket 对象接
 * 收到消息后，会把消息放到队列中，接着调用分发器的函数 DispatchReceviedMessage()
 * 将原始的二进制消息传递给分发器。分发器需将原始的二进制消息封装成 ITinySocketMessage
 * 子类对象，然后再分发给对应的消息侦听器。当然，侦听器想要接收到消息通知的前提是，
 * 先调用分发器的函数 RegisterListener() 进行注册。当不再侦听时，可通过调用函数 
 * UnregisterListener() 注销消息通知。
 */
class W2X_NETWORK_API ITinySocketDispatcher
{
public:
	ITinySocketDispatcher(void);
	virtual ~ITinySocketDispatcher(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketDispatcher)

public:
	/*
	 * 将接收到的消息经过封装后分发给各消息侦听器。
	 * _msg_ptr 指向一个原始的二进制消息数据缓冲区，
	 * _msg_bytes 为原始二进制消息的字节数。
	 */
	virtual void DispatchReceviedMessage(
		const BYTE* _msg_ptr, 
		size_t _msg_bytes
	) = 0;

	/* 
	 * 注册消息侦听器，只有注册后的侦听器才能接收到消息通知。
	 */
	virtual bool RegisterListener(
		ITinySocketListener* _listener_ptr
	) = 0;

	/* 
	 * 注销消息侦听器，不再接受消息通知，请务必将不再使用的侦听器注销掉。
	 */
	virtual bool UnregisterListener(
		ITinySocketListener* _listener_ptr
	) = 0;
};

/*
 * Socket 消息封装接口类 ITinySocketMessage，定义了消息的协议结构。
 */
class W2X_NETWORK_API ITinySocketMessage
{
public:
	enum {
		MAX_MSG_DATA = 1024,	// 消息数据最大字节数
		INVALID_MSG_ID = 0,		// 无效的消息 ID
	};

public:
	ITinySocketMessage(void);
	virtual ~ITinySocketMessage(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketMessage)

public:
	virtual void Handle(const BYTE* _msg_data) = 0;

	/*
	 * 构建消息数据结构，构建成功返回 true，否则返回 false。
	 */
	virtual bool Create(
		WORD _msg_id, 
		const BYTE* _msg_data_ptr, 
		WORD _data_bytes
	);

	/*
	 * 消息数据结构有效返回 true，否则返回 false。
	 */
	virtual bool IsValid(void) const;

	/*
	 * 向远程主机发送 UDP 消息。
	 * _is_broadcast 表示是否发送广播消息，如果要发送广播消息，则 _host 和 _port 
	 * 使用默认值，即 _host 值为 NULL，_port 值为 0。如果不发送广播消息，则 _host
	 * 和 _port 均要设置，_host 可为 IP 地址字符串，如 "192.168.1.121"，
	 * 也可以为主机名或域名，如 "www.baidu.com"；_port 为远程主机端口号。
	 * 若成功则返回发送字节数，若失败则并返回 SOCKET_ERROR(-1)。
	 */
	virtual bool SendUdp(
		bool _is_broadcast = false,
		LPCTSTR _host = NULL,
		WORD _port = 0
	) const;
	
	
	WORD GetId(void) const;
	WORD GetSize(void) const;
	const BYTE* GetData(void) const;
	bool GetData(OUT PBYTE _data_bufer_ptr, OUT size_t& _data_bytes) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

/*
 * CTinySocket 消息侦听器接口类 ITinySocketListener。它的子类对象必须向 CTinySocket
 * 消息分发器（ITinySocketDispatcher 子类对象）进行注册（通过调用消息分发器的函数
 * RegisterListener()），只有注册后才能接收到相应的消息通知。侦听器对象在析构时将自动注销
 * 消息通知（通过调用消息分发器的函数 UnregisterListener()）。消息分发器将封装后的消息
 * 转递给侦听器函数 HandleReceivedMessage()，用于处理接收到的消息。
 */
class W2X_NETWORK_API ITinySocketListener
{
public:
	ITinySocketListener(ITinySocketDispatcher* _dispatcher_ptr);
	virtual ~ITinySocketListener(void);

W2X_DISALLOW_COPY_AND_ASSIGN(ITinySocketListener)

public:
	/*
	 * 用于过滤消息，判断对应 ID 的消息是否需要进行处理。
	 */
	virtual bool IsMessageNeedToHandle(UINT _msg_id) = 0;

	/*
	 * 处理接收到的消息。
	 */
	virtual void HandleReceivedMessage(const ITinySocketMessage* _msg_ptr) = 0;

private:
	ITinySocketDispatcher* const m_tiny_socket_dispatcher_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_INTERFACES_OF_TINY_SOCKET_H__ */
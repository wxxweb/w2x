/*******************************************************************************
 * 文件：	tiny_socket.h
 * 描述：	对 socket 调用的一个小小的封装。
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-04-05
 ******************************************************************************/

#ifndef __W2X_NETWORK_TINY_SOCKET_H__
#define __W2X_NETWORK_TINY_SOCKET_H__

#include "exports.h"
#include "../common/macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)

/*
 * 侦听器接口类，定义了一个处理器函数接口，当 CTinySocket 对象异步接收消息后，
 * 会把消息放到消息队列中，接着处理器函数将异步回调，用于处理接收到的消息。
 * 当 ITinySocketListener 子类对象构造时，将会自动注册侦听器，析构自动注销。
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
		MAX_MSG_BUF_SIZE = 0x2000,	// 数据包消息缓存大小
	};
	/*
	 * 异步接收数据的返回状态
	 */
	enum ERecvStatus {
		kAsyncRecvError		= -1,	// 接收过程中出错
		kAsyncRecvPending	= 0,	// 正在接收中，未完成
		kAsyncRecvComplete	= 1,	// 接收数据完成
	};

public:
	CTinySocket(void);
	virtual ~CTinySocket(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CTinySocket)

public:
	/* 
	 * 进程调用该函数初始化 WinSock DLL，若成功则返回 true, 否则返回 false。
	 */
	static bool Initialize(void);

	/* 
	 * 进程调用该函数释放WinSock DLL，若成功则返回 true, 否则返回 false。
	 */
	static bool Uninitialize(void);

	/* 
	 * 创建用于收发 UDP 数据包的 Socket，并将本地 IP 地址绑定到 Socket。
	 * _local_port 为本地端口号，如果值为 0 则自动分配 1024-5000 间端口号。
	 * _is_enable_broadcast 用于设置是否发送广播消息，默认为 false 不发送
	 * 若成功则返回 true, 否则释放创建过程中的资源并返回 false。
	 */
	bool CreateUdp(WORD _local_port = 0, bool _is_broadcast = false);

	/*
	 * 销毁已创建的 Socket 资源， 若成功则返回 true, 否则返回 false。
	 */
	bool Destory(void);

	/* 
	 * 同步接收 UDP 数据包。
	 * _remote_addr_ptr 存放远程主机地址， 
	 * _packet_buffer 存放接收到的数据，
	 * _size_in_bytes 数据缓冲区字节数。
	 * 若接收到数据包，则返回数据字节数，若接收过程中出错则返回 SOCKET_ERROR(-1)。
	 */
	int SyncRecvUdpPacket(
		OUT	PSOCKADDR_IN _remote_addr_ptr,
		OUT	PBYTE _packet_buffer,
		const DWORD _size_in_bytes
	);

	/*
	 * 发送 UDP 数据包。
	 * _remote_addr_ptr 远程主机地址；
	 * _packet_buffer 要发送数据的缓冲区；
	 * _size_in_bytes 要发送的字节数。
	 * 若成功则返回发送字节数，若失败则并返回 SOCKET_ERROR(-1)。
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
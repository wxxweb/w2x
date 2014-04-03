/*******************************************************************************
 * 文件：	tiny_socket.h
 * 描述：	轻量级的封装类，实现对重叠异步 socket 的封装。
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-05
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
		 * 以太网（Ethernet）的 MTU（最大传输单元）为 1500 字节，
		 * IP 数据报的首部为 20 字节，UDP数据报的首部 8 字节，
		 * 所以 UDP 数据报的数据区最大长度为 1472 字节。
		 * Internet上的标准 MTU 值为 576 字节
		 * 此时 UDP 的数据报的最大长度长度为 548 字节。
		 */
		MAX_UDP_MSG_SIZE = 512,
	};

	typedef bool (CALLBACK *FnPacketDispatcher)(
		DWORD _remote_ip_addr,	// 网络字节序
		WORD _remote_port,		// 网络字节序
		const BYTE* _packet_data_ptr,
		WORD _data_bytes
	); 

public:
	CTinySocket(void);
	virtual ~CTinySocket(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CTinySocket)

public:
	/**
	 * 创建用于收发数据包的 Socket，并将本地 IP 地址绑定到 Socket。
	 * _local_port 为本地端口号，如果值为 0 则自动分配 1024-5000 间端口号。
	 * _is_udp 用于指定 Socket 的传输方式是否为 UDP，默认 false 为 TCP。
	 * 若创建成功则返回 true, 否则释放创建过程中的资源并返回 false。
	 */
	bool CreateUdp(
		WORD _local_port = 0, 
		LPCTSTR _mcast_addr_str = NULL, 
		WORD _mcast_port = 0
	);

	/** 检测 Socket 是否创建成功并有效可用，有效返回 true，否则返回 false。*/
	bool IsValid(void) const;

	/** 销毁已创建的 Socket 资源， 若成功则返回 true, 否则返回 false。*/
	void Destroy(void);

	/** 加入一个多播组 */
	bool JoinMulticast();

	/**
	 * 发送数据包。
	 * _remote_addr_str 远程主机地址，可以是 IP 地址，如 "192.168.1.121"，也可以是
	 * 域名，如 "www.baidu.com"。当值为空（NULL）是发送局域网广播消息；
	 * _port 远程主机端口号；
	 * _packet_buffer 要发送数据的缓冲区；
	 * _size_in_bytes 要发送的字节数；
	 * 若成功则返回发送字节数，若失败则并返回 SOCKET_ERROR(-1)。
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
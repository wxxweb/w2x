/*******************************************************************************
 * 文件：	lan_neighbor.h
 * 描述：	该模块中包含两个导出的接口函数findOthers和echoServer。下面称调用
 *          findOthers函数的本地主机为客户端，调用echoServer 函数的远程主机为
 *          服务器端。该模块功能是客户端通过调用 findOthers函数发送UDP广播包，
 *          并在服务器端调用 echoServer 函数来回应客户端的广播包。此时，客户端
 *          和服务器端将获知对方的地址，包括IP地址和用于发送UDP数据包的端口号。。
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-10
 ******************************************************************************/

#ifndef __W2X_NETWORK_LAN_NEIGHBOR_H__
#define __W2X_NETWORK_LAN_NEIGHBOR_H__

#include "exports.h"
#include "interfaces_of_tiny_socket.h"
#include "..\common\macros.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class W2X_NETWORK_API CLanNeighbor: public IMessageListener
{
public:
	CLanNeighbor(void);
	virtual ~CLanNeighbor(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CLanNeighbor)

public:
	virtual bool IsMessageNeedToHandle(UINT _msg_id);

	virtual void HandleMessage(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header_ref,
		const BYTE* _msg_data_ptr,
		WORD _data_bytes
	);

public:
	bool SayHello(WORD _remote_port);
	bool Listen(WORD _local_port);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_LAN_NEIGHBOR_H__ */
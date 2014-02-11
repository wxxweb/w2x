/*******************************************************************************
 * 文件：	lan_neighbor.cpp
 * 描述：	参见 lan_neighbor.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-10
 ******************************************************************************/

#include "stdafx.h"
#include "lan_neighbor.h"
#include "utility.h"
#include "tiny_socket.h"

/*
 * 用于验证双方身份的 GUID，由双发事先约定好。 
 */
static const GUID AUTHENTCATION_GUID[] = {
	0x37ff70ac,					// data1
	0x8215,						// data2
	0x490c,						// data3
	{0x81, 0xC8, 0xF4, 0x4B, 
     0xF8, 0x59, 0xEB, 0x6F}	// data4
};


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class CMsgNeighbor: public ITinySocketMessage
{
public:
	CMsgNeighbor(void) {}
	virtual ~CMsgNeighbor(void) {}

W2X_DISALLOW_COPY_AND_ASSIGN(CMsgNeighbor)

public:
	virtual void Handle(DWORD _remote_ip_addr, const BYTE* _msg_data);
};

void CMsgNeighbor::Handle(DWORD _remote_ip_addr, const BYTE* _msg_data)
{
	// 接收到回应消息
	if (0 != memcmp(&AUTHENTCATION_GUID, _msg_data, sizeof(GUID)))
	{
		return;
	}

	TCHAR ip_addr_str_buf[internal::MAX_IP_ADDR_STR] = TEXT("");
	BYTE  msg_data_buf[SO_MAX_MSG_SIZE] = {0};
	SOCKADDR_IN	remote_sock_addr_in = {0};

	internal::ParseIpAddressString(ip_addr_str_buf, 
		sizeof(ip_addr_str_buf), _remote_ip_addr);

	_tprintf_s(
		TEXT("Received the ack msg.\nRemote host IP: %s\n"), 
		ip_addr_str_buf);

	// 发送回应消息
	const int sent_result = ITinySocketMessage::SendUdp(
		ip_addr_str_buf, 22222, (PBYTE)msg_data_buf, sizeof(GUID));
	if (SOCKET_ERROR == sent_result)
	{
		_tprintf_s(TEXT("Send UDP packet faild.\n"));
		return;
	}

	_tprintf_s(TEXT("Send response msg [%d Bytes].\n"), sent_result);
}


class CLanNeighbor::CImpl
{
public:
	CImpl(void);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)

public:
	bool SayHello(WORD _remote_port);
	bool Listen(WORD _local_port);

public:
	CMsgNeighbor* msg_neighbor_ptr;
};


CLanNeighbor::CImpl::CImpl(void)
{}


CLanNeighbor::CImpl::~CImpl(void)
{
	SAFE_DELETE(msg_neighbor_ptr);
}


CLanNeighbor::CLanNeighbor(void)
	: m_impl_ptr(new CImpl())
{

}

CLanNeighbor::~CLanNeighbor(void)
{

}

bool CLanNeighbor::IsMessageNeedToHandle(UINT _msg_id)
{
	return 1 == _msg_id;
}

void CLanNeighbor::HandleReceivedMessage(
	DWORD _remote_ip_addr,
	UINT _msg_id, 
	const BYTE* _msg_data, 
	WORD _data_bytes
	)
{
	if (NULL == _msg_data)
	{
		return;
	}

	CMsgNeighbor msg_neighbor;
	msg_neighbor.Handle(_remote_ip_addr, _msg_data);
}

bool CLanNeighbor::CImpl::SayHello(WORD _remote_port)
{
	ITinySocketMessage::Initialize(0);

	CMsgNeighbor msg_neighbor;
	if (false == msg_neighbor.Create(1, (PBYTE)&AUTHENTCATION_GUID, sizeof(GUID)))
	{
		return false;
	}

	const int send_bytes = msg_neighbor.SendUdp(NULL, _remote_port);
	if (SOCKET_ERROR == send_bytes)
	{
		_tprintf_s(TEXT("Send UDP packet faild.\n"));
		return false;
	}
	else
	{
		_tprintf_s(TEXT("Send broadcast msg [%d Bytes].\n"), send_bytes);
	}

	return true;
}


bool CLanNeighbor::CImpl::Listen(WORD _local_port)
{
	_tprintf_s(TEXT("Receiving UDP packet...\n"));

	ITinySocketMessage::Initialize(_local_port);

	return true;
}


bool CLanNeighbor::SayHello(WORD _remote_port)
{
	return m_impl_ptr->SayHello(_remote_port);
}

bool CLanNeighbor::Listen(WORD _local_port)
{
	return m_impl_ptr->Listen(_local_port);
}

W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END

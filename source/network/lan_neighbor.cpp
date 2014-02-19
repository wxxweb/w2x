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


class CMsgHello: public ITinySocketMessage
{
public:
	CMsgHello(void) : ITinySocketMessage(1, 2, 1) {}
	virtual ~CMsgHello(void) {}

W2X_DISALLOW_COPY_AND_ASSIGN(CMsgHello)

public:
	virtual void HandleEchoMsg(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		WORD _data_bytes
	);
};


class CMsgEchoHello: public ITinySocketMessage
{
public:
	CMsgEchoHello(void) : ITinySocketMessage(2, 1, 1) {}
	virtual ~CMsgEchoHello(void) {}

	W2X_DISALLOW_COPY_AND_ASSIGN(CMsgEchoHello)

public:
	virtual void HandleEchoMsg(
		DWORD _remote_ip_addr,
		const MsgHeader& _msg_header,
		const BYTE* _msg_data,
		WORD _data_bytes
	);
};


void CMsgHello::HandleEchoMsg(
	DWORD _remote_ip_addr,
	const MsgHeader& _msg_header,
	const BYTE* _msg_data,
	WORD _data_bytes
	)
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
		MAX_IP_ADDR_STR, _remote_ip_addr);

	_tprintf_s(
		TEXT("Received the echo msg.\nRemote host IP: %s\n"), 
		ip_addr_str_buf);
}


void CMsgEchoHello::HandleEchoMsg(
	DWORD _remote_ip_addr,
	const MsgHeader& _msg_header_ref,
	const BYTE* _msg_data_ptr,
	WORD _data_bytes
	)
{
	// 接收到回应消息
	if (0 != memcmp(&AUTHENTCATION_GUID, _msg_data_ptr, sizeof(GUID)))
	{
		return;
	}

	TCHAR ip_addr_str_buf[internal::MAX_IP_ADDR_STR] = TEXT("");
	BYTE  msg_data_buf[SO_MAX_MSG_SIZE] = {0};
	SOCKADDR_IN	remote_sock_addr_in = {0};

	internal::ParseIpAddressString(ip_addr_str_buf, 
		MAX_IP_ADDR_STR, _remote_ip_addr);

	_tprintf_s(
		TEXT("Received the hello msg.\nRemote host IP: %s\n"), 
		ip_addr_str_buf);

	// 发送回应消息

	if (true == this->CreateSendMsg(_msg_data_ptr, sizeof(GUID)))
	{
		const int sent_bytes = this->SendMsg(ip_addr_str_buf, 22222, true);
		if (SOCKET_ERROR == sent_bytes)
		{
			_tprintf_s(TEXT("Send UDP packet faild.\n"));
			return;
		}

		_tprintf_s(TEXT("Send echo hello msg [%d Bytes].\n"), sent_bytes);
	}
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
	CMsgHello* m_msg_hello_ptr;
	CMsgEchoHello* m_msg_echo_hello_ptr;
};


CLanNeighbor::CImpl::CImpl(void)
	: m_msg_hello_ptr(NULL)
	, m_msg_echo_hello_ptr(NULL)
{}


CLanNeighbor::CImpl::~CImpl(void)
{
	SAFE_DELETE(m_msg_hello_ptr);
	SAFE_DELETE(m_msg_echo_hello_ptr);
}


CLanNeighbor::CLanNeighbor(void)
	: m_impl_ptr(new CImpl())
{

}

CLanNeighbor::~CLanNeighbor(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}

bool CLanNeighbor::IsMessageNeedToHandle(UINT _msg_id)
{
	return 1 == _msg_id;
}

void CLanNeighbor::HandleMessage(
	DWORD _remote_ip_addr,
	const MsgHeader& _msg_header_ref,
	const BYTE* _msg_data_ptr,
	WORD _data_bytes
	)
{
	if (NULL == _msg_data_ptr)
	{
		return;
	}
}

bool CLanNeighbor::CImpl::SayHello(WORD _remote_port)
{
	ITinySocketMessage::InitializeUdp(22222);

	if (NULL == m_msg_hello_ptr)
	{
		m_msg_hello_ptr = new CMsgHello();
		IF_NULL_ASSERT_RETURN_VALUE(m_msg_hello_ptr, false);
	}
	
	if (false == m_msg_hello_ptr->CreateSendMsg(
		reinterpret_cast<const BYTE*>(&AUTHENTCATION_GUID), sizeof(GUID)))
	{
		return false;
	}

	const int send_bytes = m_msg_hello_ptr->SendMsg(NULL, _remote_port, true);
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

	ITinySocketMessage::InitializeUdp(_local_port);

	if (NULL == m_msg_echo_hello_ptr)
	{
		m_msg_echo_hello_ptr = new CMsgEchoHello();
		IF_NULL_ASSERT_RETURN_VALUE(m_msg_echo_hello_ptr, false);
	}

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

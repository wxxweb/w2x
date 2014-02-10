/*******************************************************************************
 * 文件：	interfacess_of_tiny_socket.cpp
 * 描述：	参见 interfacess_of_tiny_socket.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-08
 ******************************************************************************/

#include "stdafx.h"
#include "interfaces_of_tiny_socket.h"
#include "tiny_socket.h"
#include "utility.h"
#include "..\common\common.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


ITinySocketDispatcher::ITinySocketDispatcher(void)
{
	CTinySocket::RegisterDispatcher(this);
}


ITinySocketDispatcher::~ITinySocketDispatcher(void)
{
	CTinySocket::UnregisterDispatcher(this);
}


struct MsgInfo {
	WORD msg_id;
	WORD data_bytes;
	BYTE data_buffer[ITinySocketMessage::MAX_MSG_DATA];
};

class ITinySocketMessage::CImpl
{
public:
	CImpl(void);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)

public:
	bool m_is_valid;
	MsgInfo* m_msg_info_ptr;
};


ITinySocketMessage::CImpl::CImpl(void)
	: m_msg_info_ptr(NULL)
	, m_is_valid(false)
{

}


ITinySocketMessage::CImpl::~CImpl(void)
{
	SAFE_DELETE(m_msg_info_ptr);
}


ITinySocketMessage::ITinySocketMessage(void)
	: m_impl_ptr(new CImpl())
{
}


ITinySocketMessage::~ITinySocketMessage(void)
{
	SAFE_DELETE(static_cast<CImpl*>(m_impl_ptr));
}


bool ITinySocketMessage::IsValid(void) const
{
	return m_impl_ptr->m_is_valid;
}


bool ITinySocketMessage::Create(
	WORD _msg_id, 
	const BYTE* _msg_data_ptr, 
	WORD _data_bytes)
{
	IF_FALSE_ASSERT (
		INVALID_MSG_ID != _msg_id && NULL != _msg_data_ptr && 
		0 < _data_bytes && _data_bytes <= MAX_MSG_DATA)
	{
		return m_impl_ptr->m_is_valid = false;
	}

	SAFE_DELETE(m_impl_ptr->m_msg_info_ptr);
	MsgInfo* const msg_info_ptr = m_impl_ptr->m_msg_info_ptr = new MsgInfo;
	msg_info_ptr->msg_id = _msg_id;
	msg_info_ptr->data_bytes = _data_bytes;

	memcpy_s(&msg_info_ptr->data_buffer, MAX_MSG_DATA,
		_msg_data_ptr, _data_bytes);

	return true;
}


bool ITinySocketMessage::SendUdp(
	bool _is_broadcast,
	LPCTSTR _host,
	WORD _port
	) const
{
	IF_NULL_ASSERT_RETURN_VALUE(_host, false);
	IF_FALSE_ASSERT_RETURN_VALUE(m_impl_ptr->m_is_valid, false);
	
	CTinySocket tiny_socket;
	if (false == tiny_socket.CreateUdp(_port, _is_broadcast))
	{
		return false;
	}

	char ip_addr_str[MAX_IP_ADDR_STR] = "";

	if (true == internal::IsValidIpAddress(_host))
	{
		std::string ascii_ip_addr;
		if (false == w2x::encode::Unicode2Ascii(ascii_ip_addr, _host))
		{
			return false;
		}
		strcpy_s(ip_addr_str, MAX_IP_ADDR_STR, ascii_ip_addr.c_str());
	}
	else
	{
		std::string host_name;
		if (false == w2x::encode::Unicode2Ascii(host_name, _host))
		{
			return false;
		}

		HOSTENT* host_ent_ptr = gethostbyname(host_name.c_str());
		IF_FALSE_ASSERT (NULL != host_ent_ptr && NULL != host_ent_ptr->h_addr_list[0])
		{
			w2x::log::LogError(TEXT("Get IP address by name(%s) failed."), 
				_host);
			return false;
		}

		memcpy(&ip_addr_str, 
			inet_ntoa(*(in_addr*)host_ent_ptr->h_addr_list[0]), 
			MAX_IP_ADDR_STR);
	}
	
	SOCKADDR_IN sock_addr_in;
	sock_addr_in.sin_addr.S_un.S_addr = inet_addr(ip_addr_str);
	sock_addr_in.sin_family = AF_INET;
	sock_addr_in.sin_port = htons(_port);

	return tiny_socket.SendUdpPacket(&sock_addr_in, 
		m_impl_ptr->m_msg_info_ptr->data_buffer,
		m_impl_ptr->m_msg_info_ptr->data_bytes);
}


WORD ITinySocketMessage::GetId(void) const
{
	IF_NULL_ASSERT_RETURN_VALUE(m_impl_ptr->m_msg_info_ptr, INVALID_MSG_ID);

	return m_impl_ptr->m_msg_info_ptr->msg_id;
}


WORD ITinySocketMessage::GetSize(void) const
{
	IF_NULL_ASSERT_RETURN_VALUE(m_impl_ptr->m_msg_info_ptr, 0);

	return m_impl_ptr->m_msg_info_ptr->data_bytes;
}


const BYTE* ITinySocketMessage::GetData(void) const
{
	IF_NULL_ASSERT_RETURN_VALUE(m_impl_ptr->m_msg_info_ptr, NULL);

	return m_impl_ptr->m_msg_info_ptr->data_buffer;
}


bool ITinySocketMessage::GetData(
	OUT PBYTE _data_bufer_ptr, 
	OUT size_t& _data_bytes
	) const
{
	IF_NULL_ASSERT_RETURN_VALUE(_data_bufer_ptr, false);
	IF_FALSE_ASSERT_RETURN_VALUE(0 < _data_bytes, false);

	memset(_data_bufer_ptr, 0, _data_bytes);

	IF_NULL_ASSERT_RETURN_VALUE(m_impl_ptr->m_msg_info_ptr, false);
	
	memcpy_s(_data_bufer_ptr, _data_bytes, 
		m_impl_ptr->m_msg_info_ptr->data_buffer, 
		m_impl_ptr->m_msg_info_ptr->data_bytes);

	return true;
}


ITinySocketListener::ITinySocketListener(ITinySocketDispatcher* _dispatcher_ptr)
	: m_tiny_socket_dispatcher_ptr(_dispatcher_ptr)
{
	IF_NULL_ASSERT (m_tiny_socket_dispatcher_ptr)
	{
		// 啥都不做
	}
	else {
		m_tiny_socket_dispatcher_ptr->RegisterListener(this);
	}
}


ITinySocketListener::~ITinySocketListener(void)
{
	IF_NULL_ASSERT (m_tiny_socket_dispatcher_ptr)
	{
		// 啥都不做
	}
	else {
		m_tiny_socket_dispatcher_ptr->UnregisterListener(this);
	}
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
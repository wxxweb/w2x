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
	bool Create(
		WORD _msg_id, 
		const BYTE* _msg_data_ptr, 
		WORD _data_bytes
	);

	inline int Send(LPCTSTR _remote_addr_str, WORD _remote_port) const;

	static bool Initialize(WORD _local_port, bool _is_udp);

public:
	bool		 m_is_valid;
	MsgInfo*	 m_msg_info_ptr;
	static CTinySocket sm_udp_socket;
	static CTinySocket sm_tcp_socket;
};


CTinySocket ITinySocketMessage::CImpl::sm_udp_socket;
CTinySocket ITinySocketMessage::CImpl::sm_tcp_socket;


ITinySocketMessage::CImpl::CImpl(void)
	: m_is_valid(false)
	, m_msg_info_ptr(NULL)
{

}


ITinySocketMessage::CImpl::~CImpl(void)
{
	SAFE_DELETE(m_msg_info_ptr);
}


bool ITinySocketMessage::CImpl::Create(
	WORD _msg_id, 
	const BYTE* _msg_data_ptr, 
	WORD _data_bytes)
{
	IF_FALSE_ASSERT (INVALID_MSG_ID != _msg_id && NULL != _msg_data_ptr && 
		0 < _data_bytes && _data_bytes <= MAX_MSG_DATA)
	{
		return m_is_valid = false;
	}

	SAFE_DELETE(m_msg_info_ptr);
	MsgInfo* const msg_info_ptr = m_msg_info_ptr = new MsgInfo;
	memset(msg_info_ptr, 0, sizeof(MsgInfo));
	msg_info_ptr->msg_id = _msg_id;
	msg_info_ptr->data_bytes = _data_bytes;

	memcpy_s(&msg_info_ptr->data_buffer, MAX_MSG_DATA,
		_msg_data_ptr, _data_bytes);
	
	return m_is_valid = true;
}


inline int ITinySocketMessage::CImpl::Send(
	LPCTSTR _remote_addr_str,
	WORD _remote_port
	) const
{
	IF_FALSE_ASSERT_RETURN_VALUE(m_is_valid, SOCKET_ERROR);
	if (NULL == _remote_addr_str)
	{
		IF_FALSE_ASSERT_RETURN_VALUE(sm_udp_socket.IsValid(), SOCKET_ERROR);

		return sm_udp_socket.SendPacket(_remote_addr_str, _remote_port,
			m_msg_info_ptr->data_buffer, m_msg_info_ptr->data_bytes);
	}
	else
	{
		IF_FALSE_ASSERT_RETURN_VALUE(sm_tcp_socket.IsValid(), SOCKET_ERROR);

// 		return CImpl::sm_tcp_socket->SendUdpPacket(
// 			_remote_addr_str, _remote_port,
// 			m_msg_info_ptr->data_buffer, m_msg_info_ptr->data_bytes);
	}

	return SOCKET_ERROR;
}


bool ITinySocketMessage::CImpl::Initialize(WORD _local_port, bool _is_udp)
{
	if (true == _is_udp && false == sm_udp_socket.IsValid())
	{
		return sm_udp_socket.Create(_local_port, _is_udp);
	}

	if (false == _is_udp && false == sm_tcp_socket.IsValid())
	{
		return sm_tcp_socket.Create(_local_port, _is_udp);
	}

	return true;
}


ITinySocketMessage::ITinySocketMessage(void)
	: m_impl_ptr(new CImpl())
{
}


ITinySocketMessage::~ITinySocketMessage(void)
{
	SAFE_DELETE(static_cast<CImpl*>(m_impl_ptr));
}


bool ITinySocketMessage::Create(
	WORD _msg_id, 
	const BYTE* _msg_data_ptr, 
	WORD _data_bytes
	)
{
	return m_impl_ptr->Create(_msg_id, _msg_data_ptr, _data_bytes);
}


bool ITinySocketMessage::IsValid(void) const
{
	return m_impl_ptr->m_is_valid;
}


bool ITinySocketMessage::InitializeUdp(WORD _local_port)
{
	return CImpl::Initialize(_local_port, true);
}


int ITinySocketMessage::Send(LPCTSTR _remote_addr_str, WORD _reomte_port) const
{
	return m_impl_ptr->Send(_remote_addr_str, _reomte_port);
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


int ITinySocketMessage::Send(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{
	if (NULL == _remote_addr_str && 
		false == ITinySocketMessage::CImpl::sm_udp_socket.IsValid())
	{
		return ITinySocketMessage::CImpl::sm_udp_socket.SendPacket(
			NULL, _remote_port, _packet_buffer, _size_in_bytes);
	}

	if (NULL != _remote_addr_str  && 
		false == ITinySocketMessage::CImpl::sm_tcp_socket.IsValid())
	{
		return ITinySocketMessage::CImpl::sm_tcp_socket.SendPacket(
			_remote_addr_str, _remote_port, _packet_buffer, _size_in_bytes);
	}
	
	return SOCKET_ERROR;
}


ITinySocketListener::ITinySocketListener(void)
{
	CTinySocket::RegisterListener(this);
}


ITinySocketListener::~ITinySocketListener(void)
{
	CTinySocket::UnregisterListener(this);
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
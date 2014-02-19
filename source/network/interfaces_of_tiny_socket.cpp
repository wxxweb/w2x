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


class ITinySocketMessage::CImpl
{
	typedef std::multimap<WORD, CImpl*> Messages;
	typedef std::list<IMessageListener*> Listeners;
	
public:
	CImpl(WORD _send_msg_id, WORD _echo_msg_id, WORD _version);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)
W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)

public:
	bool Create(
		const BYTE* _msg_data_ptr, 
		WORD _data_bytes
	);

	inline int Send(
		LPCTSTR _remote_addr_str, 
		WORD _remote_port, 
		bool _is_udp);

	inline WORD GetSize(void);

	inline const BYTE* GetData(void);

	inline bool GetData(
		OUT PBYTE _data_bufer_ptr, 
		OUT size_t& _data_bytes
	);

	static bool Initialize(WORD _local_port, bool _is_udp);

	static bool CALLBACK DispatchPacket(
		DWORD _remote_ip_addr,
		const BYTE* _packet_data_ptr,
		WORD _data_bytes
	);

	static bool RegisterListener(IMessageListener* _listener_ptr);

	static bool UnregisterListener(IMessageListener* _listener_ptr);

public:
	WORD	 m_send_msg_id;
	WORD	 m_echo_msg_id;
	WORD	 m_version;
	bool	 m_is_valid;
	MsgInfo* m_msg_info_ptr;
	ITinySocketMessage* m_parent;

	static CTinySocket sm_udp_socket;
	static CTinySocket sm_tcp_socket;
	static Messages	   sm_messages;
	static Listeners   sm_listeners;
};

W2X_IMPLEMENT_LOCKING_CLASS(ITinySocketMessage::CImpl, CAutoLock)

CTinySocket ITinySocketMessage::CImpl::sm_udp_socket;
CTinySocket ITinySocketMessage::CImpl::sm_tcp_socket;
ITinySocketMessage::CImpl::Listeners ITinySocketMessage::CImpl::sm_listeners;
ITinySocketMessage::CImpl::Messages	 ITinySocketMessage::CImpl::sm_messages;


ITinySocketMessage::CImpl::CImpl(
	WORD _send_msg_id, 
	WORD _echo_msg_id, 
	WORD _version
	)
	: m_send_msg_id(_send_msg_id)
	, m_echo_msg_id(_echo_msg_id)
	, m_version(_version)
	, m_is_valid(false)
	, m_msg_info_ptr(NULL)
	, m_parent(NULL)
{
	ASSERT(INVALID_MSG_ID != m_echo_msg_id);
	ASSERT(INVALID_MSG_ID != m_echo_msg_id);

	if (INVALID_MSG_ID != m_echo_msg_id)
	{
		CAutoLock class_lock(NULL, __FUNCTION__);
		sm_messages.insert(std::pair<WORD, CImpl*>(m_echo_msg_id, this));
	}
}


ITinySocketMessage::CImpl::~CImpl(void)
{
	if (INVALID_MSG_ID != m_echo_msg_id)
	{
		CAutoLock class_lock(NULL, __FUNCTION__);

		for (std::pair<Messages::iterator, Messages::iterator> range 
				= sm_messages.equal_range(m_echo_msg_id);
			range.first != range.second; ++(range.first))
		{
			CImpl* msg_obj_ptr = range.first->second;
			IF_NULL_ASSERT(msg_obj_ptr)
			{
				continue;
			}
			if (msg_obj_ptr == this)
			{
				sm_messages.erase(range.first);
				break;
			}
		}
	}

	SAFE_DELETE(m_msg_info_ptr);
}


bool ITinySocketMessage::CImpl::Create(
	const BYTE* _msg_data_ptr, 
	WORD _data_bytes
	)
{
	CAutoLock this_lock(this, __FUNCTION__);

	IF_FALSE_ASSERT (INVALID_MSG_ID != m_send_msg_id && NULL != _msg_data_ptr && 
		0 < _data_bytes && _data_bytes <= MAX_MSG_DATA)
	{
		return m_is_valid = false;
	}
	
	if (NULL == m_msg_info_ptr)
	{
		m_msg_info_ptr = new MsgInfo;
		IF_NULL_ASSERT (m_msg_info_ptr)
		{
			return m_is_valid = false;
		}
	}

	MsgHeader msg_header = {0};
	msg_header.version = m_version;
	msg_header.msg_id = m_send_msg_id;
	msg_header.msg_data_bytes = _data_bytes;
	msg_header.check_sum = 0;
	
	memset(m_msg_info_ptr, 0, sizeof(MsgInfo));
	memcpy(&m_msg_info_ptr->msg_header, &msg_header, sizeof(MsgHeader));
	memcpy(&m_msg_info_ptr->msg_data, _msg_data_ptr, _data_bytes);
	
	return m_is_valid = true;
}


inline int ITinySocketMessage::CImpl::Send(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	bool _is_udp
	)
{
	CAutoLock this_lock(this, __FUNCTION__);

	IF_FALSE_ASSERT_RETURN_VALUE(m_is_valid, SOCKET_ERROR);
	IF_FALSE_ASSERT_RETURN_VALUE(0 != _remote_port, SOCKET_ERROR);

	if (NULL == _remote_addr_str || true == _is_udp)
	{
		IF_FALSE_ASSERT_RETURN_VALUE(sm_udp_socket.IsValid(), SOCKET_ERROR);

		return sm_udp_socket.SendPacket(_remote_addr_str, _remote_port,
			reinterpret_cast<const BYTE*>(m_msg_info_ptr), 
			m_msg_info_ptr->msg_header.msg_data_bytes + sizeof(MsgHeader));
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


inline WORD ITinySocketMessage::CImpl::GetSize(void)
{
	CAutoLock this_lock(this, __FUNCTION__);

	IF_NULL_ASSERT_RETURN_VALUE(m_msg_info_ptr, 0);

	return m_msg_info_ptr->msg_header.msg_data_bytes;
}


inline const BYTE* ITinySocketMessage::CImpl::GetData(void)
{
	CAutoLock this_lock(this, __FUNCTION__);

	IF_NULL_ASSERT_RETURN_VALUE(m_msg_info_ptr, NULL);

	return m_msg_info_ptr->msg_data;
}


inline bool ITinySocketMessage::CImpl::GetData(
	OUT PBYTE _data_bufer_ptr, 
	OUT size_t& _data_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_data_bufer_ptr, false);
	IF_FALSE_ASSERT_RETURN_VALUE(0 < _data_bytes, false);

	memset(_data_bufer_ptr, 0, _data_bytes);

	CAutoLock this_lock(this, __FUNCTION__);

	IF_NULL_ASSERT_RETURN_VALUE(m_msg_info_ptr, false);

	memcpy_s(_data_bufer_ptr, _data_bytes, 
		m_msg_info_ptr->msg_data, m_msg_info_ptr->msg_header.msg_data_bytes);

	return true;
}


bool ITinySocketMessage::CImpl::Initialize(WORD _local_port, bool _is_udp)
{
	CAutoLock class_lock(NULL, __FUNCTION__);

	if (true == _is_udp && false == sm_udp_socket.IsValid())
	{
		sm_udp_socket.SetPacketDispatcher(CImpl::DispatchPacket);
		return sm_udp_socket.Create(_local_port, _is_udp);
	}

	if (false == _is_udp && false == sm_tcp_socket.IsValid())
	{
		sm_tcp_socket.SetPacketDispatcher(CImpl::DispatchPacket);
		return sm_tcp_socket.Create(_local_port, _is_udp);
	}

	return true;
}


bool CALLBACK ITinySocketMessage::CImpl::DispatchPacket(
	DWORD _remote_ip_addr,
	const BYTE* _packet_data_ptr,
	WORD _data_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_remote_ip_addr, false);
	IF_NULL_ASSERT_RETURN_VALUE(_packet_data_ptr, false);

	CAutoLock class_lock(NULL, __FUNCTION__);

	const MsgHeader* const msg_header_ptr 
		= reinterpret_cast<const MsgHeader*>(_packet_data_ptr);
	const BYTE* const msg_data_ptr = _packet_data_ptr + sizeof(MsgHeader);

	for (std::pair<Messages::iterator, Messages::iterator> range 
			= sm_messages.equal_range(msg_header_ptr->msg_id);
		 range.first != range.second; ++(range.first))
	{
		CImpl* msg_obj_ptr = range.first->second;
		IF_NULL_ASSERT(msg_obj_ptr)
		{
			continue;
		}

		IF_NULL_ASSERT (msg_obj_ptr->m_parent)
		{
			continue;
		}

		msg_obj_ptr->m_parent->HandleEchoMsg(_remote_ip_addr, *msg_header_ptr, 
			msg_data_ptr, msg_header_ptr->msg_data_bytes);
		break;
	}

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		IMessageListener* listener_ptr = *it;
		IF_NULL_ASSERT (listener_ptr)
		{
			continue;
		}

		if (false == listener_ptr->IsMessageNeedToHandle(msg_header_ptr->msg_id))
		{
			continue;
		}

		listener_ptr->HandleMessage(_remote_ip_addr, *msg_header_ptr,
			msg_data_ptr, msg_header_ptr->msg_data_bytes);
	}

	return true;
}


bool ITinySocketMessage::CImpl::RegisterListener(IMessageListener* _listener_ptr)
{
	CAutoLock class_lock(NULL, __FUNCTION__);

	IF_NULL_ASSERT_RETURN_VALUE(_listener_ptr, false);

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		IF_FALSE_ASSERT_RETURN_VALUE(_listener_ptr != *it, false);
	}

	sm_listeners.push_back(_listener_ptr);

	return true;
}


bool ITinySocketMessage::CImpl::UnregisterListener(IMessageListener* _listener_ptr)
{
	CAutoLock class_lock(NULL, __FUNCTION__);

	IF_NULL_ASSERT_RETURN_VALUE(_listener_ptr, false);

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		if (_listener_ptr == *it)
		{
			sm_listeners.erase(it);
			return true;
		}
	}

	ASSERT(false);
	return false;
}


ITinySocketMessage::ITinySocketMessage(
	WORD _send_msg_id,
	WORD _echo_msg_id, 
	WORD _version
	) : m_impl_ptr(new CImpl(_send_msg_id, _echo_msg_id, _version))
{
	m_impl_ptr->m_parent = this;
}


ITinySocketMessage::~ITinySocketMessage(void)
{
	SAFE_DELETE(static_cast<CImpl*>(m_impl_ptr));
}


bool ITinySocketMessage::CreateSendMsg(
	const BYTE* _msg_data_ptr, 
	WORD _data_bytes
	)
{
	return m_impl_ptr->Create(_msg_data_ptr, _data_bytes);
}


bool ITinySocketMessage::IsValid(void) const
{
	return m_impl_ptr->m_is_valid;
}


bool ITinySocketMessage::InitializeUdp(WORD _local_port)
{
	return CImpl::Initialize(_local_port, true);
}


int ITinySocketMessage::SendMsg(
	LPCTSTR _remote_addr_str, 
	WORD _reomte_port, 
	bool _is_udp
	) const
{
	return m_impl_ptr->Send(_remote_addr_str, _reomte_port, _is_udp);
}


WORD ITinySocketMessage::GetSendId(void) const
{
	return m_impl_ptr->m_send_msg_id;
}


WORD ITinySocketMessage::GetEchoId(void) const
{
	return m_impl_ptr->m_echo_msg_id;
}


WORD ITinySocketMessage::GetSize(void) const
{
	return m_impl_ptr->GetSize();
}


const BYTE* ITinySocketMessage::GetData(void) const
{
	return m_impl_ptr->GetData();
}


bool ITinySocketMessage::GetData(
	OUT PBYTE _data_bufer_ptr, 
	OUT size_t& _data_bytes
	) const
{
	return m_impl_ptr->GetData(_data_bufer_ptr, _data_bytes);
}


bool ITinySocketMessage::RegisterListener(IMessageListener* _listener_ptr)
{
	return CImpl::RegisterListener(_listener_ptr);
}


bool ITinySocketMessage::UnregisterListener(IMessageListener* _listener_ptr)
{
	return CImpl::UnregisterListener(_listener_ptr);
}


IMessageListener::IMessageListener(void)
{
	ITinySocketMessage::RegisterListener(this);
}


IMessageListener::~IMessageListener(void)
{
	ITinySocketMessage::UnregisterListener(this);
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
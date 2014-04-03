/*******************************************************************************
 * 文件：	interfacess_of_tiny_socket.cpp
 * 描述：	参见 interfacess_of_tiny_socket.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-08
 ******************************************************************************/

#include "stdafx.h"
#include "tiny_socket_msg.h"
#include "tiny_socket.h"
#include "utility.h"
#include "w2x_common\common.h"
#include "w2x_common\encode.h"
#include "w2x_common\log.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)


class ITinySocketMsg::CImpl
{
	//typedef std::map<DWORD, internal::W2X_IP_ADDR_STR>	IpAddrStrMap;
	typedef ITinySocketMsg::MsgInfo				MsgInfo;
	typedef std::multimap<WORD, CImpl*>			Messages;
	typedef std::list<IMsgListener*>			Listeners;
	
public:
	CImpl(WORD _send_msg_id, WORD _recv_msg_id, BYTE _version);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)
W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)

public:
	bool Create(
		const BYTE* _msg_data_ptr, 
		DWORD _data_bytes,
		DWORD _fill_bytes,
		BYTE _filler
	);

	inline bool Send(
		LPCTSTR _remote_addr_str, 
		WORD _remote_port, 
		bool _is_udp);

	inline DWORD GetDataSize(void);

	inline const BYTE* GetData(void);

	inline bool CopyData(
		OUT PBYTE _data_bufer_ptr,
		OUT size_t& _data_bytes
	);

	static bool Initialize(
		WORD _local_port, 
		bool _is_udp,
		LPCTSTR _mcast_addr_str, 
		WORD _mcast_port
		);

	static bool CALLBACK DispatchRecvPacket(
		DWORD _remote_ip_addr,
		WORD _remote_port,
		const BYTE* _packet_data_ptr,
		WORD _data_bytes
	);

	static bool CALLBACK DispatchSendPacket(
		DWORD _remote_ip_addr,
		WORD _remote_port,
		const BYTE* _packet_data_ptr,
		WORD _data_bytes
	);

	static bool RegisterListener(IMsgListener* _listener_ptr);

	static bool UnregisterListener(IMsgListener* _listener_ptr);

	static void SetAuthCode(WORD dwCode);

	static void FilterOutLoopback(bool _is_filter_out);

public:
	WORD	 m_send_msg_id;
	WORD	 m_recv_msg_id;
	BYTE	 m_version;
	bool	 m_is_valid;
	MsgInfo* m_send_msg_ptr;
	ITinySocketMsg* m_parent;

	static bool			sm_filter_out_loopback;
	static WORD			sm_auth_code;
	static CTinySocket	sm_udp_socket;
	static CTinySocket	sm_tcp_socket;
	static Messages		sm_messages;
	static Listeners	sm_listeners;
};

W2X_IMPLEMENT_LOCKING_CLASS(ITinySocketMsg::CImpl, CAutoLock)

bool ITinySocketMsg::CImpl::sm_filter_out_loopback = false;
WORD ITinySocketMsg::CImpl::sm_auth_code = 0x0000;
CTinySocket ITinySocketMsg::CImpl::sm_udp_socket;
CTinySocket ITinySocketMsg::CImpl::sm_tcp_socket;
ITinySocketMsg::CImpl::Listeners ITinySocketMsg::CImpl::sm_listeners;
ITinySocketMsg::CImpl::Messages	 ITinySocketMsg::CImpl::sm_messages;


ITinySocketMsg::CImpl::CImpl(
	WORD _send_msg_id, 
	WORD _recv_msg_id, 
	BYTE _version
	)
	: m_send_msg_id(_send_msg_id)
	, m_recv_msg_id(_recv_msg_id)
	, m_version(_version)
	, m_is_valid(false)
	, m_send_msg_ptr(NULL)
	, m_parent(NULL)
{
	ASSERT(INVALID_MSG_ID != m_send_msg_id);

	if (INVALID_MSG_ID != m_recv_msg_id)
	{
		CAutoLock class_lock(NULL);
		sm_messages.insert(std::pair<WORD, CImpl*>(m_recv_msg_id, this));
	}
}


ITinySocketMsg::CImpl::~CImpl(void)
{
	if (INVALID_MSG_ID != m_recv_msg_id)
	{
		CAutoLock class_lock(NULL);

		for (std::pair<Messages::iterator, Messages::iterator> range 
				= sm_messages.equal_range(m_recv_msg_id);
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

	SAFE_DELETE(m_send_msg_ptr);
}


bool ITinySocketMsg::CImpl::Create(
	const BYTE* _msg_data_ptr, 
	DWORD _data_bytes,
	DWORD _fill_bytes,
	BYTE _filler
	)
{
	IF_FALSE_ASSERT (INVALID_MSG_ID != m_send_msg_id)
	{
		return m_is_valid = false;
	}

	if (NULL != _msg_data_ptr)
	{
		IF_FALSE_ASSERT (0 < _data_bytes + _fill_bytes  && 
			_data_bytes + _fill_bytes <= MAX_MSG_DATA)
		{
			return m_is_valid = false;
		}
	}
	
	if (NULL == m_send_msg_ptr)
	{
		m_send_msg_ptr = new MsgInfo;
		IF_NULL_ASSERT (m_send_msg_ptr)
		{
			return m_is_valid = false;
		}
	}

	MsgHeader msg_header = {0};
	msg_header.header_bytes = sizeof(MsgHeader);
	msg_header.auth_code = sm_auth_code;
	msg_header.version = m_version;
	msg_header.msg_id = m_send_msg_id;
	msg_header.data_bytes = _data_bytes + _fill_bytes;
	msg_header.check_sum = 0;
	
	memset(m_send_msg_ptr, 0, sizeof(MsgInfo));
	memcpy(&m_send_msg_ptr->msg_header, &msg_header, sizeof(MsgHeader));
	if (NULL != _msg_data_ptr)
	{
		memcpy(&m_send_msg_ptr->msg_data, _msg_data_ptr, _data_bytes);
	}
	if (0 < _fill_bytes)
	{
		memset((m_send_msg_ptr->msg_data + _data_bytes), _filler, _fill_bytes);
	}
	
	return m_is_valid = true;
}


inline bool ITinySocketMsg::CImpl::Send(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	bool _is_udp
	)
{
	IF_FALSE_ASSERT_RETURN_VALUE(m_is_valid, false);
	IF_FALSE_ASSERT_RETURN_VALUE(0 != _remote_port, false);

	if (NULL == _remote_addr_str || true == _is_udp)
	{
		IF_FALSE_ASSERT_RETURN_VALUE(sm_udp_socket.IsValid(), false);

		return sm_udp_socket.SendPacket(_remote_addr_str, 
			_remote_port, reinterpret_cast<const BYTE*>(m_send_msg_ptr), 
			m_send_msg_ptr->msg_header.data_bytes + sizeof(MsgHeader));
	}
	else
	{
		IF_FALSE_ASSERT_RETURN_VALUE(sm_tcp_socket.IsValid(), false);

// 		return CImpl::sm_tcp_socket->SendUdpPacket(
// 			_remote_addr_str, _remote_port,
// 			m_msg_info_ptr->data_buffer, m_msg_info_ptr->data_bytes);
	}

	return false;
}


inline DWORD ITinySocketMsg::CImpl::GetDataSize(void)
{
	IF_NULL_ASSERT_RETURN_VALUE(m_send_msg_ptr, 0);

	return m_send_msg_ptr->msg_header.data_bytes;
}


inline const BYTE* ITinySocketMsg::CImpl::GetData(void)
{
	IF_NULL_ASSERT_RETURN_VALUE(m_send_msg_ptr, NULL);

	return m_send_msg_ptr->msg_data;
}


inline bool ITinySocketMsg::CImpl::CopyData(
	OUT PBYTE _data_bufer_ptr, 
	OUT size_t& _data_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_data_bufer_ptr, false);
	IF_FALSE_ASSERT_RETURN_VALUE(0 < _data_bytes, false);

	memset(_data_bufer_ptr, 0, _data_bytes);

	IF_NULL_ASSERT_RETURN_VALUE(m_send_msg_ptr, false);

	memcpy_s(_data_bufer_ptr, _data_bytes, 
		m_send_msg_ptr->msg_data, m_send_msg_ptr->msg_header.data_bytes);

	return true;
}


bool ITinySocketMsg::CImpl::Initialize(
	WORD _local_port, 
	bool _is_udp, 
	LPCTSTR _mcast_addr_str, 
	WORD _mcast_port
	)
{
	CAutoLock class_lock(NULL);

	bool successed = true;
	LPCTSTR socket_type = _is_udp ? TEXT("UDP") : TEXT("TCP");
	log::LogInfo(TEXT("[TinySocketMsg] %s initializing..."), socket_type);

	if (true == _is_udp && false == sm_udp_socket.IsValid())
	{
		sm_udp_socket.SetRecvDispatcher(CImpl::DispatchRecvPacket);
		sm_udp_socket.SetSendDispatcher(CImpl::DispatchSendPacket);
		successed = sm_udp_socket.CreateUdp(_local_port, _mcast_addr_str, _mcast_port);
	}
	else if (false == _is_udp && false == sm_tcp_socket.IsValid())
	{
		sm_udp_socket.SetRecvDispatcher(CImpl::DispatchRecvPacket);
		sm_udp_socket.SetSendDispatcher(CImpl::DispatchSendPacket);
		//successed = sm_tcp_socket.Create(_local_port, _is_udp);
	}
	else
	{
		log::LogInfo(TEXT("[TinySocketMsg] %s initialized."), socket_type);
		return true;
	}

	log::LogInfo(TEXT("[TinySocketMsg] %s initialization %s."), 
		socket_type, successed ? TEXT("successed") : TEXT("failed"));
	
	return successed;
}


bool CALLBACK ITinySocketMsg::CImpl::DispatchRecvPacket(
	DWORD _remote_ip_addr,
	WORD _remote_port,
	const BYTE* _packet_data_ptr,
	WORD _data_bytes
	)
{
	static DWORD s_work_ip_addr = 0;
	if (0 != s_work_ip_addr || true == internal::GetWorkIpAddr(s_work_ip_addr))
	{
		if (_remote_ip_addr == s_work_ip_addr) 
		{
			// 过来掉本地回送消息
			if (true == sm_filter_out_loopback)
			{
				log::LogInfo(TEXT("Filter out the loopback msg."));
				return true;
			}
			log::LogInfo(TEXT("Loopback msg."));
		}
	}

	IF_NULL_ASSERT_RETURN_VALUE(_packet_data_ptr, false);

	const MsgHeader* const msg_header_ptr 
		= reinterpret_cast<const MsgHeader*>(_packet_data_ptr);

	// 比较协议验证码是否一致
	if (msg_header_ptr->auth_code != sm_auth_code)
	{
		log::LogInfo(
			TEXT("[TinySocketMsg] Invalid protocol auth code, ")
			TEXT("must be 0x%04x instead of 0x%04x."),
			sm_auth_code, msg_header_ptr->auth_code);
		return false;
	}

	// 比较消息协议头长度是否一致
	static const size_t s_msg_header_bytes = sizeof(MsgHeader);
	if (msg_header_ptr->header_bytes != s_msg_header_bytes)
	{
		log::LogWarn(
			TEXT("[TinySocketMsg] Do not match the length of the message header, ")
			TEXT("local is %d bytes but the remote is %d bytes."),
			s_msg_header_bytes, msg_header_ptr->header_bytes);
	}

	// 校验 CRC16
	if (0 != msg_header_ptr->check_sum)
	{
		// 如果校验失败则返回
	}

	static WORD s_remote_port_in_host = 0;
	static internal::W2X_IP_ADDR_STR s_remote_ip_addr_str = {0};
	s_remote_port_in_host = ::ntohs(_remote_port);
	internal::IpAddr2Str(s_remote_ip_addr_str, _remote_ip_addr);
	log::LogInfo(
		TEXT("[TinySocketMsg] The msg header checking has been pass, from %s:%d."),
		s_remote_ip_addr_str.str, s_remote_port_in_host);

	bool is_handled = false;
	const BYTE* const msg_data_ptr = 
		0 == msg_header_ptr->data_bytes 
		? NULL : _packet_data_ptr + msg_header_ptr->header_bytes;

	CAutoLock class_lock(NULL);

	// 查找默认消息处理器
	for (std::pair<Messages::iterator, Messages::iterator> range 
			= sm_messages.equal_range(msg_header_ptr->msg_id);
		 range.first != range.second; ++(range.first))
	{
		CImpl* msg_obj_ptr = range.first->second;
		IF_NULL_ASSERT (msg_obj_ptr)
		{
			continue;
		}
		IF_NULL_ASSERT (msg_obj_ptr->m_parent)
		{
			continue;
		}

		msg_obj_ptr->m_parent->HandleRecvMsg(
			s_remote_ip_addr_str.str, s_remote_port_in_host,
			*msg_header_ptr, msg_data_ptr, msg_header_ptr->data_bytes);
		is_handled = true;
	}

	// 没找到默认消息处理器
	if (false == is_handled)
	{
		log::LogInfo(TEXT("[TinySocketMsg] Not found default handler!"));
	}
	is_handled = true;

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		IMsgListener* listener_ptr = *it;
		IF_NULL_ASSERT (listener_ptr)
		{
			continue;
		}
		if (false == listener_ptr->IsMsgNeedToHandle(msg_header_ptr->msg_id))
		{
			continue;
		}

		log::LogInfo(TEXT("[TinySocketMsg] Pass msg to listener!"));
		listener_ptr->HandleRecvMsg(_remote_ip_addr, _remote_port, 
			*msg_header_ptr, msg_data_ptr, msg_header_ptr->data_bytes);
	}

	// 没找到默认消息处理器
	if (false == is_handled)
	{
		log::LogInfo(TEXT("[TinySocketMsg] Not found msg handler!"));
	}

	return true;
}


bool CALLBACK ITinySocketMsg::CImpl::DispatchSendPacket(
	DWORD _remote_ip_addr,
	WORD _remote_port,
	const BYTE* _packet_data_ptr,
	WORD _data_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_packet_data_ptr, false);

	const MsgHeader* const msg_header_ptr 
		= reinterpret_cast<const MsgHeader*>(_packet_data_ptr);

	static WORD s_remote_port_in_host = 0;
	static internal::W2X_IP_ADDR_STR s_remote_ip_addr_str = {0};
	s_remote_port_in_host = ::ntohs(_remote_port);
	internal::IpAddr2Str(s_remote_ip_addr_str, _remote_ip_addr);

	bool is_handled = false;
	const BYTE* const msg_data_ptr = 
		0 == msg_header_ptr->data_bytes 
		? NULL : _packet_data_ptr + msg_header_ptr->header_bytes;

	CAutoLock class_lock(NULL);

	// 查找默认消息处理器
	for (std::pair<Messages::iterator, Messages::iterator> range 
		= sm_messages.equal_range(msg_header_ptr->msg_id);
		range.first != range.second; ++(range.first))
	{
		CImpl* msg_obj_ptr = range.first->second;
		IF_NULL_ASSERT (msg_obj_ptr)
		{
			continue;
		}
		IF_NULL_ASSERT (msg_obj_ptr->m_parent)
		{
			continue;
		}

		msg_obj_ptr->m_parent->HandleSendMsg(
			s_remote_ip_addr_str.str, s_remote_port_in_host,
			*msg_header_ptr, msg_data_ptr, msg_header_ptr->data_bytes);
		is_handled = true;
	}

	// 没找到默认消息处理器
	if (false == is_handled)
	{
		log::LogInfo(TEXT("[TinySocketMsg] Not found default handler!"));
	}
	is_handled = true;

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		IMsgListener* listener_ptr = *it;
		IF_NULL_ASSERT (listener_ptr)
		{
			continue;
		}
		if (false == listener_ptr->IsMsgNeedToHandle(msg_header_ptr->msg_id))
		{
			continue;
		}

		log::LogInfo(TEXT("[TinySocketMsg] Pass msg to listener!"));
		listener_ptr->HandleSendMsg(_remote_ip_addr, _remote_port, 
			*msg_header_ptr, msg_data_ptr, msg_header_ptr->data_bytes);
	}

	// 没找到默认消息处理器
	if (false == is_handled)
	{
		log::LogInfo(TEXT("[TinySocketMsg] Not found msg handler!"));
	}

	return true;
}


bool ITinySocketMsg::CImpl::RegisterListener(IMsgListener* _listener_ptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_listener_ptr, false);

	CAutoLock class_lock(NULL);

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		IF_FALSE_ASSERT_RETURN_VALUE(_listener_ptr != *it, false);
	}

	sm_listeners.push_back(_listener_ptr);

	return true;
}


bool ITinySocketMsg::CImpl::UnregisterListener(IMsgListener* _listener_ptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_listener_ptr, false);

	CAutoLock class_lock(NULL);

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


void ITinySocketMsg::CImpl::SetAuthCode(WORD dwCode)
{
	sm_auth_code = dwCode;
}


void ITinySocketMsg::CImpl::FilterOutLoopback(bool _is_filter_out)
{
	sm_filter_out_loopback = _is_filter_out;
}


ITinySocketMsg::ITinySocketMsg(
	WORD _send_msg_id,
	WORD _echo_msg_id, 
	BYTE _version
	) : m_impl_ptr(new CImpl(_send_msg_id, _echo_msg_id, _version))
{
	m_impl_ptr->m_parent = this;
}


ITinySocketMsg::~ITinySocketMsg(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool ITinySocketMsg::CreateMsg(
	const BYTE* _msg_data_ptr, 
	DWORD _data_bytes,
	DWORD _fill_bytes,
	BYTE _filler
	)
{
	return m_impl_ptr->Create(_msg_data_ptr, _data_bytes, _fill_bytes, _filler);
}


bool ITinySocketMsg::CreateUtf8Msg(
	LPCTSTR _t_str_ptr,
	DWORD _fill_bytes,
	BYTE _filler
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_t_str_ptr, false);

	int cntrl_char_count = 0;
	for(size_t i = _tcslen(_t_str_ptr); 0 < i; ++i)
	{
		int ch = _t_str_ptr[i - 1];
		if (0x80 > ch && iscntrl(ch)) {
			++cntrl_char_count;
			continue;
		}
		break;
	}

	LPSTR utf8_ptr = W2X_ALLOC_STR_T2UTF(_t_str_ptr);

	const size_t str_len = strlen(utf8_ptr);
	IF_FALSE_ASSERT_RETURN_VALUE(0 < str_len, false);

	bool is_successed = m_impl_ptr->Create(
		reinterpret_cast<const BYTE*>(utf8_ptr), 
		static_cast<DWORD>(str_len - cntrl_char_count), 
		_fill_bytes, _filler);

	W2X_FREE_STR_A(utf8_ptr);

	return true;
}


bool ITinySocketMsg::IsValid(void) const
{
	return m_impl_ptr->m_is_valid;
}


bool ITinySocketMsg::InitUdp(
	WORD _local_port,
	LPCTSTR _mcast_addr_str, 
	WORD _mcast_port
	)
{
	return CImpl::Initialize(_local_port, true, _mcast_addr_str, _mcast_port);
}


bool ITinySocketMsg::SendMsg(
	LPCTSTR _remote_addr_str, 
	WORD _reomte_port, 
	bool _is_udp
	) const
{
	return m_impl_ptr->Send(_remote_addr_str, _reomte_port, _is_udp);
}


WORD ITinySocketMsg::GetSendId(void) const
{
	return m_impl_ptr->m_send_msg_id;
}


WORD ITinySocketMsg::GetRecvId(void) const
{
	return m_impl_ptr->m_recv_msg_id;
}


DWORD ITinySocketMsg::GetDataSize(void) const
{
	return m_impl_ptr->GetDataSize();
}


const BYTE* ITinySocketMsg::GetData(void) const
{
	return m_impl_ptr->GetData();
}


bool ITinySocketMsg::CopyData(
	OUT PBYTE _data_bufer_ptr, 
	OUT size_t& _data_bytes
	) const
{
	return m_impl_ptr->CopyData(_data_bufer_ptr, _data_bytes);
}


bool ITinySocketMsg::RegisterListener(IMsgListener* _listener_ptr)
{
	return CImpl::RegisterListener(_listener_ptr);
}


bool ITinySocketMsg::UnregisterListener(IMsgListener* _listener_ptr)
{
	return CImpl::UnregisterListener(_listener_ptr);
}


void ITinySocketMsg::SetAuthCode(WORD dwCode)
{
	return CImpl::SetAuthCode(dwCode);
}


void ITinySocketMsg::FilterOutLoopback(bool _is_filter_out)
{
	return CImpl::FilterOutLoopback(_is_filter_out);
}


IMsgListener::IMsgListener(void)
{
	ITinySocketMsg::RegisterListener(this);
}


IMsgListener::~IMsgListener(void)
{
	ITinySocketMsg::UnregisterListener(this);
}


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END
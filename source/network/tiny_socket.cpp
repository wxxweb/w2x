/*******************************************************************************
 * �ļ���	tiny_socket.cpp
 * ������	�μ� tiny_socket.h
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-02-05
 ******************************************************************************/

#include "stdafx.h"
#include "..\common\common.h"
#include "tiny_socket.h"
#include "interfaces_of_tiny_socket.h"
#include "utility.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class CTinySocket::CImpl
{
	/*
	 * ��ŷַ���
	 */
	typedef std::list<ITinySocketListener*> Listeners;

	/*
	 * �ýṹ�����첽 Socket �������Ϣ���ݸ��ص�������
	 * wsa_overlapped ���������λ���������ĵ�ַ�õ�
	 * ���� SocketInfo �����ַ��
	 */
	struct SocketInfo {
		WSAOVERLAPPED	wsa_overlapped;
		CImpl*			this_ptr;
	};

public:
	CImpl(void);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)
W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)

public:
	static bool Initialize(void);

	static bool Uninitialize(void);

	static bool RegisterListener(ITinySocketListener* _listener_ptr);

	static bool UnregisterListener(ITinySocketListener* _listener_ptr);

	static bool SetPacketParser(CTinySocket::FPacketParser _packet_parser_fn_ptr);

	bool CreateUdp(WORD _local_port);

	bool Destory(void);
	
	int SyncRecvUdpPacket(
		OUT	PSOCKADDR_IN _remote_addr_ptr,
		OUT	PBYTE _packet_buffer,
		const DWORD _size_in_bytes
	);

	ERecvStatus RecvUdpPacket(void);

	int SendUdpPacket(
		LPCTSTR _remote_addr_str,
		WORD _remote_port,
		const BYTE* _packet_buffer,
		DWORD _size_in_bytes
	);

	bool EnableBroadcast(bool _is_enable);

	bool WaitRecvPacket(DWORD _timeout);

	inline bool IsBroadcastEnable(void) const;

	inline size_t SetRecvMsgBytes(size_t _bytes);

	// Ϊ���յ�����Ϣ���ý�����
	inline bool SetRecvMsgEnd(void);

	// �����յ�����Ϣ��ӵ���Ϣ������
	inline bool AddToRecvMsgLoop(void);

private:
	bool BindLocalAddress(WORD _local_port);

	/*
	 * �̺߳����������첽�ص� UDP ��������
	 */
	static DWORD CALLBACK RecvUdpPacketThread(PVOID _param);

	/*
	 * ������յ�����Ϣ��
	 */
	static bool CALLBACK HandleRecvMsg(
		PVOID _handler_param,	// ����������
		LPCTSTR _msg,			// ��Ϣ�����������������ݣ�
		size_t _bytes,			// ��Ϣ�ֽ���
		PVOID _msg_param		// ��Ϣ����
	);	

	/*
	 * ���첽����������ɺ󣬱� WSARecvFrom() �ص���
	 */
	static void CALLBACK HandleRecvCompletion(
		IN DWORD _error_code,
		IN DWORD _bytes_transferred,
		IN LPWSAOVERLAPPED _overlapped_ptr,
		IN DWORD _flags
	);

private:
	static bool			 sm_is_initalized;
	static size_t		 sm_recv_msg_count;
	static CMsgLoop		 sm_recv_msg_loop;
	static Listeners	 sm_listeners;
	static FPacketParser sm_packet_parser_fn_ptr;

	bool		m_is_broadcast_enable;
	SOCKET		m_local_socket;
	SOCKADDR_IN	m_remote_sock_addr;
	CHAR		m_recv_msg_buffer[MAX_MSG_BUF_SIZE];
	size_t		m_recv_msg_bytes;
	SocketInfo	m_socket_info;
	HANDLE		m_recv_thread_handle;
	HANDLE		m_recv_thread_exit_event;
	 
};

W2X_IMPLEMENT_LOCKING_CLASS(CTinySocket::CImpl, CAutoLock)


bool	    CTinySocket::CImpl::sm_is_initalized = false;
size_t	    CTinySocket::CImpl::sm_recv_msg_count = 0;
CMsgLoop    CTinySocket::CImpl::sm_recv_msg_loop;
CTinySocket::CImpl::Listeners CTinySocket::CImpl::sm_listeners;
CTinySocket::FPacketParser CTinySocket::CImpl::sm_packet_parser_fn_ptr = NULL;

CTinySocket::CImpl::CImpl(void)
	: m_local_socket(INVALID_SOCKET)
	, m_recv_msg_bytes(0)
	, m_recv_thread_handle(NULL)
	, m_recv_thread_exit_event(NULL)
	, m_is_broadcast_enable(false)
{
	memset(&m_remote_sock_addr, 0, sizeof(m_remote_sock_addr));
	memset(&m_socket_info, 0, sizeof(m_socket_info));
	memset(&m_recv_msg_buffer, 0, sizeof(m_recv_msg_buffer));
}


CTinySocket::CImpl::~CImpl(void)
{
	::WSASetEvent(m_socket_info.wsa_overlapped.hEvent);
	if (WAIT_TIMEOUT == ::WaitForSingleObject(m_recv_thread_handle, 5000))
	{
		::TerminateThread(m_recv_thread_handle, 0);
		m_recv_thread_handle = NULL;
	}
	this->Destory();
}


bool CTinySocket::CImpl::Initialize(void)
{
	CAutoLock class_lock(NULL);

	if (true == sm_is_initalized)
	{
		return true;
	}

	// ��ʼ��WinSock DLL 2.2

	w2x::log::LogInfo(TEXT("Initializing WinSock DLL 2.2 ..."));

	WORD version_requested = MAKEWORD(2, 2);
	WSADATA	wsa_data = {0};

	IF_FALSE_ASSERT (NO_ERROR == ::WSAStartup(version_requested, &wsa_data))
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Initialize WinSock DLL 2.2 faild. %d - %s."), 
			error_code, error_msg);
		return false;
	}

	sm_is_initalized = true;
	w2x::log::LogInfo(TEXT("Initialize WinSock DLL 2.2 successed."));

	sm_recv_msg_loop.StartLoopThread(CImpl::HandleRecvMsg, NULL);

	return true;
}


bool CTinySocket::CImpl::Uninitialize(void)
{
	CAutoLock class_lock(NULL);

	IF_FALSE_ASSERT_RETURN_VALUE (true == sm_is_initalized, false);

	sm_recv_msg_loop.StopLoopThread();

	w2x::log::LogInfo(TEXT("Uninitializing WinSock DLL 2.2 ..."));

	// �ͷ�WinSock DLL

	IF_FALSE_ASSERT (SOCKET_ERROR != ::WSACleanup())
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Uninitialize WinSock DLL 2.2 faild. %d - %s."), 
			error_code, error_msg);
		return false;
	}

	sm_is_initalized = false;

	w2x::log::LogInfo(TEXT("Uninitialize WinSock DLL 2.2 successed."));
	return true;
}


bool CTinySocket::CImpl::RegisterListener(ITinySocketListener* _listener_ptr)
{
	CAutoLock class_lock(NULL);

	IF_NULL_ASSERT_RETURN_VALUE(_listener_ptr, false);

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		IF_FALSE_ASSERT_RETURN_VALUE(_listener_ptr != *it, false);
	}

	sm_listeners.push_back(_listener_ptr);

	return true;
}


bool CTinySocket::CImpl::UnregisterListener(ITinySocketListener* _listener_ptr)
{
	CAutoLock class_lock(NULL);

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


bool CTinySocket::CImpl::SetPacketParser(
	CTinySocket::FPacketParser _packet_parser_fn_ptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_packet_parser_fn_ptr, false);

	CAutoLock class_lock(NULL);

	sm_packet_parser_fn_ptr = _packet_parser_fn_ptr;
	return true;
}


bool CTinySocket::CImpl::CreateUdp(WORD _local_port)
{
	CAutoLock this_lock(this);

	CImpl::Initialize();

	// �����Ѵ����� Socket
	this->Destory();

	// ���� Socket
	m_local_socket = ::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 
		NULL, 0, WSA_FLAG_OVERLAPPED);

	IF_FALSE_ASSERT (INVALID_SOCKET != m_local_socket)
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(TEXT("Create UDP socket faild. %d - %s."), 
			error_code, error_msg);

		return false;
	}

	// �󶨱��� IP ��ַ
	if (false == this->BindLocalAddress(_local_port))
	{
		return false;
	}

	// �����첽�ص����������߳�
	m_recv_thread_handle = ::CreateThread(
		NULL, 0, CImpl::RecvUdpPacketThread, 
		reinterpret_cast<LPVOID>(this), 0, NULL);

	m_recv_thread_exit_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	return true;
}


bool CTinySocket::CImpl::Destory(void)
{
	CAutoLock this_lock(this);

	if (INVALID_SOCKET == m_local_socket)
	{
		return false;
	}

	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, false);
	IF_FALSE_ASSERT_RETURN_VALUE(true == sm_is_initalized, false);

	w2x::log::LogInfo(TEXT("Destoring socket(%d) ..."), m_local_socket);

	bool is_successed = true;

	// ֹͣ�շ�����

	IF_FALSE_ASSERT (SOCKET_ERROR != ::shutdown(m_local_socket, SD_BOTH))
	{
		is_successed = false;

		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Shutdown socket(%d) faild. %d - %s."),
			m_local_socket, error_code, error_msg);
	}

	// �ر�Socket

	IF_FALSE_ASSERT (SOCKET_ERROR != ::closesocket(m_local_socket))
	{
		is_successed = false;

		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Close socket(%d) faild. %d - %s."),
			m_local_socket, error_code, error_msg);
	}

	m_local_socket = INVALID_SOCKET;

	w2x::log::LogInfo(
		TEXT("Destory socket(%d) %s."), m_local_socket, 
		(true == is_successed ? TEXT("successed") : TEXT("falid")));

	return is_successed;
}


bool CTinySocket::CImpl::BindLocalAddress(WORD _port)
{
	CAutoLock this_lock(this);

	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, false);

	// ���ñ��ص�ַ��Ϣ

	SOCKADDR_IN	sock_addr_in = {0};
	sock_addr_in.sin_family			= AF_INET;
	sock_addr_in.sin_addr.s_addr	= ::htonl(INADDR_ANY);	// �Զ�����������ַ
	sock_addr_in.sin_port			= ::htons(_port);		// ����˿ں�

	TCHAR ip_addr_str[MAX_IP_ADDR_STR] = TEXT("");
	internal::ParseIpAddressString(ip_addr_str, MAX_IP_ADDR_STR, 
		sock_addr_in.sin_addr.s_addr);
	w2x::log::LogInfo(
		TEXT("Binding a local address(%s) with the socket."), ip_addr_str);

	// �󶨱��ص�ַ�� Socket

	IF_FALSE_ASSERT (SOCKET_ERROR != ::bind(m_local_socket, 
		reinterpret_cast<PSOCKADDR>(&sock_addr_in), sizeof(SOCKADDR)))	
	{
		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Bind a local address(%s) with the socket faild. %d - %s."),
			ip_addr_str, error_code, error_msg);

		return false;
	}

	w2x::log::LogInfo(
		TEXT("Bind a local address(%s) with the socket successed."), ip_addr_str);

	return true;
}


int CTinySocket::CImpl::SyncRecvUdpPacket(
	OUT	PSOCKADDR_IN _remote_addr_ptr,
	OUT	PBYTE _packet_buffer,
	const DWORD _size_in_bytes
	)
{
	CAutoLock this_lock(this);

	IF_NULL_ASSERT_RETURN_VALUE(_remote_addr_ptr, SOCKET_ERROR);
	IF_NULL_ASSERT_RETURN_VALUE(_packet_buffer, SOCKET_ERROR);
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, SOCKET_ERROR);

	int remote_addr_bytes = sizeof(SOCKADDR);

	// ͬ������UDP���ݰ�

	const int bytes_received = ::recvfrom(m_local_socket, 
		reinterpret_cast<char*>(_packet_buffer),  _size_in_bytes, 0, 
		reinterpret_cast<PSOCKADDR>(_remote_addr_ptr), &remote_addr_bytes);

	IF_FALSE_ASSERT (SOCKET_ERROR != bytes_received)
	{
		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Sync receive UDP packet falid. %d - %s."),
			error_code, error_msg);

		return SOCKET_ERROR;
	}

	sm_recv_msg_loop.AddMsg(reinterpret_cast<LPCTSTR>(_packet_buffer), bytes_received);

	return bytes_received;
}


CTinySocket::ERecvStatus CTinySocket::CImpl::RecvUdpPacket(void)
{
	// ���ں����и��ȴ�ʱ�䣬�������ֱ���� CAutoLock this_lock(this) ������
	m_cs_of_this_by_CAutoLock.Enter();

	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, kAsyncRecvError);

	memset(&m_remote_sock_addr, 0, sizeof(m_remote_sock_addr));
	memset(&m_socket_info, 0, sizeof(SocketInfo)); // �ص��ṹ�������ÿգ���������
	m_socket_info.this_ptr = this;

	int	remote_addr_size = sizeof(SOCKADDR);	// Զ��������ַ�ṹ�ֽ���
	DWORD behavior_flags = 0;					// WSARecvFrom �� Flags ����
	WSABUF wsa_buf = {0};
	wsa_buf.buf = m_recv_msg_buffer;
	wsa_buf.len = sizeof(m_recv_msg_buffer);
	m_recv_msg_bytes = 0;

	const int recv_ret_val = ::WSARecvFrom(
		m_local_socket, &wsa_buf, 1, NULL, &behavior_flags, 
		reinterpret_cast<PSOCKADDR>(&m_remote_sock_addr),
		&remote_addr_size, &m_socket_info.wsa_overlapped,
		CImpl::HandleRecvCompletion);

	if (SOCKET_ERROR == recv_ret_val)
	{
		const DWORD error_code = ::WSAGetLastError();
		IF_FALSE_ASSERT (WSA_IO_PENDING == error_code)
		{
			// ���չ����г���
			TCHAR error_msg[MAX_PATH] = TEXT("");
			w2x::log::FormatError(error_msg, MAX_PATH, error_code);
			w2x::log::LogError(
				TEXT("Async receive UDP packet falid. %d - %s."),
				error_code, error_msg);

			this->Destory();

			// ���ں����и��ȴ�ʱ�䣬�������ֱ���� CAutoLock this_lock(this) ������
			m_cs_of_this_by_CAutoLock.Leave();

			return kAsyncRecvError;
		}
	}

	// ���ں����и��ȴ�ʱ�䣬�������ֱ���� CAutoLock this_lock(this) ������
	m_cs_of_this_by_CAutoLock.Leave();
	
	if (false == this->WaitRecvPacket(WSA_INFINITE))
	{
		return kAsyncRecvError;
	}

	return kAsyncRecvComplete;
}


bool CTinySocket::CImpl::WaitRecvPacket(DWORD _timeout)
{
	for (;;)
	{
		// �ȴ����ݽ�������¼�
		const DWORD wait_result = ::WSAWaitForMultipleEvents(1, 
			&m_recv_thread_exit_event, FALSE, _timeout, TRUE);

		if (WSA_WAIT_IO_COMPLETION == wait_result)
		{
			return true;
		}
		else if (WSA_WAIT_TIMEOUT == wait_result)
		{
			// �ȴ���ʱ����������
			continue;
		}
		else // ֻ��һ���¼��������ֻ֧������ WSA_WAIT_FAILED
		{
			// ������� cEvents �� lphEvents ���������Ƿ���Ч

			const DWORD error_code = ::WSAGetLastError();

			ASSERT(false);

			TCHAR error_msg[MAX_PATH] = TEXT("");
			w2x::log::FormatError(error_msg, MAX_PATH, error_code);
			w2x::log::LogError(
				TEXT("Wait for async receive UDP packet falid. %d - %s."),
				error_code, error_msg);

			this->Destory();

			return false;
		}
	}
}


int CTinySocket::CImpl::SendUdpPacket(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{
	CAutoLock this_lock(this);
	
	IF_NULL_ASSERT_RETURN_VALUE(_packet_buffer, SOCKET_ERROR);
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, SOCKET_ERROR);

	const bool is_broadcast = NULL == _remote_addr_str;
	
	DWORD ip_address = INADDR_ANY;
	if (true == is_broadcast)
	{
		if (false == internal::GetBroadcastIpAddress(ip_address))
		{
			return false;
		}
	}
	else if (false == internal::ParseHostToIpAddress(ip_address , _remote_addr_str))
	{
		return false;
	}

	this->EnableBroadcast(is_broadcast);

	DWORD bytes_sent = 0;
	WSABUF wsa_buf = {0};
	wsa_buf.buf = reinterpret_cast<CHAR*>(const_cast<BYTE*>(_packet_buffer));
	wsa_buf.len = _size_in_bytes;

	SOCKADDR_IN sock_addr_in = {0};
	sock_addr_in.sin_family = AF_INET;
	sock_addr_in.sin_addr.s_addr = ip_address;
	sock_addr_in.sin_port = htons(_remote_port);

	const int sent_result = ::WSASendTo(
		m_local_socket, &wsa_buf, 1, &bytes_sent, 0, 
		reinterpret_cast<PSOCKADDR>(&sock_addr_in), 
		sizeof(SOCKADDR), NULL, NULL);

	const DWORD error_code = ::WSAGetLastError();
	IF_FALSE_ASSERT (SOCKET_ERROR != sent_result)
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Send UDP packet falid. %d - %s."),
			error_code, error_msg);

		return SOCKET_ERROR;
	}

	return bytes_sent;
}


bool CTinySocket::CImpl::EnableBroadcast(bool _is_enable)
{
	CAutoLock this_lock(this);

	if (_is_enable == m_is_broadcast_enable)
	{
		return true;
	}

	// ����Socket�㲥����

	BOOL is_enable_broadcast = _is_enable ? TRUE : FALSE;
	IF_FALSE_ASSERT (SOCKET_ERROR != ::setsockopt(
		m_local_socket, SOL_SOCKET, SO_BROADCAST, 
		reinterpret_cast<const char*>(&is_enable_broadcast), sizeof(BOOL)))
	{
		// ����ʧ��

		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Enable socket to send broadcast data faild. %d - %s."),
			error_code, error_msg);

		return false;
	}
	
	m_is_broadcast_enable = _is_enable;

	return true;
}


inline bool CTinySocket::CImpl::IsBroadcastEnable(void) const
{
	return m_is_broadcast_enable;
}


inline size_t CTinySocket::CImpl::SetRecvMsgBytes(size_t _bytes)
{
	CAutoLock this_lock(this);

	return m_recv_msg_bytes = _bytes;
}


inline bool CTinySocket::CImpl::SetRecvMsgEnd(void)
{
	CAutoLock this_lock(this);

	IF_FALSE_ASSERT_RETURN_VALUE(0 < m_recv_msg_bytes, false);

	// ��� sizeof(TCHAR) ���ֽڵĽ����� 0

	const size_t buffer_bytes = sizeof(m_recv_msg_buffer);
	const size_t buffer_end_pos 
		= buffer_bytes - sizeof(TCHAR) > m_recv_msg_bytes
		? m_recv_msg_bytes : buffer_bytes - sizeof(TCHAR);
	m_recv_msg_buffer[buffer_end_pos] = 0;
	m_recv_msg_buffer[buffer_end_pos + 1] = 0;

	return true;
}


inline bool CTinySocket::CImpl::AddToRecvMsgLoop(void)
{
	CAutoLock this_lock(this);

	static size_t s_prev_recv_msg_count = sm_recv_msg_count++;

	IF_FALSE_ASSERT_RETURN_VALUE(s_prev_recv_msg_count < sm_recv_msg_count, false);
	s_prev_recv_msg_count = sm_recv_msg_count++;

	DWORD remote_ip_addr = m_remote_sock_addr.sin_addr.s_addr;
	return sm_recv_msg_loop.AddMsg(
		reinterpret_cast<LPCTSTR>(m_recv_msg_buffer), m_recv_msg_bytes, 
		reinterpret_cast<PVOID>(remote_ip_addr));
}


DWORD CALLBACK CTinySocket::CImpl::RecvUdpPacketThread(PVOID _param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_param, 1);

	CImpl* const this_ptr = reinterpret_cast<CImpl*>(_param);
	this_ptr->RecvUdpPacket();

	return 0;
}


bool CALLBACK CTinySocket::CImpl::HandleRecvMsg(
	PVOID _handler_param,
	LPCTSTR _msg,
	size_t _bytes,
	PVOID _msg_param
	)
{
	CAutoLock class_lock(NULL);

	IF_NULL_ASSERT_RETURN_VALUE(_msg, false);
	IF_NULL_ASSERT_RETURN_VALUE(_msg_param, false);

	const DWORD remote_ip_addr = reinterpret_cast<DWORD>(_msg_param);

	for (CImpl::Listeners::iterator it = sm_listeners.begin();
		sm_listeners.end() != it; ++it)
	{
		ITinySocketListener* listener_ptr = *it;
		IF_NULL_ASSERT (listener_ptr)
		{
			continue;
		}

		listener_ptr->HandleReceivedMessage(remote_ip_addr, 1, 
			reinterpret_cast<const BYTE*>(_msg), _bytes);

		continue;

		IF_NULL_ASSERT (sm_packet_parser_fn_ptr)
		{
			continue;
		}

		IF_FALSE_ASSERT (true == sm_packet_parser_fn_ptr(listener_ptr, 
			reinterpret_cast<const BYTE*>(_msg), _bytes, remote_ip_addr))
		{
			continue;
		}
	}

	return true;
}


void CALLBACK CTinySocket::CImpl::HandleRecvCompletion(
	IN DWORD _error_code,
	IN DWORD _bytes_transferred,
	IN LPWSAOVERLAPPED _overlapped_ptr,
	IN DWORD _flags
	)
{
	IF_NULL_ASSERT_RETURN(_overlapped_ptr);
	UNUSED(_flags);

	// SocketInfo �ĵ�һ���ֶ�����Ϊ WSAOVERLAPPED����� SocketInfo �����
	// ��һ���ֶ� wsa_overlapped �ĵ�ַ��Ϊ SocketInfo ����ĵ�ַ���˴�����ǿ
	// ��ת�����൱��ͨ�� LPWSAOVERLAPPED ������ SocketInfo ָ�롣
	CImpl::SocketInfo* const socket_info_ptr 
		= reinterpret_cast<CImpl::SocketInfo*>(_overlapped_ptr);

	CImpl* const this_ptr = socket_info_ptr->this_ptr;

	// ���������������жϣ��Ͽ���Զ�̷��������ӣ������ٴ�����
	if (NO_ERROR != _error_code || 0 == _bytes_transferred)
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, _error_code);
		LPCTSTR desc 
			= (NO_ERROR != _error_code) 
			? TEXT("Socket I/O operation faild")
			: TEXT("Socket closed");
		w2x::log::LogError(TEXT("%s. %d - %s."), desc, _error_code, error_msg);

		return;
	}

	this_ptr->SetRecvMsgBytes(_bytes_transferred);
	this_ptr->SetRecvMsgEnd();
	this_ptr->AddToRecvMsgLoop();
	this_ptr->RecvUdpPacket(); // �ٴ�Ͷ�ݽ�����������
}


CTinySocket::CTinySocket(void)
	: m_impl_ptr(new CImpl())
{

}


CTinySocket::~CTinySocket(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CTinySocket::Initialize(void)
{
	return CImpl::Initialize();
}


bool CTinySocket::Uninitialize(void)
{
	return CImpl::Uninitialize();
}


bool CTinySocket::RegisterListener(ITinySocketListener* _listener_ptr)
{
	return CImpl::RegisterListener(_listener_ptr);
}


bool CTinySocket::UnregisterListener(ITinySocketListener* _listener_ptr)
{
	return CImpl::UnregisterListener(_listener_ptr);
}


bool CTinySocket::CreateUdp(WORD _local_port)
{
	return m_impl_ptr->CreateUdp(_local_port);
}


bool CTinySocket::Destory(void)
{
	return m_impl_ptr->Destory();
}


int CTinySocket::SyncRecvUdpPacket(
	OUT	PSOCKADDR_IN _remote_addr_ptr,
	OUT	PBYTE _packet_buffer,
	const DWORD _size_in_bytes
	)
{
	return m_impl_ptr->SyncRecvUdpPacket(
		_remote_addr_ptr, _packet_buffer, _size_in_bytes);
}


CTinySocket::ERecvStatus CTinySocket::RecvUdpPacket(void)
{
	return m_impl_ptr->RecvUdpPacket();
}


int CTinySocket::SendUdpPacket(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{
	return m_impl_ptr->SendUdpPacket(
		_remote_addr_str, _remote_port, _packet_buffer, _size_in_bytes);
}


bool CTinySocket::EnableBroadcast(bool _is_enable)
{
	return m_impl_ptr->EnableBroadcast(_is_enable);
}


bool CTinySocket::IsBroadcastEnable(void) const
{
	return m_impl_ptr->IsBroadcastEnable();
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
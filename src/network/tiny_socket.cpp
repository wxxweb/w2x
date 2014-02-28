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
#include "utility.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class CTinySocket::CImpl
{
	enum {
		MAX_MSG_BUF_SIZE = 1024,
	};

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
	/* 
	 * ���̵��øú�����ʼ�� WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 * ֻ������һ�Σ������ظ����ò�����Ӱ�죬�ڲ��������������ظ���ʼ����
	 */
	static bool InitWinSock(void);

	/* 
	 * ���̵��øú����ͷ� WinSock DLL�����ɹ��򷵻� true, ���򷵻� false��
	 */
	static void UninitWinSock(void);

	/*
	 * ȷ�� WinSock DLL ����ʼ����
	 */
	inline void EnsureWinSock(void);

	bool Create(WORD _local_port, bool _is_udp);

	inline bool IsValid(void) const;

	bool Destory(void);

	int SendPacket(
		LPCTSTR _remote_addr_str,
		WORD _remote_port,
		const BYTE* _packet_buffer,
		DWORD _size_in_bytes
	);

	ERecvStatus RecvPacket(void);

	bool EnableBroadcast(bool _is_enable);

	inline bool IsBroadcastEnable(void) const;

	inline size_t SetRecvPacketBytes(size_t _bytes);

	// Ϊ���յ������ݰ����ý�����
	inline bool SetRecvPacketEnd(void);

	// �����յ�����Ϣ��ӵ���Ϣ������
	inline bool AddToMsgLoop(void);

	inline bool SetPacketDispatcher(FPacketDispatcher _dispatcher);

private:
	bool BindLocalAddress(WORD _local_port);

	bool WaitRecvPacket(DWORD _timeout);

	bool CreateRecvPacketThread(void);

	/*
	 * �̺߳����������첽�ص� UDP ��������
	 */
	static DWORD CALLBACK RecvPacketThread(PVOID _param);

	/*
	 * ������Ϣѭ���еĳ�����Ϣ��
	 */
	static bool CALLBACK HandlePopPacket(
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
	static bool			 sm_is_win_sock_init;

	bool		m_is_udp;
	bool		m_is_broadcast_enable;
	SOCKET		m_local_socket;
	SOCKADDR_IN	m_remote_sock_addr;
	CHAR		m_msg_buffer[MAX_MSG_BUF_SIZE];
	size_t		m_msg_bytes;
	SocketInfo	m_socket_info;
	size_t		m_packet_count;
	CMsgLoop	m_packet_loop;
	HANDLE		m_recv_thread_handle; 
	HANDLE		m_recv_thread_exit_event;

	FPacketDispatcher m_packet_dispatcher;
};

W2X_IMPLEMENT_LOCKING_CLASS(CTinySocket::CImpl, CAutoLock)


bool CTinySocket::CImpl::sm_is_win_sock_init = false;


CTinySocket::CImpl::CImpl(void)
	: m_local_socket(INVALID_SOCKET)
	, m_msg_bytes(0)
	, m_recv_thread_handle(NULL)
	, m_recv_thread_exit_event(NULL)
	, m_is_broadcast_enable(false)
	, m_is_udp(false)
	, m_packet_dispatcher(NULL)
{
	memset(&m_remote_sock_addr, 0, sizeof(m_remote_sock_addr));
	memset(&m_socket_info, 0, sizeof(m_socket_info));
	memset(&m_msg_buffer, 0, sizeof(m_msg_buffer));
}


CTinySocket::CImpl::~CImpl(void)
{
	if (NULL != m_recv_thread_exit_event)
	{
		::WSASetEvent(m_recv_thread_exit_event);
	}
	IF_FALSE_ASSERT (m_socket_info.wsa_overlapped.hEvent == m_recv_thread_exit_event)
	{
		::WSASetEvent(m_socket_info.wsa_overlapped.hEvent);
	}
	m_socket_info.wsa_overlapped.hEvent = m_recv_thread_exit_event = NULL;
	
	if (WAIT_TIMEOUT == ::WaitForSingleObject(m_recv_thread_handle, 5000))
	{
		::TerminateThread(m_recv_thread_handle, 0);
		m_recv_thread_handle = NULL;
	}
	this->Destory();
}


bool CTinySocket::CImpl::InitWinSock(void)
{
	CAutoLock class_lock(NULL, __FUNCTION__);

	if (true == sm_is_win_sock_init)
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

	sm_is_win_sock_init = true;
	w2x::log::LogInfo(TEXT("Initialize WinSock DLL 2.2 successed."));

	::atexit(CImpl::UninitWinSock);

	return true;
}


void CTinySocket::CImpl::UninitWinSock(void)
{
	CAutoLock class_lock(NULL, __FUNCTION__);

	IF_FALSE_ASSERT_RETURN(true == sm_is_win_sock_init);

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
		return;
	}

	sm_is_win_sock_init = false;

	w2x::log::LogInfo(TEXT("Uninitialize WinSock DLL 2.2 successed."));
}


inline void CTinySocket::CImpl::EnsureWinSock(void)
{
	if (true == sm_is_win_sock_init)
	{
		return;
	}

	CImpl::InitWinSock();
}


bool CTinySocket::CImpl::Create(WORD _local_port, bool _is_udp)
{
	CAutoLock this_lock(this, __FUNCTION__);

	this->EnsureWinSock();

	// �����Ѵ����� Socket
	this->Destory();

	m_is_udp = _is_udp;

	// ���� Socket
	m_local_socket = ::WSASocket(AF_INET, (m_is_udp ? SOCK_DGRAM : SOCK_STREAM), 
		(m_is_udp ? IPPROTO_UDP : IPPROTO_TCP), NULL, 0, WSA_FLAG_OVERLAPPED);

	IF_FALSE_ASSERT (INVALID_SOCKET != m_local_socket)
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(TEXT("Create %s socket faild. %d - %s."), 
			(m_is_udp ? TEXT("UDP") : TEXT("TCP")), error_code, error_msg);

		return false;
	}

	// �󶨱��� IP ��ַ
	if (false == this->BindLocalAddress(_local_port))
	{
		return false;
	}

	if (false == m_packet_loop.StartLoopThread(CImpl::HandlePopPacket, this))
	{
		return false;
	}

	// �����첽�ص����������߳�
	if (false == this->CreateRecvPacketThread())
	{
		return false;
	}
	
	return true;
}


inline bool CTinySocket::CImpl::IsValid(void) const
{
	return INVALID_SOCKET != m_local_socket;
}


bool CTinySocket::CImpl::Destory(void)
{
	CAutoLock this_lock(this, __FUNCTION__);

	if (INVALID_SOCKET == m_local_socket || false == sm_is_win_sock_init)
	{
		return false;
	}

	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, false);

	w2x::log::LogInfo(TEXT("Destoring socket(%d) ..."), m_local_socket);

	bool is_successed = true;

	// ֹͣ�շ�����

	if (SOCKET_ERROR == ::shutdown(m_local_socket, SD_BOTH))
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

	if (SOCKET_ERROR == ::closesocket(m_local_socket))
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

	m_packet_loop.StopLoopThread();

	return is_successed;
}


bool CTinySocket::CImpl::BindLocalAddress(WORD _port)
{
	CAutoLock this_lock(this, __FUNCTION__);

	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, false);

	// ���ñ��ص�ַ��Ϣ

	SOCKADDR_IN	sock_addr_in = {0};
	sock_addr_in.sin_family			= AF_INET;
	sock_addr_in.sin_addr.s_addr	= ::htonl(INADDR_ANY);	// �Զ�����������ַ
	sock_addr_in.sin_port			= ::htons(_port);		// ����˿ں�

	w2x::log::LogInfo(
		TEXT("Binding a local port(%d) with the socket."), _port);

	// �󶨱��ص�ַ�� Socket

	IF_FALSE_ASSERT (SOCKET_ERROR != ::bind(m_local_socket, 
		reinterpret_cast<PSOCKADDR>(&sock_addr_in), sizeof(SOCKADDR)))	
	{
		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Bind a local port(%d) with the socket faild. %d - %s."),
			_port, error_code, error_msg);

		this->Destory();

		return false;
	}

	w2x::log::LogInfo(
		TEXT("Bind a local port(%d) with the socket successed."), _port);

	return true;
}


CTinySocket::ERecvStatus CTinySocket::CImpl::RecvPacket(void)
{
	// ���ں����и��ȴ�ʱ�䣬�������ֱ���� CAutoLock this_lock(this) ������
	this->LockThis();

	IF_FALSE_ASSERT (INVALID_SOCKET != m_local_socket)
	{
		// ���ں����и��ȴ�ʱ�䣬�������ֱ���� CAutoLock this_lock(this) ������
		this->UnlockThis();

		return kAsyncRecvError;
	}

	if (NULL == m_recv_thread_exit_event)
	{
		m_recv_thread_exit_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	memset(&m_remote_sock_addr, 0, sizeof(m_remote_sock_addr));
	memset(&m_socket_info, 0, sizeof(SocketInfo)); // �ص��ṹ�������ÿգ���������
	m_socket_info.this_ptr = this;
	m_socket_info.wsa_overlapped.hEvent = m_recv_thread_exit_event;

	int	remote_addr_size = sizeof(SOCKADDR);	// Զ��������ַ�ṹ�ֽ���
	DWORD behavior_flags = 0;					// WSARecvFrom �� Flags ����
	WSABUF wsa_buf = {0};
	memset(m_msg_buffer, 0, sizeof(m_msg_buffer));
	wsa_buf.buf = m_msg_buffer;
	wsa_buf.len = sizeof(m_msg_buffer);
	m_msg_bytes = 0;

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
			this->UnlockThis();

			return kAsyncRecvError;
		}
	}

	// ���ں����и��ȴ�ʱ�䣬�������ֱ���� CAutoLock this_lock(this) ������
	this->UnlockThis();
	
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
			&m_socket_info.wsa_overlapped.hEvent, FALSE, _timeout, TRUE);

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

			//ASSERT(false);

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


bool CTinySocket::CImpl::CreateRecvPacketThread(void)
{
	m_recv_thread_handle = ::CreateThread(
		NULL, 0, CImpl::RecvPacketThread, 
		reinterpret_cast<LPVOID>(this), 0, NULL);
	IF_NULL_ASSERT (m_recv_thread_handle)
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::GetLastError();
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(TEXT("Create UDP socket receive thread faild. %d - %s."), 
			error_code, error_msg);

		this->Destory();

		return false;
	}

	return true;
}


int CTinySocket::CImpl::SendPacket(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{
	CAutoLock this_lock(this, __FUNCTION__);
	
	IF_NULL_ASSERT_RETURN_VALUE(_packet_buffer, SOCKET_ERROR);
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_local_socket, SOCKET_ERROR);

	IF_FALSE_ASSERT (!(false == m_is_udp && NULL == _remote_addr_str))
	{
		return SOCKET_ERROR;
	}

	const bool is_broadcast = NULL == _remote_addr_str;
	
	DWORD ip_address = INADDR_ANY;
	if (true == is_broadcast)
	{
		if (false == internal::GetBroadcastIpAddress(ip_address))
		{
			return false;
		}
		IF_FALSE_ASSERT_RETURN_VALUE(0 != ip_address, SOCKET_ERROR);
	}
	else if (false == internal::ParseHostToIpAddress(ip_address , _remote_addr_str))
	{
		return false;
	}

	static bool s_pre_broadcast_state = is_broadcast;
	if (true == m_is_udp && (true == is_broadcast || is_broadcast != s_pre_broadcast_state))
	{
		s_pre_broadcast_state = is_broadcast;
		this->EnableBroadcast(is_broadcast);
	}

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

	IF_FALSE_ASSERT (SOCKET_ERROR != sent_result)
	{
		const DWORD error_code = ::WSAGetLastError();
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
	CAutoLock this_lock(this, __FUNCTION__);

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


inline size_t CTinySocket::CImpl::SetRecvPacketBytes(size_t _bytes)
{
	CAutoLock this_lock(this, __FUNCTION__);

	return m_msg_bytes = _bytes;
}


inline bool CTinySocket::CImpl::SetRecvPacketEnd(void)
{
	CAutoLock this_lock(this, __FUNCTION__);

	IF_FALSE_ASSERT_RETURN_VALUE(0 < m_msg_bytes, false);

	// ��� sizeof(TCHAR) ���ֽڵĽ����� 0

	const size_t buffer_bytes = sizeof(m_msg_buffer);
	const size_t packet_end_pos 
		= buffer_bytes - sizeof(TCHAR) > m_msg_bytes
		? m_msg_bytes : buffer_bytes - sizeof(TCHAR);
	m_msg_buffer[packet_end_pos] = 0;
	m_msg_buffer[packet_end_pos + 1] = 0;

	return true;
}


inline bool CTinySocket::CImpl::AddToMsgLoop(void)
{
	CAutoLock this_lock(this, __FUNCTION__);

	static size_t s_prev_packet_count = m_packet_count++;

	IF_FALSE_ASSERT_RETURN_VALUE(s_prev_packet_count < m_packet_count, false);
	s_prev_packet_count = m_packet_count++;

	return m_packet_loop.AddMsg(
		reinterpret_cast<LPCTSTR>(m_msg_buffer), m_msg_bytes, 
		reinterpret_cast<PVOID>(m_remote_sock_addr.sin_addr.s_addr));
}

inline bool CTinySocket::CImpl::SetPacketDispatcher(
	FPacketDispatcher _dispatcher
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_dispatcher, false);
	CAutoLock this_lock(this, __FUNCTION__);

	m_packet_dispatcher = _dispatcher;
	return true;
}

DWORD CALLBACK CTinySocket::CImpl::RecvPacketThread(PVOID _param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_param, 1);

	CImpl* const this_ptr = reinterpret_cast<CImpl*>(_param);
	this_ptr->RecvPacket();

	return 0;
}


bool CALLBACK CTinySocket::CImpl::HandlePopPacket(
	PVOID _handler_param,
	LPCTSTR _packet_data_ptr,
	size_t _data_bytes,
	PVOID _packet_param
	)
{
	CAutoLock class_lock(NULL, __FUNCTION__);

	IF_NULL_ASSERT_RETURN_VALUE(_handler_param, false);
	IF_NULL_ASSERT_RETURN_VALUE(_packet_data_ptr, false);
	IF_NULL_ASSERT_RETURN_VALUE(_packet_param, false);

	CImpl* const this_ptr = reinterpret_cast<CImpl*>(_handler_param);
	IF_NULL_ASSERT_RETURN_VALUE(this_ptr->m_packet_dispatcher, false);

	const DWORD remote_ip_addr = reinterpret_cast<DWORD>(_packet_param);
	this_ptr->m_packet_dispatcher(remote_ip_addr, 
		reinterpret_cast<const BYTE*>(_packet_data_ptr), _data_bytes);

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
	IF_FALSE_ASSERT (NO_ERROR == _error_code && 0 < _bytes_transferred)
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

	this_ptr->SetRecvPacketBytes(_bytes_transferred);
	this_ptr->SetRecvPacketEnd();
	this_ptr->AddToMsgLoop();
	this_ptr->CreateRecvPacketThread();	// �ٴ�Ͷ�ݽ�����������
}


CTinySocket::CTinySocket(void)
	: m_impl_ptr(new CImpl())
{

}


CTinySocket::~CTinySocket(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CTinySocket::Create(WORD _local_port, bool _is_udp)
{
	return m_impl_ptr->Create(_local_port, _is_udp);
}


bool CTinySocket::IsValid(void) const
{
	return m_impl_ptr->IsValid();
}


bool CTinySocket::Destory(void)
{
	return m_impl_ptr->Destory();
}


CTinySocket::ERecvStatus CTinySocket::RecvPacket(void)
{
	return m_impl_ptr->RecvPacket();
}


int CTinySocket::SendPacket(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{
	return m_impl_ptr->SendPacket(
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


bool CTinySocket::SetPacketDispatcher(FPacketDispatcher _dispatcher)
{
	return m_impl_ptr->SetPacketDispatcher(_dispatcher);
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
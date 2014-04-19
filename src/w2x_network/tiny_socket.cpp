/*******************************************************************************
 * 文件：	tiny_socket.cpp
 * 描述：	参见 tiny_socket.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-05
 ******************************************************************************/

#include "stdafx.h"
#include "tiny_socket.h"
#include "utility.h"
#include "w2x_common/encode.h"
#include "w2x_common/error.h"
#include "w2x_common/mutex.h"
#include "w2x_common/msg_loop.h"
#include "w2x_common/log.h"
#include "w2x_common/thread.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)

using namespace thread;

// 收发数据包包头
struct PacketHeader {
	WORD	port;		// 端口号，网络字节序
	DWORD	ip_addr;	// IP 地址，网络字节序
};


class CTinySocket::CImpl
{
	enum {
		MAX_PACKET_BUF_SIZE = 1024,
	};

	/*
	 * 该结构保存异步 Socket 的相关信息传递给回调函数，
	 * wsa_overlapped 必须放在首位，根据它的地址得到
	 * 整个 SocketInfo 对象地址。
	 */
	struct SocketInfo {
		WSAOVERLAPPED	wsa_overlapped;
		CImpl*			this_ptr;
	};

	struct PacketInfo {
		PacketHeader header;
		CHAR		 data[MAX_PACKET_BUF_SIZE];
	};

	typedef bool (CALLBACK* FnPacketLoopHandler)(
		PVOID _handler_param,	// 处理器参数
		LPTSTR _packet,			// 数据包缓存区（二进制数据）
		size_t _bytes,			// 数据包字节数
		PVOID _packet_param		// 数据包参数
	);

public:
	CImpl(void);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)
W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)

public:
	/**
	 * 进程调用该函数初始化 WinSock DLL，若成功则返回 true, 否则返回 false。
	 * 只被调用一次，不过重复调用不会有影响，内部已做处理，不会重复初始化。
	 */
	static bool InitWinSock(void);

	/** 
	 * 进程调用该函数释放 WinSock DLL，若成功则返回 true, 否则返回 false。
	 */
	static void UninitWinSock(void);

	/** 确保 WinSock DLL 被初始化。*/
	static void EnsureWinSock(void);

	inline bool Create(
		WORD _local_port, 
		bool _is_udp, 
		LPCTSTR _mcast_addr_str,
		WORD _mcast_port
	);

	inline void Destroy(void);

	inline bool IsValid(void) const;

	inline bool IsBroadcastEnable(void) const;

	bool SendPacket(
		LPCTSTR _remote_addr_str,
		WORD _remote_port,
		const BYTE* _packet_buffer,
		DWORD _size_in_bytes
	);

	bool RecvPacket(void);

	bool EnableBroadcast(bool _is_enable);

	inline bool SetRecvDispatcher(FnPacketDispatcher _dispatcher);

	inline bool SetSendDispatcher(FnPacketDispatcher _dispatcher);

private:
	inline bool CreateUdpRecv(
		WORD _local_port, 
		DWORD _mcast_addr,
		WORD _mcast_port
	);

	inline bool CreateUdpSend(
		WORD _local_port, 
		DWORD _mcast_addr,
		WORD _mcast_port
	);

	// 为接收到的数据包设置结束符
	inline bool TrimRecvPacket(size_t _bytes);

	// 将接收到的消息添加到消息队列中
	inline bool AddToRecvLoop(void);

	bool WaitRecvPacket(DWORD _timeout);

	inline bool CreateRecvThread(void);

	//inline bool CreateSendThread(void);

	static bool CreateUdp(
		OUT SOCKET& _socket,
		WORD _local_port,
		bool _is_join_mcast
	);

	static bool JoinMulticast(
		SOCKET _socket,
		DWORD _mcast_addr,
		WORD _mcast_port,
		DWORD _act_flags
	);

	static bool DestroySocket(SOCKET _socket);

	static bool BindPort(SOCKET _socket, WORD _port);

	/** 线程函数，处理异步重叠 UDP 收包请求。*/
	static UINT CALLBACK RecvPacketThread(PVOID _param);

	/** 处理收包消息循环中的出队消息。*/
	static bool CALLBACK HandlePopRecvPacket(
		PVOID _handler_param,	// 处理器参数
		LPTSTR _packet,			// 数据包缓存区（二进制数据）
		size_t _bytes,			// 数据包字节数
		PVOID _packet_param		// 数据包参数
	);

	/** 处理发包消息循环中的出队消息。*/
	static bool CALLBACK HandlePopSendPacket(
		PVOID _handler_param,	// 处理器参数
		LPTSTR _packet,			// 数据包缓存区（二进制数据）
		size_t _bytes,			// 数据包字节数
		PVOID _packet_param		// 数据包参数
	);

	/*
	 * 当异步接收数据完成后，被 WSARecvFrom() 回调。
	 */
	static void CALLBACK HandleRecvCompletion(
		IN DWORD _error_code,
		IN DWORD _bytes_transferred,
		IN LPWSAOVERLAPPED _overlapped_ptr,
		IN DWORD _flags
	);

private:
	static bool	sm_is_win_sock_init;

	bool		m_is_udp;
	bool		m_is_broadcast_enable;
	SOCKET		m_recv_socket;
	SOCKET		m_send_socket;
//	SOCKET		m_mcast_recv_socket;
//	SOCKET		m_mcast_send_socket;
	SOCKADDR_IN	m_remote_sock_addr;
	PacketInfo	m_packet_info;
	size_t		m_packet_bytes;
	SocketInfo	m_recv_socket_info;
	SocketInfo	m_send_socket_info;
	size_t		m_packet_count;
	CMsgLoop	m_recv_loop;
	CMsgLoop	m_send_loop;
//	HANDLE		m_recv_thread_handle;
//	HANDLE		m_send_thread_handle;
	CThread*	m_recv_thread;

	FnPacketDispatcher m_recv_disp;
	FnPacketDispatcher m_send_disp;
};

W2X_IMPLEMENT_LOCKING_CLASS(CTinySocket::CImpl, CAutoLock)


bool CTinySocket::CImpl::sm_is_win_sock_init = false;


CTinySocket::CImpl::CImpl(void)
	: m_is_udp(false)
	, m_is_broadcast_enable(false)
	, m_recv_socket(INVALID_SOCKET)
	, m_send_socket(INVALID_SOCKET)
//	, m_mcast_recv_socket(INVALID_SOCKET)
//	, m_mcast_send_socket(INVALID_SOCKET)
	, m_packet_bytes(0)
//	, m_recv_thread_handle(NULL)
//	, m_send_thread_handle(NULL)
	, m_packet_count(0)
	, m_recv_disp(NULL)
	, m_send_disp(NULL)
	, m_recv_thread(new CThread(TEXT("TinySocketRecv")))
{
	memset(&m_remote_sock_addr, 0, sizeof(m_remote_sock_addr));
	memset(&m_recv_socket_info, 0, sizeof(m_recv_socket_info));
	memset(&m_send_socket_info, 0, sizeof(m_send_socket_info));
	memset(&m_packet_info, 0, sizeof(m_packet_info));
}


CTinySocket::CImpl::~CImpl(void)
{
	this->Destroy();
}


bool CTinySocket::CImpl::InitWinSock(void)
{
	if (true == sm_is_win_sock_init)
	{
		return true;
	}

	// 初始化WinSock DLL 2.2

	w2x::log::LogInfo(TEXT("[TinySocket] Initializing WinSock DLL 2.2 ..."));

	WORD version_requested = MAKEWORD(2, 2);
	WSADATA	wsa_data = {0};

	IF_FALSE_ASSERT (NO_ERROR == ::WSAStartup(version_requested, &wsa_data))
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Initialize WinSock DLL 2.2 faild. %d - %s."), 
			error_code, error_msg);
		return false;
	}

	sm_is_win_sock_init = true;
	w2x::log::LogInfo(TEXT("[TinySocket] Initialize WinSock DLL 2.2 successed."));

	::atexit(CImpl::UninitWinSock);

	return true;
}


void CTinySocket::CImpl::UninitWinSock(void)
{
	IF_FALSE_ASSERT_RETURN(true == sm_is_win_sock_init);

	w2x::log::LogInfo(TEXT("[TinySocket] Uninitializing WinSock DLL 2.2 ..."));

	// 释放WinSock DLL

	IF_FALSE_ASSERT (SOCKET_ERROR != ::WSACleanup())
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Uninitialize WinSock DLL 2.2 faild. %d - %s."), 
			error_code, error_msg);
		return;
	}

	sm_is_win_sock_init = false;

	w2x::log::LogInfo(TEXT("[TinySocket] Uninitialize WinSock DLL 2.2 successed."));
}


inline void CTinySocket::CImpl::EnsureWinSock(void)
{
	if (true == sm_is_win_sock_init)
	{
		return;
	}

	CImpl::InitWinSock();
}

bool CTinySocket::CImpl::Create(
	WORD _local_port, 
	bool _is_udp, 
	LPCTSTR _mcast_addr_str,
	WORD _mcast_port
	)
{
	m_is_udp = _is_udp;

	DWORD mcast_addr = INADDR_ANY;
	if (NULL != _mcast_addr_str)
	{
		internal::Str2IpAddr(mcast_addr, 
			*reinterpret_cast<const W2X_IP_ADDR_STR*>(_mcast_addr_str));
	}
	if (_is_udp) 
	{
		if (false == this->CreateUdpRecv(_local_port, mcast_addr, _mcast_port)) 
		{
			return false;
		}
		if (false == this->CreateUdpSend(0, mcast_addr, _mcast_port)) 
		{
			return false;
		}
		return true;
	}
	return false;
}


bool CTinySocket::CImpl::CreateUdpRecv(
	WORD _local_port, 
	DWORD _mcast_addr,
	WORD _mcast_port
	)
{
	const bool is_join_mcast = (INADDR_ANY != _mcast_addr);

	if (false == CImpl::CreateUdp(m_recv_socket, _local_port, is_join_mcast))
	{
		return false;
	}

	// 开启数据包接收循环队列
	if (false == m_recv_loop.StartLoopThread(CImpl::HandlePopRecvPacket, this))
	{
		w2x::log::LogError(TEXT("[TinySocket] Start loop failed!"));
		CImpl::DestroySocket(m_recv_socket);
		return false;
	}

	// 创建异步重叠发送数据线程
	if (false == this->CreateRecvThread())
	{
		m_recv_loop.StopLoopThread();
		CImpl::DestroySocket(m_recv_socket);
		return false;
	}

	if (is_join_mcast) 
	{
		if (false == CImpl::JoinMulticast(
			m_recv_socket, _mcast_addr, _mcast_port, JL_RECEIVER_ONLY)) 
		{
			return false;
		}
	}

	return true;
}


bool CTinySocket::CImpl::CreateUdpSend(
	WORD _local_port, 
	DWORD _mcast_addr,
	WORD _mcast_port
	)
{
	const bool is_join_mcast = (INADDR_ANY != _mcast_addr);

	if (false == CImpl::CreateUdp(m_send_socket, _local_port, is_join_mcast))
	{
		return false;
	}

	// 开启数据包接收循环队列
	if (false == m_send_loop.StartLoopThread(CImpl::HandlePopSendPacket, this))
	{
		w2x::log::LogError(TEXT("[TinySocket] Start loop failed!"));
		CImpl::DestroySocket(m_send_socket);
		return false;
	}

	// 创建异步重叠发送数据线程
// 	if (false == this->CreateSendThread())
// 	{
// 		m_send_loop.StopLoopThread();
// 		CImpl::DestroySocket(m_send_socket);
// 		return false;
// 	}

	if (is_join_mcast) 
	{
		if (false == CImpl::JoinMulticast(
			m_send_socket, _mcast_addr, _mcast_port, JL_SENDER_ONLY)) 
		{
			return false;
		}
	}

	return true;
}


bool CTinySocket::CImpl::CreateUdp(
	OUT SOCKET& _socket,
	WORD _local_port,
	bool _is_join_mcast
	)
{
	CImpl::EnsureWinSock();

	// 销毁已创建的 Socket
	CImpl::DestroySocket(_socket);

	// 是否收发送组播包
	DWORD create_flags = WSA_FLAG_OVERLAPPED;
	if (_is_join_mcast) 
	{
		create_flags |= WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF;
		w2x::log::LogInfo(TEXT("[TinySocket] Multicast UDP socket to create."));
	}

	_socket = ::WSASocket(
		AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, create_flags);

	if (INVALID_SOCKET == _socket)
	{
		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] UDP socket create failed. %d - %s"), 
			error_code, error_msg);
		return false;
	}
	else {
		w2x::log::LogInfo(
			TEXT("[TinySocket] UDP socket(%d) create."), _socket);
	}

	if (false == CImpl::BindPort(_socket, _local_port)) {
		CImpl::DestroySocket(_socket);
		return false;
	}

	return true;
}


bool CTinySocket::CImpl::JoinMulticast(
	IN SOCKET _socket,
	DWORD _mcast_addr,
	WORD _mcast_port,
	DWORD _act_flags
	)
{
	if (0 != (_act_flags & JL_SENDER_ONLY)) {
		_act_flags = JL_SENDER_ONLY;
	}
	else if (0 != (_act_flags & JL_RECEIVER_ONLY)) {
		_act_flags = JL_RECEIVER_ONLY;
	}
	else if (0 != (_act_flags & JL_BOTH)) {
		_act_flags = JL_BOTH;
	}
	else {
		w2x::log::LogError(
			TEXT("[TinySocket] UDP socket(%d): Invalid mcast action flags: %08X."), 
			_socket, _act_flags);
		ASSERT(false);
		return false;
	}

	const DWORD ip_cls = _mcast_addr & 0x000000FF;
	if (224 > ip_cls || 240 <= ip_cls) {
		w2x::log::LogError(
			TEXT("[TinySocket] UDP socket(%d): No a mcast address: %08X."), 
			_socket, _mcast_addr);
		ASSERT(false);
		return false;
	}

	SOCKADDR_IN sock_addr = {0};
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = _mcast_addr;
	sock_addr.sin_port = _mcast_port;

	SOCKET mcast_socket = ::WSAJoinLeaf(_socket, 
		reinterpret_cast<sockaddr*>(&sock_addr), 
		sizeof(sock_addr), NULL, NULL, NULL, NULL, _act_flags);

	if (INVALID_SOCKET == mcast_socket) {
		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] UDP socket(%d): MCast join failed. %d - %s"), 
			_socket, error_code, error_msg);
		CImpl::DestroySocket(_socket);
		ASSERT(false);
		return false;
	}

	w2x::log::LogInfo(
		TEXT("[TinySocket] UDP socket(%d): MCast join successed, socket(%d)."), 
		_socket, mcast_socket);

	return true;
}


inline bool CTinySocket::CImpl::IsValid(void) const
{
	return INVALID_SOCKET != m_recv_socket && INVALID_SOCKET != m_send_socket;
}

void CTinySocket::CImpl::Destroy(void)
{
	if (NULL != m_recv_socket_info.wsa_overlapped.hEvent)
	{
		::WSACloseEvent(m_recv_socket_info.wsa_overlapped.hEvent);
		m_recv_socket_info.wsa_overlapped.hEvent = INVALID_HANDLE_VALUE;
	}

// 	if (NULL != m_recv_thread_handle)
// 	{
// 		::CloseHandle(m_recv_thread_handle);
// 		m_recv_thread_handle = NULL;
// 	}

	SAFE_DELETE(m_recv_thread);

// 	if (NULL != m_send_thread_handle)
// 	{
// 		::CloseHandle(m_send_thread_handle);
// 		m_send_thread_handle = NULL;
// 	}

	CImpl::DestroySocket(m_recv_socket);
	CImpl::DestroySocket(m_send_socket);

	m_recv_loop.StopLoopThread();
	m_send_loop.StopLoopThread();
}


bool CTinySocket::CImpl::DestroySocket(SOCKET _socket)
{
	if (INVALID_SOCKET == _socket || false == sm_is_win_sock_init) {
		return false;
	}
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != _socket, false);

	w2x::log::LogInfo(
		TEXT("[TinySocket] Socket(%d) destroy..."), _socket);

	bool is_successed = true;

	// 停止发数据
	if (SOCKET_ERROR == ::shutdown(_socket, SD_SEND))
	{
		is_successed = false;

		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Socket(%d) shutdown failed. %d - %s"),
			_socket, error_code, error_msg);
	}

	// 关闭Socket
	if (SOCKET_ERROR == ::closesocket(_socket))
	{
		is_successed = false;

		TCHAR error_msg[MAX_PATH] = TEXT("");
		const DWORD error_code = ::WSAGetLastError();
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Socket(%d) close failed. %d - %s"),
			_socket, error_code, error_msg);
	}

	if (is_successed) {
		w2x::log::LogInfo(
			TEXT("[TinySocket] Socket(%d) destroy successed."), 
			_socket);
	} else {
		w2x::log::LogError(
			TEXT("[TinySocket] Socket(%d) destroy failed."), 
			_socket);
	}

	_socket = INVALID_SOCKET;

	return is_successed;
}


bool CTinySocket::CImpl::BindPort(SOCKET _socket, WORD _port)
{
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != _socket, false);

	// 配置本地地址信息
	SOCKADDR_IN	sock_addr_in = {0};
	sock_addr_in.sin_family			= AF_INET;
	sock_addr_in.sin_addr.s_addr	= ::htonl(INADDR_ANY);	// 自动分配主机地址
	sock_addr_in.sin_port			= ::htons(_port);		// 分配端口号

	w2x::log::LogInfo(
		TEXT("[TinySocket] Socket(%d) bind port(%d)."), 
		_socket, _port);

	// 绑定本地地址到 Socket
	if (SOCKET_ERROR == ::bind(_socket, 
		reinterpret_cast<PSOCKADDR>(&sock_addr_in), sizeof(SOCKADDR)))	
	{
		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Socket(%d) bind port(%d) failed. %d - %s"),
			_socket, _port, error_code, error_msg);
		ASSERT(false);
		return false;
	}

	w2x::log::LogInfo(
		TEXT("[TinySocket] Socket(%d) bind port(%d) successed."), 
		_socket, _port);

	return true;
}


bool CTinySocket::CImpl::RecvPacket(void)
{
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_recv_socket, false);

	WSAEVENT temp_event_handle = m_recv_socket_info.wsa_overlapped.hEvent;

	memset(&m_remote_sock_addr, 0, sizeof(m_remote_sock_addr));
	memset(&m_recv_socket_info, 0, sizeof(SocketInfo)); // 重叠结构，必须置空，否则会出错
	m_recv_socket_info.this_ptr = this;
	m_recv_socket_info.wsa_overlapped.hEvent = temp_event_handle;

	int	remote_addr_size = sizeof(SOCKADDR);	// 远程主机地址结构字节数
	DWORD behavior_flags = 0;					// WSARecvFrom 的 Flags 参数
	WSABUF wsa_buf = {0};
	memset(&m_packet_info, 0, sizeof(m_packet_info));
	wsa_buf.buf = m_packet_info.data;
	wsa_buf.len = sizeof(m_packet_info.data);
	m_packet_bytes = 0;

//	log::LogInfo(TEXT("[TinySocket] Begin WSARecvFrom."));

	//CImpl::LockClass();

	DWORD recv_bytes = 0;
	const int recv_ret_val = ::WSARecvFrom(
		m_recv_socket, &wsa_buf, 1, &recv_bytes, &behavior_flags, 
		reinterpret_cast<PSOCKADDR>(&m_remote_sock_addr),
		&remote_addr_size, &m_recv_socket_info.wsa_overlapped,
		CImpl::HandleRecvCompletion);

	//CImpl::UnlockClass();

//	log::LogInfo(TEXT("[TinySocket] End WSARecvFrom."));

//	log::LogInfo(TEXT("[TinySocket] Begin WaitRecvPacket."));

	if (false == this->WaitRecvPacket(WSA_INFINITE))
	{
		return false;
	}

//	log::LogInfo(TEXT("[TinySocket] End WaitRecvPacket."));

	return true;
}


bool CTinySocket::CImpl::WaitRecvPacket(DWORD _timeout)
{
	for (;;)
	{
		// 等待数据接收完成事件
		const DWORD wait_result = ::WSAWaitForMultipleEvents(1, 
			&m_recv_socket_info.wsa_overlapped.hEvent, FALSE, _timeout, TRUE);

		//::WSAResetEvent(m_recv_socket_info.wsa_overlapped.hEvent);

		if (WSA_WAIT_IO_COMPLETION == wait_result)
		{
			return true;
		}
		else if (WSA_WAIT_TIMEOUT == wait_result)
		{
			// 等待超时，继续等着
			continue;
		}
		else // 只有一个事件，这个分支只可能是 WSA_WAIT_FAILED
		{
			// 出错，检查 cEvents 和 lphEvents 两个参数是否有效

			const DWORD error_code = ::WSAGetLastError();
			TCHAR error_msg[MAX_PATH] = TEXT("");
			w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
			w2x::log::LogError(
				TEXT("[TinySocket] Wait for async receive UDP packet falid. %d - %s."),
				error_code, error_msg);
			ASSERT(false);
			CImpl::DestroySocket(m_recv_socket);

			::WSACloseEvent(m_recv_socket_info.wsa_overlapped.hEvent);
			m_recv_socket_info.wsa_overlapped.hEvent = NULL;

			return false;
		}
	}
}


inline bool CTinySocket::CImpl::CreateRecvThread(void)
{
	if (CThread::kNew != m_recv_thread->GetState()) {
		return false;
	}

	m_recv_socket_info.wsa_overlapped.hEvent = ::WSACreateEvent();
	IF_FALSE_ASSERT (WSA_INVALID_EVENT != m_recv_socket_info.wsa_overlapped.hEvent)
	{
		w2x::log::LogError(TEXT("[TinySocket] Create recv event failed."));
		return false;
	}

	if (false == m_recv_thread->Create(CImpl::RecvPacketThread, PVOID(this)) ||
		false == m_recv_thread->Start())
	{
		const DWORD error_code = ::GetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Create UDP socket receive thread failed. %d - %s."), 
			error_code, error_msg);

		::WSACloseEvent(m_recv_socket_info.wsa_overlapped.hEvent);
		m_recv_socket_info.wsa_overlapped.hEvent = INVALID_HANDLE_VALUE;

		return false;
	}

// 	m_recv_thread_handle = 
// 		::CreateThread(NULL, 0, CImpl::RecvPacketThread, 
// 		, 0, NULL);
// 
// 	if (NULL == m_recv_thread_handle)
// 	{
// 		const DWORD error_code = ::GetLastError();
// 		TCHAR error_msg[MAX_PATH] = TEXT("");
// 		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
// 		w2x::log::LogError(
// 			TEXT("[TinySocket] Create UDP socket receive thread failed. %d - %s."), 
// 			error_code, error_msg);
// 
// 		ASSERT(NULL != m_recv_thread_handle);
// 
// 		::WSACloseEvent(m_recv_socket_info.wsa_overlapped.hEvent);
// 		m_recv_socket_info.wsa_overlapped.hEvent = INVALID_HANDLE_VALUE;
// 
// 		return false;
// 	}

	return true;
}


// inline bool CTinySocket::CImpl::CreateSendThread(void)
// {
// 	m_send_socket_info.wsa_overlapped.hEvent = ::WSACreateEvent();
// 	IF_FALSE_ASSERT (WSA_INVALID_EVENT != m_send_socket_info.wsa_overlapped.hEvent)
// 	{
// 		w2x::log::LogError(TEXT("[TinySocket] Create recv event failed."));
// 		return false;
// 	}
// 
// 	m_send_thread_handle = ::CreateThread(
// 		NULL, 0, CImpl::SendPacketThread, 
// 		reinterpret_cast<LPVOID>(this), 0, NULL);
// 	IF_NULL_ASSERT (m_send_thread_handle)
// 	{
// 		TCHAR error_msg[MAX_PATH] = TEXT("");
// 		const DWORD error_code = ::GetLastError();
// 		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
// 		w2x::log::LogError(
// 			TEXT("[TinySocket] Create UDP socket send thread failed. %d - %s."), 
// 			error_code, error_msg);
// 
// 		::WSACloseEvent(m_send_socket_info.wsa_overlapped.hEvent);
// 		m_send_socket_info.wsa_overlapped.hEvent = WSA_INVALID_EVENT;
// 		m_send_loop.StopLoopThread();
// 
// 		CImpl::DestroySocket(m_send_socket);
// 
// 		return false;
// 	}
// 
// 	return true;
// }


bool CTinySocket::CImpl::SendPacket(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{	
 	IF_NULL_ASSERT_RETURN_VALUE(_packet_buffer, false);
	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_SOCKET != m_send_socket, false);

	// 如果为 TCP 则远程地址不能为空
	if (false == m_is_udp)
	{
		IF_NULL_ASSERT_RETURN_VALUE(_remote_addr_str, false);
	}

//	log::LogInfo(TEXT("[TinySocket] SendPacket Begin."));

	DWORD remote_addr = INADDR_ANY;
	const bool is_broadcast = (NULL == _remote_addr_str);
	if (true == is_broadcast)
	{	
		static DWORD s_broadcast_addr = INADDR_BROADCAST;
		if (INADDR_BROADCAST == s_broadcast_addr) {
			if (false == internal::GetBroadcastIpAddr(s_broadcast_addr)) {
				return false;
			}
		}
		remote_addr = s_broadcast_addr;
	}
	else {
		if (false == internal::Str2IpAddr(remote_addr,
				*(reinterpret_cast<const W2X_IP_ADDR_STR*>(_remote_addr_str)))) 
		{
			return false;
		}
	}

	// UDP 且要发送广播包则开启广播功能
	if (true == m_is_udp && true == is_broadcast)
	{
		static bool s_has_enable_broadcast = false;
		if (false == s_has_enable_broadcast)
		{
			w2x::log::LogInfo(TEXT("[CTinySocket] Enable Broadcast."));
			s_has_enable_broadcast = true;
			this->EnableBroadcast(is_broadcast);
		}
	}

	PacketInfo packet_info = {0};
	packet_info.header.ip_addr = remote_addr;
	packet_info.header.port = htons(_remote_port);
	memcpy_s(&(packet_info.data), sizeof(packet_info.data), 
		_packet_buffer, _size_in_bytes);

//	log::LogInfo(TEXT("[TinySocket] SendPacket Add."));

	return m_send_loop.AddMsg(reinterpret_cast<LPCTSTR>(&packet_info), 
		sizeof(packet_info.header) + _size_in_bytes, 
		reinterpret_cast<PVOID>(this));
}


bool CTinySocket::CImpl::EnableBroadcast(bool _is_enable)
{
	if (_is_enable == m_is_broadcast_enable)
	{
		return true;
	}

	// 启用Socket广播功能

	BOOL is_enable_broadcast = _is_enable ? TRUE : FALSE;
	IF_FALSE_ASSERT (SOCKET_ERROR != ::setsockopt(
		m_send_socket, SOL_SOCKET, SO_BROADCAST, 
		reinterpret_cast<const char*>(&is_enable_broadcast), sizeof(BOOL)))
	{
		// 启用失败

		const DWORD error_code = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("[TinySocket] Enable socket to send broadcast data failed. %d - %s."),
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


inline bool CTinySocket::CImpl::TrimRecvPacket(size_t _bytes)
{
	m_packet_bytes = _bytes;
	IF_FALSE_ASSERT_RETURN_VALUE(0 < m_packet_bytes, false);

	// 填充 sizeof(TCHAR) 个字节的结束符 0

	const size_t buffer_bytes = sizeof(m_packet_info.data);
	const size_t packet_end_pos 
		= buffer_bytes - sizeof(TCHAR) > m_packet_bytes
		? m_packet_bytes : buffer_bytes - sizeof(TCHAR);
	m_packet_info.data[packet_end_pos] = 0;
	m_packet_info.data[packet_end_pos + 1] = 0;

	return true;
}


inline bool CTinySocket::CImpl::AddToRecvLoop(void)
{
	static size_t s_prev_packet_count = m_packet_count++;

	IF_FALSE_ASSERT_RETURN_VALUE(s_prev_packet_count < m_packet_count, false);
	s_prev_packet_count = m_packet_count++;

	m_packet_info.header.port = m_remote_sock_addr.sin_port;
	m_packet_info.header.ip_addr = m_remote_sock_addr.sin_addr.s_addr;

	return m_recv_loop.AddMsg(
		reinterpret_cast<LPCTSTR>(&m_packet_info), 
		m_packet_bytes + sizeof(m_packet_info.header));
}


inline bool CTinySocket::CImpl::SetRecvDispatcher(
	FnPacketDispatcher _dispatcher
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_dispatcher, false);

	m_recv_disp = _dispatcher;
	return true;
}


inline bool CTinySocket::CImpl::SetSendDispatcher(
	FnPacketDispatcher _dispatcher
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_dispatcher, false);

	m_send_disp = _dispatcher;
	return true;
}


UINT CALLBACK CTinySocket::CImpl::RecvPacketThread(PVOID _param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_param, 1);

	CImpl* const this_ptr = reinterpret_cast<CImpl*>(_param);
	WSAEVENT* event_ptr = &(this_ptr->m_recv_socket_info.wsa_overlapped.hEvent);
	do  {
		try {
			this_ptr->RecvPacket();
		}
		catch (std::exception e) 
		{
			LPCTSTR except_str_ptr = W2X_ALLOC_STR_A2T(e.what());
			log::LogError(TEXT("[TinySocket] Recv packet exception: %s."),
				except_str_ptr);
			ASSERT(false);
		}
		
	} while (NULL != *event_ptr && INVALID_HANDLE_VALUE != *event_ptr);

	log::LogInfo(
		TEXT("[TinySocket] Socket(%d) recv thread exit."), 
		this_ptr->m_recv_socket);

	return 0;
}


bool CALLBACK CTinySocket::CImpl::HandlePopRecvPacket(
	PVOID _handler_param,
	LPTSTR _packet_info_ptr,
	size_t _packet_info_bytes,
	PVOID _packet_param
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_handler_param, false);
	IF_NULL_ASSERT_RETURN_VALUE(_packet_info_ptr, false);
	UNUSED_ALWAYS(_packet_param);

	static CImpl* s_this_ptr = NULL;
	s_this_ptr = reinterpret_cast<CImpl*>(_handler_param);
	IF_NULL_ASSERT_RETURN_VALUE(s_this_ptr->m_recv_disp, false);

	static const PacketInfo* s_packet_info_ptr = NULL;
	s_packet_info_ptr = reinterpret_cast<const PacketInfo*>(_packet_info_ptr);
	static size_t s_packet_data_bytes = 0;
	s_packet_data_bytes = _packet_info_bytes - sizeof(s_packet_info_ptr->header);

	s_this_ptr->m_recv_disp(
		s_packet_info_ptr->header.ip_addr, s_packet_info_ptr->header.port,
		reinterpret_cast<const BYTE*>(s_packet_info_ptr->data), 
		s_packet_data_bytes);

	return true;
}



bool CALLBACK CTinySocket::CImpl::HandlePopSendPacket(
	PVOID _handler_param,
	LPTSTR _packet_info_ptr,
	size_t _packet_info_bytes,
	PVOID _packet_param
	)
{
//	w2x::log::LogInfo(TEXT("[TinySocket] Handle Send."));

	IF_NULL_ASSERT_RETURN_VALUE(_handler_param, false);
	IF_NULL_ASSERT_RETURN_VALUE(_packet_info_ptr, false);
	UNUSED_ALWAYS(_packet_param);

	static CImpl* s_this_ptr = NULL;
	s_this_ptr = reinterpret_cast<CImpl*>(_handler_param);
	IF_NULL_ASSERT_RETURN_VALUE(s_this_ptr->m_send_disp, false);

	static PacketInfo* s_packet_info_ptr = NULL;
	s_packet_info_ptr = reinterpret_cast<PacketInfo*>(_packet_info_ptr);
	static size_t s_packet_data_bytes = 0;
	s_packet_data_bytes = _packet_info_bytes - sizeof(s_packet_info_ptr->header);

	s_this_ptr->m_send_disp(
		s_packet_info_ptr->header.ip_addr, s_packet_info_ptr->header.port,
		reinterpret_cast<const BYTE*>(s_packet_info_ptr->data), 
		s_packet_data_bytes);

	// 开始发送数据包

	DWORD bytes_sent = 0;
	WSABUF wsa_buf = {0};
	wsa_buf.buf = s_packet_info_ptr->data;
	wsa_buf.len = s_packet_data_bytes;

	SOCKADDR_IN sock_addr_in = {0};
	sock_addr_in.sin_family = AF_INET;
	sock_addr_in.sin_addr.s_addr = s_packet_info_ptr->header.ip_addr;
	sock_addr_in.sin_port = s_packet_info_ptr->header.port;

	const DWORD ip_cls = s_packet_info_ptr->header.ip_addr & 0x000000FF;
	if (224 <= ip_cls && 240 > ip_cls)
	{
		w2x::log::LogInfo(TEXT("[TinySocket] Multicast."));
	}

//	w2x::log::LogInfo(TEXT("[TinySocket] Begin WSASendTo."));

	//CImpl::LockClass();

	const int sent_result = ::WSASendTo(
		s_this_ptr->m_send_socket, &wsa_buf, 1, &bytes_sent, 0, 
		reinterpret_cast<PSOCKADDR>(&sock_addr_in), 
		sizeof(SOCKADDR), NULL, NULL);

	//CImpl::UnlockClass();

//	w2x::log::LogInfo(TEXT("[TinySocket] End WSASendTo."));

	if (SOCKET_ERROR == sent_result)
	{
		const DWORD last_error = ::WSAGetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, last_error);
		w2x::log::LogError(
			TEXT("[TinySocket] Send UDP packet failed. %d - %s."),
			last_error, error_msg);

		ASSERT(false);
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
	UNUSED_ALWAYS(_flags);

	// 如果与服务器连接中断，断开与远程服务器连接，尝试再次连接
	IF_FALSE_ASSERT (NO_ERROR == _error_code && 0 < _bytes_transferred)
	{
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, _error_code);
		LPCTSTR desc 
			= (NO_ERROR != _error_code) 
			? TEXT("[TinySocket] Socket I/O operation failed!")
			: TEXT("[TinySocket] Socket closed!");
		w2x::log::LogError(TEXT("%s. %d - %s."), desc, _error_code, error_msg);

		return;
	}

// 	w2x::log::LogInfo(
// 		TEXT("[TinySocket] Recieve %d bytes data."), _bytes_transferred);

	// SocketInfo 的第一个字段类型为 WSAOVERLAPPED。因此 SocketInfo 对象的
	// 第一个字段 wsa_overlapped 的地址即为 SocketInfo 对象的地址，此处利用强
	// 制转换，相当于通过 LPWSAOVERLAPPED 传递了 SocketInfo 指针。
	CImpl::SocketInfo* const socket_info_ptr 
		= reinterpret_cast<CImpl::SocketInfo*>(_overlapped_ptr);

	CImpl* const this_ptr = socket_info_ptr->this_ptr;

	this_ptr->TrimRecvPacket(_bytes_transferred);
	this_ptr->AddToRecvLoop();
}


CTinySocket::CTinySocket(void)
	: m_impl_ptr(new CImpl())
{

}


CTinySocket::~CTinySocket(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CTinySocket::CreateUdp(
	WORD _local_port, 
	LPCTSTR _mcast_addr_str,
	WORD _mcast_port
	)
{
	return m_impl_ptr->Create(_local_port, true, _mcast_addr_str, _mcast_port);
}


bool CTinySocket::IsValid(void) const
{
	return m_impl_ptr->IsValid();
}


void CTinySocket::Destroy(void)
{
	m_impl_ptr->Destroy();
}


bool CTinySocket::SendPacket(
	LPCTSTR _remote_addr_str,
	WORD _remote_port,
	const BYTE* _packet_buffer,
	DWORD _size_in_bytes
	)
{
	return m_impl_ptr->SendPacket(
		_remote_addr_str, _remote_port, _packet_buffer, _size_in_bytes);
}


bool CTinySocket::SetRecvDispatcher(FnPacketDispatcher _dispatcher)
{
	return m_impl_ptr->SetRecvDispatcher(_dispatcher);
}


bool CTinySocket::SetSendDispatcher(FnPacketDispatcher _dispatcher)
{
	return m_impl_ptr->SetSendDispatcher(_dispatcher);
}



W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END
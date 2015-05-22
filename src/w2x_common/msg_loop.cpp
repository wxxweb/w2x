/******************************************************************************
 * 文件:	msg_loop.cpp
 * 描述:	参见 msg_loop.h
 * 作者:	wu.xiongxing
 * 邮箱:	wxxweb@gmail.com
 * 日期:	2013-12-10
 * 修改:	2015-05-22
 ******************************************************************************/

#include "stdafx.h"
#include "msg_loop.h"
#include "mutex.h"
#include "thread.h"

W2X_NAME_SPACE_BEGIN

using namespace thread;

//------------------------------------------------------------------------------
// 消息循环的实现类, CMsgLoop 只是对其进行包装。
//------------------------------------------------------------------------------
class CMsgLoop::CImpl
{
private:
	// 消息处理器
	typedef bool (CALLBACK *MsgHandler)(
		PVOID /* _handler_param */,
		LPTSTR /* _msg */,
		size_t /* _bytes */, 
		PVOID /* _msg_param */
	);

	// 消息信息结构
	struct Msg {
		LPCTSTR msg;
		size_t bytes;
		PVOID param;
	};

	// 消息队列
	typedef std::queue<Msg> MsgQueue;

public:
	CImpl(void);
	virtual ~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)

public:
	inline bool EnterMsg(LPCTSTR _msg, size_t _bytes, PVOID _msg_handler);
	inline bool SetMsgHandler(MsgHandler _msg_handler, PVOID _handler_param);
	inline bool StartLoopThread(void);
	inline void StopLoopThread(void);

private:
	inline size_t GetFrontMsgLength(void);

	inline bool QuitMsg(void);

	inline bool GetFrontMsg(
		LPCTSTR _buffer, 
		IN OUT size_t& _bytes, 
		OUT PVOID& _msg_param
	);

	inline bool WaitMsg(void);

	inline void ExitLoop(void);

	inline bool HandleMsg(LPTSTR _msg, size_t _bytes, PVOID _msg_param);

	static UINT CALLBACK LoopThread(PVOID _thread_param);

private:
	HANDLE		m_msg_event_handle;
	MsgQueue	m_msg_queue;
	MsgHandler	m_msg_handler;
	PVOID		m_handler_param;
	CThread*	m_loop_thread;

W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)
};

W2X_IMPLEMENT_LOCKING_CLASS(CMsgLoop::CImpl, CAutoLock)

CMsgLoop::CImpl::CImpl(void)
	: m_msg_event_handle(NULL)
	, m_msg_handler(NULL)
	, m_handler_param(NULL)
	, m_loop_thread(new CThread(TEXT("MsgLoop")))
{
	_ASSERT(m_loop_thread);
}

CMsgLoop::CImpl::~CImpl(void)
{
	this->StopLoopThread();
}

inline bool CMsgLoop::CImpl::SetMsgHandler(
	MsgHandler _msg_handler, 
	PVOID _handler_param
	)
{
	if (thread::CThread::kRunning == m_loop_thread->GetState())
	{
		return false;
	}

	ASSERT(NULL != _msg_handler);
	m_msg_handler = _msg_handler;
	m_handler_param = _handler_param;
	return NULL != m_msg_handler;
}

inline bool CMsgLoop::CImpl::HandleMsg(LPTSTR _msg, size_t _bytes, PVOID _msg_param)
{
	IF_NULL_ASSERT_RETURN_VALUE(m_msg_handler, true);
	bool ret_val = false;
	try {
		ret_val = m_msg_handler(m_handler_param, _msg, _bytes, _msg_param);
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:msg_loop", MB_ICONERROR);
		OutputDebugStringA("\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
	}
	return ret_val;
}

inline bool CMsgLoop::CImpl::EnterMsg(
	LPCTSTR _msg, 
	size_t _bytes, 
	PVOID _msg_param
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_msg, false);

	Msg msg = {0};
	msg.param = _msg_param;
	msg.bytes = (0 == _bytes) ? ((_tcslen(_msg) + 1) * sizeof(TCHAR)) : _bytes;
	msg.msg = reinterpret_cast<LPTSTR>(new BYTE[msg.bytes]);
	memcpy((void*)msg.msg, (void*)_msg, msg.bytes);

	CImpl::LockThis();
	m_msg_queue.push(msg);

	if (NULL != m_msg_event_handle) {
		::SetEvent(m_msg_event_handle);
	}
	CImpl::UnlockThis();
	return true;
}


inline size_t CMsgLoop::CImpl::GetFrontMsgLength(void)
{
	CImpl::LockThis();
	if (true == m_msg_queue.empty())
	{
		CImpl::UnlockThis();
		return 0;
	}
	CImpl::UnlockThis();

	CImpl::LockThis();
	const size_t bytes = m_msg_queue.front().bytes;
	CImpl::UnlockThis();

	return bytes;
}

inline bool CMsgLoop::CImpl::GetFrontMsg(
	LPCTSTR _buffer, 
	IN OUT size_t& _bytes, 
	OUT PVOID& _msg_param
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_buffer, false);

	CAutoLock autoLock(this);

	if (true == m_msg_queue.empty())
	{
		memset((void*)_buffer, 0, _bytes);
		return false;
	}

	const Msg& msg_ref = m_msg_queue.front();
	_msg_param = msg_ref.param;
	if (_bytes >= msg_ref.bytes)
	{
		_bytes = msg_ref.bytes;
		memcpy((void*)_buffer, (void*)msg_ref.msg, _bytes);

		return true;
	}

	memset((void*)_buffer, 0, _bytes);
	return false;
}

inline bool CMsgLoop::CImpl::QuitMsg(void)
{
	CImpl::LockThis();
	if (true == m_msg_queue.empty())
	{
		CImpl::UnlockThis();
		return false;
	}

	Msg& msg_ref = m_msg_queue.front();
	LPCTSTR msg_ptr = msg_ref.msg;
	m_msg_queue.pop();

	if (NULL != m_msg_event_handle)
	{
		bool is_empty = m_msg_queue.empty();
		if (is_empty) {
			::ResetEvent(m_msg_event_handle);
		}
	}
	CImpl::UnlockThis();

	try {
		delete[] msg_ptr;
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:msg_loop:QuitMsg", MB_ICONERROR);
	}

	return true;
}

inline bool CMsgLoop::CImpl::WaitMsg(void)
{
	while (NULL == m_msg_event_handle)
	{
		if (NULL == m_loop_thread) {
			return false;
		}
		::Sleep(50);
	}

	return WAIT_OBJECT_0 == ::WaitForSingleObject(m_msg_event_handle, INFINITE);
}

inline void CMsgLoop::CImpl::ExitLoop(void)
{
	CAutoLock autoLock(this);

	::ResetEvent(m_msg_event_handle);
	::CloseHandle(m_msg_event_handle);
	m_msg_event_handle = NULL;
	while (true == this->QuitMsg())
	{}
}


inline bool CMsgLoop::CImpl::StartLoopThread(void)
{
	if (NULL == m_msg_event_handle)
	{
		CImpl::LockThis();
		if (NULL == m_msg_event_handle) {
			m_msg_event_handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		if (false == m_msg_queue.empty()) {
			::SetEvent(m_msg_event_handle);
		}
		CImpl::UnlockThis();
	}

	if (CThread::kNew == m_loop_thread->GetState() &&
		true == m_loop_thread->Create(CImpl::LoopThread, PVOID(this)) &&
		true == m_loop_thread->Start())
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline void CMsgLoop::CImpl::StopLoopThread(void)
{
	this->ExitLoop();

	CAutoLock autoLock(this);
	SAFE_DELETE(m_loop_thread);
}

UINT CMsgLoop::CImpl::LoopThread(PVOID _thread_param)
{
	if (NULL == _thread_param)
	{
		ASSERT(false);
		return S_FALSE;
	}

	CImpl* const this_ptr = static_cast<CImpl*>(_thread_param);

	while (true == this_ptr->WaitMsg())
	{
		size_t bytes = this_ptr->GetFrontMsgLength();

		if (0 == bytes)
		{
			_ASSERT(0 != bytes);
			this_ptr->QuitMsg();
			continue;
		}
		LPTSTR msg_buffer = reinterpret_cast<LPTSTR>(new BYTE[bytes]);
		PVOID msg_param = NULL;
		if (false == this_ptr->GetFrontMsg(msg_buffer, bytes, msg_param))
		{
			this_ptr->QuitMsg();
			SAFE_DELETE_ARRAY(msg_buffer);
			continue;
		}
		this_ptr->HandleMsg(msg_buffer, bytes, msg_param);

		SAFE_DELETE_ARRAY(msg_buffer);
		this_ptr->QuitMsg();
	}

	::OutputDebugString(TEXT("[MsgLoop] exit thread!"));
	return S_OK;
}

//------------------------------------------------------------------------------
// 消息循环的包装类。
//------------------------------------------------------------------------------
CMsgLoop::CMsgLoop(void)
	: m_impl_ptr(new CImpl())
{
	ASSERT(NULL != m_impl_ptr);
}

CMsgLoop::~CMsgLoop(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}

bool CMsgLoop::AddMsg(LPCTSTR _msg, size_t _bytes, PVOID _msg_param)
{
	return m_impl_ptr->EnterMsg(_msg, _bytes, _msg_param);
}

bool CMsgLoop::StartLoopThread(MsgHandler _msg_handler, PVOID _handler_param)
{
	m_impl_ptr->SetMsgHandler(_msg_handler, _handler_param);
	return m_impl_ptr->StartLoopThread();
}

void CMsgLoop::StopLoopThread(void)
{
	m_impl_ptr->StopLoopThread();
}


W2X_NAME_SPACE_END
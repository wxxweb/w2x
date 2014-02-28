/******************************************************************************
 * �ļ���msg_loop.cpp
 * ������	�μ� msg_loop.h
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�2013-12-10
 ******************************************************************************/

#include "stdafx.h"
#include "msg_loop.h"
#include "thread.h"

//------------------------------------------------------------------------------
// ��Ϣѭ����ʵ����, CMsgLoop ֻ�Ƕ�����а�װ��
//------------------------------------------------------------------------------
class CMsgLoop::CImpl
{
private:
	// ��Ϣ������
	typedef bool (CALLBACK *MsgHandler)(
		PVOID /* _handler_param */,
		LPCTSTR /* _msg */,
		size_t /* _bytes */, 
		PVOID /* _msg_param */
	);

	// ��Ϣ��Ϣ�ṹ
	struct Msg {
		LPCTSTR msg;
		size_t bytes;
		PVOID param;
	};

	// ��Ϣ����
	typedef std::queue<Msg> MsgQueue;

public:
	CImpl(void);
	virtual ~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)

public:
	bool EnterMsg(LPCTSTR _msg, size_t _bytes, PVOID _msg_handler);
	bool SetMsgHandler(MsgHandler _msg_handler, PVOID _handler_param);
	bool StartLoopThread(void);
	void StopLoopThread(void);

private:
	size_t GetMsgQueueSize(void);
	size_t GetFrontMsgLength(void);
	bool QuitMsg(void);
	bool GetFrontMsg(LPCTSTR _buffer, 
					 IN OUT size_t& _bytes, 
					 OUT PVOID& _msg_param);
	bool WaitMsg(void);
	void ExitLoop(void);
	bool HandleMsg(LPCTSTR _msg, size_t _bytes, PVOID _msg_param);

	static DWORD WINAPI LoopThread(PVOID _thread_param);

private:
	HANDLE		m_msg_event_handle;
	HANDLE		m_loop_thread_handle;
	MsgQueue	m_msg_queue;
	MsgHandler	m_msg_handler;
	PVOID		m_handler_param;

W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)
};

W2X_IMPLEMENT_LOCKING_CLASS(CMsgLoop::CImpl, CAutoLock)

CMsgLoop::CImpl::CImpl(void)
	: m_msg_event_handle(NULL)
	, m_loop_thread_handle(NULL)
	, m_msg_handler(NULL)
	, m_handler_param(NULL)
{

}

CMsgLoop::CImpl::~CImpl(void)
{
	this->StopLoopThread();
}

bool CMsgLoop::CImpl::SetMsgHandler(MsgHandler _msg_handler, PVOID _handler_param)
{
	CAutoLock autoLock(this);
	if (NULL != m_loop_thread_handle)
	{
		return false;
	}

	ASSERT(NULL != _msg_handler);
	m_msg_handler = _msg_handler;
	m_handler_param = _handler_param;
	return NULL != m_msg_handler;
}

bool CMsgLoop::CImpl::HandleMsg(LPCTSTR _msg, size_t _bytes, PVOID _msg_param)
{
	CAutoLock autoLock(this);

	IF_NULL_ASSERT_RETURN_VALUE(m_msg_handler, true);
	return m_msg_handler(m_handler_param, _msg, _bytes, _msg_param);
}

bool CMsgLoop::CImpl::EnterMsg(LPCTSTR _msg, size_t _bytes, PVOID _msg_param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_msg, false);

	CAutoLock autoLock(this);

	bool is_set_event = true == m_msg_queue.empty();
	
	Msg msg = {0};
	msg.param = _msg_param;
	msg.bytes = (0 == _bytes) ? ((_tcslen(_msg) + 1) * sizeof(TCHAR)) : _bytes;
	LPTSTR buffer = reinterpret_cast<LPTSTR>(new BYTE[msg.bytes]);
	msg.msg = buffer;
	memcpy((void*)msg.msg, (void*)_msg, msg.bytes);
	m_msg_queue.push(msg);

	if (NULL == m_msg_event_handle)
	{
		m_msg_event_handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	if (true == is_set_event)
	{
		::SetEvent(m_msg_event_handle);
	}
	return true;
}

size_t CMsgLoop::CImpl::GetMsgQueueSize(void)
{
	CAutoLock autoLock(this);
	return m_msg_queue.size();
}

size_t CMsgLoop::CImpl::GetFrontMsgLength(void)
{
	CAutoLock autoLock(this);

	if (false == m_msg_queue.empty())
	{
		Msg& msg_ref = m_msg_queue.front();
		return msg_ref.bytes;
	}

	return 0;
}

bool CMsgLoop::CImpl::GetFrontMsg(LPCTSTR _buffer, 
								  IN OUT size_t& _bytes, 
								  OUT PVOID& _msg_param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_buffer, false);

	CAutoLock autoLock(this);

	if (true == m_msg_queue.empty())
	{
		memset((void*)_buffer, 0, _bytes);
		return false;
	}

	Msg& msg_ref = m_msg_queue.front();
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

bool CMsgLoop::CImpl::QuitMsg(void)
{
	CAutoLock autoLock(this);

	bool is_successed = false;

	if (false == m_msg_queue.empty())
	{
		is_successed = true;
		Msg& msg_ref = m_msg_queue.front();
		delete[] msg_ref.msg;
		msg_ref.msg = NULL;
		m_msg_queue.pop();
	}

	if (NULL != m_msg_event_handle && true == m_msg_queue.empty())
	{
		::ResetEvent(m_msg_event_handle);
	}
	return is_successed;
}

bool CMsgLoop::CImpl::WaitMsg(void)
{
	while (NULL == m_msg_event_handle)
	{
		::Sleep(100);
	}

	return WAIT_OBJECT_0 == ::WaitForSingleObject(m_msg_event_handle, INFINITE);
}

void CMsgLoop::CImpl::ExitLoop(void)
{
	CAutoLock autoLock(this);

	::ResetEvent(m_msg_event_handle);
	::CloseHandle(m_msg_event_handle);
	m_msg_event_handle = NULL;
	while (true == this->QuitMsg())
	{}
}


bool CMsgLoop::CImpl::StartLoopThread(void)
{
	CAutoLock autoLock(this);

	if (NULL != m_loop_thread_handle)
	{
		return false;
	}

	m_loop_thread_handle = ::CreateThread(NULL, 0, 
		CMsgLoop::CImpl::LoopThread, (LPVOID)this, 0, NULL);

	return true;
}

void CMsgLoop::CImpl::StopLoopThread(void)
{
	CAutoLock autoLock(this);

	this->ExitLoop();

	IF_NULL_RETURN(m_loop_thread_handle);

	::TerminateThread(m_loop_thread_handle, 0);
	::CloseHandle(m_loop_thread_handle);
	m_loop_thread_handle = NULL;
}

DWORD CMsgLoop::CImpl::LoopThread(PVOID _thread_param)
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
			continue;
		}
		LPTSTR msg_buffer = reinterpret_cast<LPTSTR>(new BYTE[bytes]);
		PVOID msg_param = NULL;
		if (false == this_ptr->GetFrontMsg(msg_buffer, bytes, msg_param))
		{
			continue;
		}
		this_ptr->HandleMsg(msg_buffer, bytes, msg_param);
		delete[] msg_buffer;
		msg_buffer = NULL;
		this_ptr->QuitMsg();
	}

	return S_OK;
}

//------------------------------------------------------------------------------
// ��Ϣѭ���İ�װ�ࡣ
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
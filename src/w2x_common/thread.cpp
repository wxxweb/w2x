/******************************************************************************
 * �ļ�:		thread.cpp
 * ����:		�μ� thread.h
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-04-01
 ******************************************************************************/

#include "stdafx.h"
#include "thread.h"
#include "mutex.h"
#include "debug.h"
#include "error.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(thread)


using namespace mutex;


class CThread::CImpl
{
	enum RetVal {
		kRetValArgsNull			= 0x00004001,	// �߳����̲���ΪNULL
		kRetValArgsInvalid		= 0x00004002,	// �߳����̲�����Ч
		kRetValThisPtrNull		= 0x00004003,	// thisָ��ΪNULL
		kRetValThisPtrInvalid	= 0x00004004,	// thisָ����Ч��Ұָ��
		kRetValRoutineNull		= 0x00004005,	// �߳����̺���ָ��ΪNULL
		kRetValRoutineError		= 0x00004006,	// �߳����̺���ִ���쳣
		kRetValUserBase			= 0x00004F00,	// �û����ػ���ֵ
	};

	typedef UINT (CALLBACK* FnRoutine)(PVOID);

	struct RoutineArgs {
		FnRoutine routine;
		PVOID argument;
		CImpl* this_ptr;
	};

public:
	CImpl(LPCTSTR _name);
	virtual ~CImpl(void);

public:
	bool Create(FnRoutine _routine, PVOID _arg_ptr);
	bool Start(void);
	inline void Wait(DWORD _millis);

private:
	static UINT CALLBACK RealRoutine(PVOID _routine_args);

public:
	DWORD		m_obj_id;
	UINT		m_thread_id;
	HANDLE		m_handle;
	LPTSTR		m_obj_name;
	State		m_state;
	RoutineArgs	m_routine_args;
	

	CThreadMutex m_mutex;

	static DWORD sm_obj_count;
};


DWORD CThread::CImpl::sm_obj_count = 0;


CThread::CImpl::CImpl(LPCTSTR _name)
	: m_obj_id(W2X_ATOMIC_INCREMENT(&sm_obj_count))
	, m_obj_name(NULL)
	, m_handle(NULL)
	, m_state(kNew)
{
	if (NULL == _name) {
		_name = TEXT("w2x:thread:unnamed(%08x)");
		const size_t count = _tcslen(_name) + 8;
		try {
			m_obj_name = new TCHAR[count];
			_ASSERT(m_obj_name);
			_stprintf_s(m_obj_name, count, _name, m_obj_id);
		}
		catch(std::exception e) {
			m_obj_name = TEXT("w2x:thread:unnamed(null)");
			::MessageBoxA(NULL, e.what(), "w2x:thread:unnamed", MB_ICONERROR);
		}
	}
	else {
		const size_t len = _tcslen(_name);
		try {
			m_obj_name = new TCHAR[len + 1];
			_ASSERT(m_obj_name);
			memcpy(m_obj_name, _name, len * sizeof(TCHAR));
			m_obj_name[len] = TEXT('\0');
		}
		catch (std::exception e) {
			m_obj_name = TEXT("w2x:thread:name(null)");
			MessageBoxA(NULL, e.what(), "w2x:thread:name", MB_ICONERROR);
		}
	}

	memset(&m_routine_args, 0, sizeof(m_routine_args));
	m_routine_args.this_ptr = this;
}


CThread::CImpl::~CImpl(void)
{
	TCHAR thread_info[128] = TEXT("");
	_stprintf_s(thread_info, TEXT("w2x:thread:destruct: ")
		TEXT("obj_id(%d), name(%s), handle(0x%x), id(0x%x)"),
		m_obj_id, m_obj_name, m_handle, m_thread_id);
	::OutputDebugString(TEXT("\n"));
	::OutputDebugString(thread_info);
	::OutputDebugString(TEXT("\n"));

	::CloseHandle(m_handle);
	m_handle = NULL;

	SAFE_DELETE_ARRAY(m_obj_name);
}


inline bool CThread::CImpl::Create(FnRoutine _routine, PVOID _arg_ptr)
{
	if (NULL == _routine) {
		_ASSERT(NULL != _routine);
		//-------------------
		// TODO:
		// SetLastError()
		//-------------------
		return false;
	}
	mutex::CScopedLock auto_lock(m_mutex);

	if (kNew == m_state)
	{
		SECURITY_ATTRIBUTES sa = {0};
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = FALSE;
		sa.lpSecurityDescriptor = NULL;	// TODO: �����Ժ�ʵ��

		m_routine_args.routine = _routine;
		m_routine_args.argument = _arg_ptr;
		m_routine_args.this_ptr = this;

		m_handle = reinterpret_cast<HANDLE>(_beginthreadex(
			&sa, 0, CImpl::RealRoutine, &m_routine_args, 
			CREATE_SUSPENDED, &m_thread_id));

		_ASSERT(m_handle);
		if (NULL == m_handle) {
			MessageBox(NULL, TEXT("null==_beginthreadex"), 
				TEXT("w2x:thread:create"), MB_ICONERROR);
		}
		else {
			m_state = kRunnable;
		}
	}
	else {
		//-------------------
		// TODO:
		// SetLastError()
		//-------------------
	}
	return kRunnable == m_state;
}


bool CThread::CImpl::Start(void)
{
	if (NULL == m_handle) {
		_ASSERT(NULL != m_handle);
		//-------------------
		// TODO:
		// SetLastError()
		//-------------------
		return false;
	}
	if (kRunnable != m_state) {
		_ASSERT(kRunnable != m_state);
		return false;
	}
	const DWORD suspend_count = ::ResumeThread(m_handle);
	if (DWORD(-1) == suspend_count) 
	{
		const DWORD last_error = ::GetLastError();
		_ASSERT(DWORD(-1) != suspend_count);
		// TODO �� FormatError �� error.h ��
		TCHAR err_str[MAX_PATH] = TEXT("");
		debug::FormatError(err_str, MAX_PATH, last_error);
		::OutputDebugString(TEXT("\n"));
		::OutputDebugString(err_str);
		::OutputDebugString(TEXT("\n"));
		return false;
	}
	return true;
}


inline void CThread::CImpl::Wait(DWORD _millis)
{
	if (kRunning != m_state || NULL == m_handle) {
		return;
	}
	::WaitForSingleObject(m_handle, _millis);
}


UINT CALLBACK CThread::CImpl::RealRoutine(PVOID _routine_args)
{
	if (NULL == _routine_args) {
		_ASSERT(NULL != _routine_args);
		return kRetValArgsNull;
	}

	RoutineArgs* const routine_args = 
		reinterpret_cast<RoutineArgs*>(_routine_args);
	
	// ��ȡ���̲����б���Ϣ
	CImpl* this_ptr = NULL;
	FnRoutine routine = NULL;
	try {
		this_ptr = routine_args->this_ptr;
		if (NULL == this_ptr) {
			_ASSERT(NULL != this_ptr);
			return kRetValThisPtrNull;
		}
		routine = routine_args->routine;
		if (NULL == routine) {
			_ASSERT(NULL != routine);
			return kRetValRoutineNull;
		}
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:thread:routine", MB_ICONERROR);
		return kRetValArgsInvalid;
	}

	// �����̶߳�����Ϣ
	TCHAR thread_info[128] = TEXT("");
	try {
		_stprintf_s(thread_info, TEXT("w2x:thread:routine: ")
			TEXT("obj_id(%d), name(%s), handle(0x%x), id(0x%x)"),
			this_ptr->m_obj_id, this_ptr->m_obj_name, 
			this_ptr->m_handle, this_ptr->m_thread_id);
		::OutputDebugString(TEXT("\n"));
		::OutputDebugString(thread_info);
		::OutputDebugString(TEXT("\n"));
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:thread:routine", MB_ICONERROR);
		return kRetValRoutineError;
	}
	
	// ִ��ʵ��
	UINT ret_val = 0;
	try {
		this_ptr->m_state = kRunning;
		ret_val = routine(routine_args->argument);
		this_ptr->m_state = kTerminated;
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:thread:routine", MB_ICONERROR);
		return kRetValRoutineError;
	}

	// ����߳����̷��ؽ��
	try {
		TCHAR buff[32] = TEXT("");
		std::wstring output = thread_info;
		output += TEXT(" exit(");
		_itot_s(ret_val, buff, 10);
		output += buff;
		output += TEXT(").");
		::OutputDebugString(TEXT("\n"));
		::OutputDebugString(output.c_str());
		::OutputDebugString(TEXT("\n"));
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:thread:exit", MB_ICONERROR);
	}

	return kRetValUserBase + ret_val;
}


CThread::CThread(LPCTSTR _name)
	: m_impl_ptr(new CImpl(_name))
{
	if (NULL == m_impl_ptr) {
		_ASSERT(NULL != m_impl_ptr);
		MessageBox(NULL, TEXT("NULL==m_impl_ptr"), 
			TEXT("w2x:thread:create"), MB_ICONERROR);
	}
}

CThread::~CThread(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CThread::Create(FnRoutine _routine, PVOID _arg_ptr)
{
	return m_impl_ptr->Create(_routine, _arg_ptr);
}


bool CThread::Start(void)
{
	return m_impl_ptr->Start();
}


void CThread::Wait(DWORD _millis)
{
	m_impl_ptr->Wait(_millis);
}


DWORD CThread::GetId(void) const
{
	return m_impl_ptr->m_obj_id;
}


LPCTSTR CThread::GetName(void) const
{
	return m_impl_ptr->m_obj_name;
}


DWORD CThread::GetThreadId(void) const
{
	return m_impl_ptr->m_thread_id;
}


HANDLE CThread::GetThreadHandle(void) const
{
	return m_impl_ptr->m_handle;
}


CThread::State CThread::GetState(void) const
{
	return m_impl_ptr->m_state;
}


W2X_DEFINE_NAME_SPACE_END(thread)
W2X_NAME_SPACE_END
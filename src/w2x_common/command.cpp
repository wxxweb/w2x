/******************************************************************************
 * 文件:		command.cpp
 * 描述:		参见 command.h
 * 作者:		wu.xiongxing
 * 邮箱:		wxxweb@gmail.com
 * 时间:		2014-01-21
 ******************************************************************************/

#include "stdafx.h"
#include "command.h"
#include "encode.h"
#include "macros.h"
#include "log.h"

enum {
	PIPE_BUFFER_SIZE = 4096,	// 匿名管道默认缓冲区大小为4KB
};

W2X_NAME_SPACE_BEGIN

class CCommand::CImpl
{
public:
	CImpl(void);
	~CImpl(void);

public:
	CCommand::ExecuteStatus Execute(
		LPCTSTR _app_path, 
		LPCTSTR _cmd_line, 
		DWORD _time_out, 
		bool _is_save_output
	);

	inline int GetOutputLength(void);
	inline TSTDSTR GetOutput(void);
	LPTSTR GetOutput(LPTSTR str_buffer, size_t size_in_words);

private:
	inline int EnsureOutputIsUnicode(void);
	CCommand::ExecuteStatus ReadPipe(void);
	static UINT CALLBACK ReadPipeThread(PVOID _thread_param);

private:
	bool		 m_is_save_output;
	HANDLE		 m_read_pipe_handle;
	HANDLE		 m_read_thread_handle;
	TCHAR		 m_app_path[MAX_PATH];
	TCHAR		 m_app_args[2*MAX_PATH];
	std::wstring m_output_str;
	std::string  m_output_str_ascii;
};

CCommand::CImpl::CImpl(void)
	: m_is_save_output(false)
	, m_read_pipe_handle(NULL)
	, m_read_thread_handle(NULL)
{

}

CCommand::CImpl::~CImpl(void)
{
	if (NULL != m_read_thread_handle)
	{
		::CloseHandle(m_read_thread_handle);
		m_read_thread_handle = NULL;
	}
	if (NULL != m_read_pipe_handle)
	{
		::CloseHandle(m_read_pipe_handle);
		m_read_pipe_handle = NULL;
	}
}

CCommand::ExecuteStatus CCommand::CImpl::Execute(
	LPCTSTR _app_path, 
	LPCTSTR _app_args, 
	DWORD _time_out, 
	bool _is_save_output)
{
	m_output_str.clear();
	m_output_str_ascii.clear();

	IF_FALSE_ASSERT ((NULL != _app_path || NULL != _app_args))
	{
		w2x::log::LogError(TEXT("App path and args are NULL."));
		return kInvalidArg;
	}

	// 创建匿名管道
	HANDLE pipe_write_handle = NULL;
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	const BOOL is_create_pipe_successed = ::CreatePipe(
		&m_read_pipe_handle, &pipe_write_handle, &sa, PIPE_BUFFER_SIZE);
	if (FALSE == is_create_pipe_successed)
	{
		const DWORD dwErrorCode = ::GetLastError();
		w2x::log::LogError(TEXT("Create pipe faild(%d). CMD: %s %s"),
			dwErrorCode, m_app_path, m_app_args);

		ASSERT(FALSE != is_create_pipe_successed);
		return kCreatePipeFailed;
	}  

	// 父进程从子进程标准输出管道里读东西，子进程只写，不继承读的管道。
	::SetHandleInformation(m_read_pipe_handle, HANDLE_FLAG_INHERIT, 0);

	memset(m_app_path, 0, sizeof(m_app_path));
	wcscpy_s(m_app_path, NULL != _app_path ? _app_path : TEXT(""));
	memset(m_app_args, 0, sizeof(m_app_args));
	m_app_args[0] = TEXT(' ');
	wcscpy_s(m_app_args + 1, _countof(m_app_args) - 1,
		NULL != _app_args ? _app_args : TEXT(""));

	// 创建子进程
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	::GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = pipe_write_handle;
	si.hStdOutput = pipe_write_handle;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	// app_path 不能用 m_app_path 替代, 否则DOS命令会执行失败
	const BOOL is_create_process_successed = ::CreateProcess(
		_app_path, m_app_args, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
	if (FALSE == is_create_process_successed)
	{  
		const DWORD dwErrorCode = ::GetLastError();

		::CloseHandle(pipe_write_handle);
		::CloseHandle(m_read_pipe_handle);
		pipe_write_handle = NULL;
		m_read_pipe_handle = NULL;

		w2x::log::LogError(TEXT("Create command sub process faild(%d). CMD: %s %s"), 
			dwErrorCode, m_app_path, m_app_args);

		ASSERT(FALSE != is_create_process_successed);
		return kCreateProcessFailed;
	}
	// 父进程不需要写句柄，需立即关掉
	::CloseHandle(pipe_write_handle);
	pipe_write_handle = NULL;

	m_is_save_output = _is_save_output;
	if (false == m_is_save_output)
	{
		//--------------------------------------------------------------------
		// NOTE: 
		// 此处啥也不干！我只是想说，就算不保存子进程输出结果，也要从管道中读取数据！
		// 这样才能保证子进程顺利运行并最终结束返回。因为，如果不把管道缓冲区中的数据
		// 读取出来，那么，当缓冲区被填满后，子进程就无法再往里面写入数据，会阻塞掉！
		//--------------------------------------------------------------------
	}

	CCommand::ExecuteStatus exe_status = kSuccessed;
	if (INFINITE == _time_out)
	{
		exe_status = this->ReadPipe();
	}
	else // 如果要控制读管道的超时时间，就要创建读管道线程
	{
		m_read_thread_handle = (HANDLE)_beginthreadex(NULL, 0, 
			CImpl::ReadPipeThread, static_cast<LPVOID>(this), 0, NULL);
		if (NULL == m_read_thread_handle)
		{
			const DWORD dwErrorCode = ::GetLastError();
			w2x::log::LogError(TEXT("Create read pipe thread faild(%d). CMD: %s %s"), 
				dwErrorCode, m_app_path, m_app_args);
			ASSERT(NULL != m_read_thread_handle);
		}
		else
		{
			// 等子进程正常退出，超时的话就把子进程干掉
			const DWORD wait_status = ::WaitForSingleObject(pi.hProcess, _time_out);
			IF_FALSE_ASSERT (WAIT_TIMEOUT != wait_status)
			{
				::TerminateProcess(pi.hProcess, 0);

				w2x::log::LogError(
					TEXT("Wait command sub process time out(%d ms). CMD: %s %s"), 
					_time_out, m_app_path, m_app_args);
			}

			// 等读管道线程正常返回（子进程结束后读管道可自动结束）
			DWORD exit_code = 0;
			::WaitForSingleObject(m_read_thread_handle, INFINITE);
			::GetExitCodeThread(m_read_thread_handle, &exit_code);
			::CloseHandle(m_read_thread_handle);
			m_read_thread_handle = NULL;
			if (WAIT_TIMEOUT == wait_status) {
				exe_status = kTimeout;
			} else {
				exe_status = static_cast<CCommand::ExecuteStatus>(exit_code);
			}
		}
	}

	::CloseHandle(m_read_pipe_handle);
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	m_read_pipe_handle = NULL;

	ASSERT(kSuccessed == exe_status);

	return exe_status;
}


CCommand::ExecuteStatus CCommand::CImpl::ReadPipe(void)
{
	BOOL is_read_successed = FALSE;
	char read_buffer[PIPE_BUFFER_SIZE] = {0};
	DWORD bytes_read = 0;
	do {
		is_read_successed = ::ReadFile(m_read_pipe_handle, read_buffer,
			PIPE_BUFFER_SIZE - 1, &bytes_read, NULL);
		if (TRUE == is_read_successed && true == m_is_save_output)
		{
			m_output_str_ascii += read_buffer;
			memset(read_buffer, 0, sizeof(read_buffer));
		}
	}
	while (TRUE == is_read_successed);

	const DWORD last_error = ::GetLastError();
	IF_FALSE_ASSERT (FALSE != is_read_successed || ERROR_BROKEN_PIPE == last_error)
	{
		w2x::log::LogError(TEXT("Read pipe error(%d). CMD: %s %s"), 
			last_error, m_app_path, m_app_args);

		return kReadFailed;
	}

	return kSuccessed;
}

UINT CALLBACK CCommand::CImpl::ReadPipeThread(PVOID _thread_param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_thread_param, kInvalidArg);

	CImpl* this_ptr = static_cast<CImpl*>(_thread_param);
	return this_ptr->ReadPipe();
}

int CCommand::CImpl::EnsureOutputIsUnicode(void)
{
	if (false == m_output_str_ascii.empty() && true == m_output_str.empty())
	{
		LPWSTR output_ptr = W2X_ALLOC_STR_A2W(m_output_str_ascii.c_str());
		m_output_str = output_ptr;
		W2X_FREE_STR(output_ptr);
	}
	return m_output_str.length();
}

int CCommand::CImpl::GetOutputLength(void)
{
#ifndef UNICODE
	return m_output_str_ascii.length();
#endif
	return this->EnsureOutputIsUnicode();
}

TSTDSTR CCommand::CImpl::GetOutput(void)
{
#ifndef UNICODE
	return m_output_str_ascii;
#endif
	this->EnsureOutputIsUnicode();
	return m_output_str;
}

LPTSTR CCommand::CImpl::GetOutput(LPTSTR str_buffer, size_t size_in_bytes)
{
	IF_NULL_ASSERT_RETURN_VALUE(str_buffer, NULL);

	size_t output_str_bytes = 0;
#ifndef UNICODE
	output_str_bytes = m_output_str_ascii.length() + 1;
#else
	output_str_bytes = (m_output_str.length() + 1) * sizeof(wchar_t);
#endif
	
	IF_FALSE_ASSERT (output_str_bytes > size_in_bytes)
	{
		memset(str_buffer, 0, size_in_bytes * sizeof(TCHAR));
		return NULL;
	}

#ifndef UNICODE
	strcpy_s(str_buffer, size_in_bytes, m_output_str_ascii.c_str());
#else /* !UNICODE */
	if (true == m_output_str.empty())
	{
		this->EnsureOutputIsUnicode();
		wcscpy_s(str_buffer, size_in_bytes, m_output_str.c_str());
	}
#endif /* UNICODE */

	return str_buffer;
}

CCommand::CCommand(void)
	: m_impl_ptr(new CImpl())
{

}

CCommand::~CCommand(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}

CCommand::ExecuteStatus CCommand::Execute(
	LPCTSTR _app_path, 
	LPCTSTR _app_args,
	DWORD _time_out,
	bool _is_save_output)
{
	return m_impl_ptr->Execute(_app_path, _app_args, _time_out, _is_save_output);
}

CCommand::ExecuteStatus CCommand::Execute(
	LPCTSTR _cmd_line, 
	DWORD _time_out, 
	bool _is_save_output)
{
	return m_impl_ptr->Execute(NULL, _cmd_line, _time_out, _is_save_output);
}

int CCommand::GetOutputLength(void) const
{
	return m_impl_ptr->GetOutputLength();
}

TSTDSTR CCommand::GetOutput(void) const
{
	return m_impl_ptr->GetOutput();
}

LPTSTR CCommand::GetOutput(LPTSTR _str_buffer, size_t _size_in_bytes) const
{
	return m_impl_ptr->GetOutput(_str_buffer, _size_in_bytes);
}

W2X_NAME_SPACE_END
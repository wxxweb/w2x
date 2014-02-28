/******************************************************************************
 * �ļ���	command.cpp
 * �������μ� command.h
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2013-01-21
 ******************************************************************************/

#include "stdafx.h"
#include "command.h"
#include "encode.h"
#include "macros.h"
#include "log.h"

enum {
	PIPE_BUFFER_SIZE = 4096,	// �����ܵ�Ĭ�ϻ�������СΪ4KB
};

W2X_NAME_SPACE_BEGIN

class CCommand::CImpl
{
public:
	CImpl(void);
	~CImpl(void);

public:
	CCommand::EExecuteStatus Execute(
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
	CCommand::EExecuteStatus ReadPipe(void);
	static DWORD WINAPI ReadPipeThread(PVOID _thread_param);

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
		::TerminateThread(m_read_thread_handle, 0);
		::CloseHandle(m_read_thread_handle);
		m_read_thread_handle = NULL;
	}
	if (NULL != m_read_pipe_handle)
	{
		::CloseHandle(m_read_pipe_handle);
		m_read_pipe_handle = NULL;
	}
}

CCommand::EExecuteStatus CCommand::CImpl::Execute(
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
		return ES_INVALID_ARG;
	}

	// ���������ܵ�
	HANDLE pipe_write_handle = 0;
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	const BOOL is_create_pipe_successed = CreatePipe(
		&m_read_pipe_handle, &pipe_write_handle, &sa, PIPE_BUFFER_SIZE);
	IF_FALSE_ASSERT (TRUE == is_create_pipe_successed)
	{
		const DWORD dwErrorCode = ::GetLastError();
		w2x::log::LogError(TEXT("Create pipe faild(%d). CMD: %s %s"),
			dwErrorCode, m_app_path, m_app_args);

		return ES_CREATE_PIPE_FAILD;
	}  

	// �����̴��ӽ��̱�׼����ܵ�����������ӽ���ֻд�����̳ж��Ĺܵ���
	SetHandleInformation(m_read_pipe_handle, HANDLE_FLAG_INHERIT, 0);

	memset(m_app_path, 0, sizeof(m_app_path));
	wcscpy_s(m_app_path, NULL != _app_path ? _app_path : TEXT(""));
	memset(m_app_args, 0, sizeof(m_app_args));
	m_app_args[0] = TEXT(' ');
	wcscpy_s(m_app_args + 1, _countof(m_app_args) - 1,
		NULL != _app_args ? _app_args : TEXT(""));

	// �����ӽ���
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	::GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = pipe_write_handle;
	si.hStdOutput = pipe_write_handle;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	// app_path ������ m_app_path ���, ����DOS�����ִ��ʧ��
	const BOOL is_create_process_successed = ::CreateProcess(
		_app_path, m_app_args, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
	IF_FALSE_ASSERT (TRUE == is_create_process_successed)
	{  
		const DWORD dwErrorCode = ::GetLastError();

		::CloseHandle(pipe_write_handle);
		::CloseHandle(m_read_pipe_handle);
		pipe_write_handle = NULL;
		m_read_pipe_handle = NULL;

		w2x::log::LogError(TEXT("Create command sub process faild(%d). CMD: %s %s"), 
			dwErrorCode, m_app_path, m_app_args);

		return ES_CREATE_PROCESS;
	}
	// �����̲���Ҫд������������ص�
	::CloseHandle(pipe_write_handle);
	pipe_write_handle = NULL;

	m_is_save_output = _is_save_output;
	if (false == m_is_save_output)
	{
		//--------------------------------------------------------------------
		// NOTE: 
		// �˴�ɶҲ���ɣ���ֻ����˵�����㲻�����ӽ�����������ҲҪ�ӹܵ��ж�ȡ���ݣ�
		// �������ܱ�֤�ӽ���˳�����в����ս������ء���Ϊ��������ѹܵ��������е�����
		// ��ȡ��������ô�������������������ӽ��̾��޷���������д�����ݣ�����������
		//--------------------------------------------------------------------
	}

	CCommand::EExecuteStatus exe_status = ES_SUCCESSED;
	if (INFINITE == _time_out)
	{
		exe_status = this->ReadPipe();
	}
	else // ���Ҫ���ƶ��ܵ��ĳ�ʱʱ�䣬��Ҫ�������ܵ��߳�
	{
		m_read_thread_handle = ::CreateThread(NULL, 0, CImpl::ReadPipeThread,
			static_cast<LPVOID>(this), 0, NULL);
		IF_FALSE_ASSERT (NULL != m_read_thread_handle)
		{
			const DWORD dwErrorCode = ::GetLastError();
			w2x::log::LogError(TEXT("Create read pipe thread faild(%d). CMD: %s %s"), 
				dwErrorCode, m_app_path, m_app_args);
		}
		else
		{
			// ���ӽ��������˳�����ʱ�Ļ��Ͱ��ӽ��̸ɵ�
			const DWORD wait_status = ::WaitForSingleObject(pi.hProcess, _time_out);
			IF_FALSE_ASSERT (WAIT_TIMEOUT != wait_status)
			{
				::TerminateProcess(pi.hProcess, 0);

				w2x::log::LogError(
					TEXT("Wait command sub process time out(%d ms). CMD: %s %s"), 
					_time_out, m_app_path, m_app_args);
			}

			// �ȶ��ܵ��߳��������أ��ӽ��̽�������ܵ����Զ�������
			DWORD exit_code = 0;
			::WaitForSingleObject(m_read_thread_handle, INFINITE);
			::GetExitCodeThread(m_read_thread_handle, &exit_code);
			::CloseHandle(m_read_thread_handle);
			m_read_thread_handle = NULL;
			exe_status = static_cast<CCommand::EExecuteStatus>(exit_code);
		}
	}

	::CloseHandle(m_read_pipe_handle);
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	m_read_pipe_handle = NULL;

	ASSERT(ES_SUCCESSED == exe_status);

	return exe_status;
}


CCommand::EExecuteStatus CCommand::CImpl::ReadPipe(void)
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

		return ES_READ_FAILD;
	}

	return ES_SUCCESSED;
}

DWORD WINAPI CCommand::CImpl::ReadPipeThread(PVOID _thread_param)
{
	IF_NULL_ASSERT_RETURN_VALUE(_thread_param, ES_INVALID_ARG);

	CImpl* this_ptr = static_cast<CImpl*>(_thread_param);
	return this_ptr->ReadPipe();
}

int CCommand::CImpl::EnsureOutputIsUnicode(void)
{
	if (false == m_output_str_ascii.empty() && true == m_output_str.empty())
	{
		return w2x::encode::Ascii2Unicode(m_output_str, 
			m_output_str_ascii.c_str());
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

CCommand::EExecuteStatus CCommand::Execute(
	LPCTSTR _app_path, 
	LPCTSTR _app_args,
	DWORD _time_out,
	bool _is_save_output)
{
	return m_impl_ptr->Execute(_app_path, _app_args, _time_out, _is_save_output);
}

CCommand::EExecuteStatus CCommand::Execute(
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
/******************************************************************************
 * 文件:	process.cpp
 * 描述:	参见 process.h
 * 作者:	wu.xiongxing					
 * 邮箱:	wxxweb@gmail.com
 * 日期:	2014-03-27
 * 修改:	2015-05-22
 ******************************************************************************/

#include "stdafx.h"
#include "process.h"
#include "error.h"
#include "log.h"
#include <Tlhelp32.h>
#include <Psapi.h>
#include <winternl.h>

// for GetModuleBaseName
#if PSAPI_VERSION == 1
	#pragma comment(lib, "Psapi.lib")
#endif

#define STR_SYSTEM				TEXT("System")
#define STR_SYSTEM_IDLE_PROCESS	TEXT("System Idle Process")
#define STR_ERROR_AND_CODE		TEXT("error(%u)")


W2X_NAME_SPACE_BEGIN

static HANDLE	gs_process_mutex_handle = NULL;
static TCHAR	gs_process_mutex_name[MAX_PATH] = TEXT("");


W2X_COMMON_API void UninitProcessMutex(void)
{
	if (NULL == gs_process_mutex_handle)
	{
		return;
	}

	if (FALSE == ::ReleaseMutex(gs_process_mutex_handle))
	{
		ASSERT(false);
		const DWORD last_error = ::GetLastError();
		TCHAR error_str[MAX_PATH] = TEXT("");
		debug::FormatError(error_str, MAX_PATH, last_error);
		log::LogError(
			TEXT("Release global process mutex(%s) failed. [%d] %s"), 
			gs_process_mutex_name, last_error, error_str);
	}
	else
	{
		log::Custom custom;
		custom.is_immediately = true;
		custom.category = log::kCategoryWarn;
		log::Log(&custom,
			TEXT("Release global process mutex(%s) successed."), 
			gs_process_mutex_name);
	}
	::CloseHandle(gs_process_mutex_handle);
	gs_process_mutex_handle = NULL;
}


W2X_COMMON_API bool InitProcessMutex(GUID _guid)
{
	IF_FALSE_ASSERT (NULL == gs_process_mutex_handle)
	{
		log::LogInfo(
			TEXT("The global process mutex(%s) has been initialized."),
			gs_process_mutex_name);
		return true;
	}

	_stprintf_s(gs_process_mutex_name, TEXT("Global\\w2x:common:process:")
		TEXT("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), 
	_guid.Data1, _guid.Data2, _guid.Data3, 
	_guid.Data4[0], _guid.Data4[1], _guid.Data4[2], _guid.Data4[3], 
	_guid.Data4[4], _guid.Data4[5], _guid.Data4[6], _guid.Data4[7]);

	gs_process_mutex_handle = 
		::CreateMutex(NULL, TRUE, gs_process_mutex_name);

	const DWORD last_error = ::GetLastError();

	ASSERT(NULL != gs_process_mutex_handle);
	if (NULL == gs_process_mutex_handle)
	{
		TCHAR error_str[MAX_PATH] = TEXT("");
		debug::FormatError(error_str, MAX_PATH, last_error);
		log::LogError(
			TEXT("Create global process mutex(%s) failed. [%d] %s"), 
			gs_process_mutex_name, last_error, error_str);
		return true;
	}

	if(NULL != gs_process_mutex_handle && ERROR_ALREADY_EXISTS == last_error)
	{
		TCHAR error_str[MAX_PATH] = TEXT("");
		debug::FormatError(error_str, MAX_PATH, last_error);
		log::LogWarn(
			TEXT("The global process mutex(%s) already exists. [%d] %s"), 
			gs_process_mutex_name, last_error, error_str);
		::CloseHandle(gs_process_mutex_handle);
		gs_process_mutex_handle = NULL;
		return false;
	}

	log::LogInfo(
		TEXT("Create global process mutex(%s) sussessed."),
		gs_process_mutex_name);
	::atexit(UninitProcessMutex);
	return true;
}


W2X_COMMON_API LPCTSTR GetProcessMutex(void)
{
	return gs_process_mutex_name;
}


W2X_COMMON_API DWORD GetProcessName(
	OUT LPTSTR _buffer,
	DWORD _size_in_words,
	DWORD _process_id,
	bool _full_file_name
	)
{
	if (NULL == _buffer || 0 == _size_in_words) {
		return 0;
	}

	if(0 == _process_id) {
		const int written = _stprintf_s(
			_buffer, _size_in_words, STR_SYSTEM_IDLE_PROCESS);
		return (0 > written) ? 0 : static_cast<DWORD>(written);
	}
	else if(10 >= _process_id) {
		const int written = _stprintf_s(_buffer, _size_in_words, STR_SYSTEM);
		return (0 > written) ? 0 : static_cast<DWORD>(written);
	}
	
	HANDLE process_handle = ::OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, _process_id);
	if(NULL == process_handle) {
		const int written = _stprintf_s(
			_buffer, _size_in_words, STR_ERROR_AND_CODE, ::GetLastError());
		return (0 > written) ? 0 : static_cast<DWORD>(written);
	}

	memset(_buffer, 0, _size_in_words * sizeof(TCHAR));
	TCHAR module_name[MAX_PATH] = TEXT("");
	if ((false == _full_file_name && 
		::GetModuleBaseName(process_handle, NULL, module_name, MAX_PATH) == 0) ||
		(true == _full_file_name &&
		::GetModuleFileNameEx(process_handle, NULL, module_name, MAX_PATH) == 0))
	{
		const int written = _stprintf_s(
			_buffer, _size_in_words, STR_ERROR_AND_CODE, ::GetLastError());
		::CloseHandle(process_handle);
		return (0 > written) ? 0 : static_cast<DWORD>(written);
	}
	
	::CloseHandle(process_handle);
	process_handle = NULL;
	
	_tcscpy_s(_buffer, _size_in_words, module_name);
	const DWORD count = _tcslen(module_name) + 1;
	return count > _size_in_words ? _size_in_words : count;
}


W2X_COMMON_API DWORD GetParentProcessId(DWORD _process_id)
{
	typedef NTSTATUS (NTAPI *FnNtQueryInformationProcess)(
		IN HANDLE ProcessHandle,
		IN PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN ULONG ProcessInformationLength,
		OUT PULONG ReturnLength OPTIONAL
		);

	HMODULE ntdll_handle = ::GetModuleHandle(TEXT("NTDLL.DLL"));
	if (NULL == ntdll_handle) {
		return 0;
	}

	FnNtQueryInformationProcess pfnNtQueryInformationProcess = 
		reinterpret_cast<FnNtQueryInformationProcess>(::GetProcAddress(
		ntdll_handle, "NtQueryInformationProcess"));
	if (NULL == pfnNtQueryInformationProcess) {
		return 0;
	}

	HANDLE process_handle = ::OpenProcess(
		PROCESS_QUERY_INFORMATION, FALSE, _process_id);
	if (NULL == process_handle) {
		return 0;
	}

	PROCESS_BASIC_INFORMATION process_base_info = {0};
	NTSTATUS status = pfnNtQueryInformationProcess(process_handle,
		ProcessBasicInformation, (PULONG)&process_base_info,
		sizeof(process_base_info), NULL);

	::CloseHandle(process_handle);
	if (0 != status) {
		return 0;
	}

	return reinterpret_cast<DWORD>(process_base_info.Reserved3);
}


class CProcessSnapshot::CImpl
{
public:
	typedef std::vector<CProcessSnapshot::ProcessInfo> ProcessInfoVec;

public:
	CImpl(void);
	~CImpl(void);

public:
	ProcessInfoVec m_process_info_vec;
	CRITICAL_SECTION m_cs;
};


CProcessSnapshot::CImpl::CImpl(void)
{
	::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
}


CProcessSnapshot::CImpl::~CImpl(void)
{
	::DeleteCriticalSection(&m_cs);
}


CProcessSnapshot::CProcessSnapshot(void)
	: m_impl(new CImpl)
{}


CProcessSnapshot::~CProcessSnapshot(void)
{
	delete m_impl;
}


bool CProcessSnapshot::CreateSnapshot(void)
{
	HANDLE snapshot_handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == snapshot_handle) {
		log::LogWarn(
			TEXT("Create process snapshot failed(%u)."), ::GetLastError());
		return false;
	}

	ProcessInfo info = {0};
	PROCESSENTRY32 entry = {0};
	entry.dwSize = sizeof(PROCESSENTRY32);
	
	::EnterCriticalSection(&m_impl->m_cs);
	m_impl->m_process_info_vec.clear();

	for (BOOL retval = ::Process32First(snapshot_handle, &entry);
		FALSE != retval; retval = ::Process32Next(snapshot_handle, &entry))
	{
		memset(&info, 0, sizeof(ProcessInfo));
		info._process_id = entry.th32ProcessID;
		info._parent_process_id = entry.th32ParentProcessID;
		info._thread_count = entry.cntThreads;
		info._base_thread_priority = entry.pcPriClassBase;
		_tcscpy_s(info._exe_file_name, entry.szExeFile);

		m_impl->m_process_info_vec.push_back(info);
	}

	::LeaveCriticalSection(&m_impl->m_cs);

	::CloseHandle(snapshot_handle);
	return true;
}


UINT CProcessSnapshot::GetProcessCount(void) const
{
	return m_impl->m_process_info_vec.size();
}


bool CProcessSnapshot::GetProcessInfo(OUT ProcessInfo* _info, UINT _index) const
{
	if (NULL == _info) {
		return false;
	}

	EnterCriticalSection(&m_impl->m_cs);

	if (m_impl->m_process_info_vec.size() <= _index) {
		::LeaveCriticalSection(&m_impl->m_cs);
		memset(_info, 0, sizeof(ProcessInfo));
		return false;
	}

	*_info = m_impl->m_process_info_vec.at(_index);
	::LeaveCriticalSection(&m_impl->m_cs);

	return true;
}


bool CProcessSnapshot::FindProcess(OUT ProcessInfo* _info, UINT _id) const
{
	::EnterCriticalSection(&m_impl->m_cs);
	for (CImpl::ProcessInfoVec::const_iterator it = m_impl->m_process_info_vec.begin(),
			itEnd = m_impl->m_process_info_vec.end(); itEnd != it; ++it)
	{
		if (_id != it->_process_id) {
			continue;
		}
		if (NULL != _info) {
			*_info = *it;
		}
		::LeaveCriticalSection(&m_impl->m_cs);
		return true;
	}
	::LeaveCriticalSection(&m_impl->m_cs);

	if (NULL != _info) {
		memset(_info, 0, sizeof(ProcessInfo));
	}
	return false;
}


UINT CProcessSnapshot::FindProcesses(
	OUT ProcessInfo* _info_array,
	IN OUT UINT* _array_length,
	LPCTSTR _exe_file_name
	) const
{
	if (NULL != _info_array && NULL != _array_length && 0 != *_array_length) {
		memset(_info_array, 0, *_array_length * sizeof(ProcessInfo));
	}
	if (NULL == _exe_file_name) {
		if (NULL != _array_length) {
			*_array_length = 0;
		}
		return 0;
	}

	UINT count_found = 0;
	UINT count_store = 0;

	::EnterCriticalSection(&m_impl->m_cs);
	for (CImpl::ProcessInfoVec::const_iterator it = m_impl->m_process_info_vec.begin(),
		itEnd = m_impl->m_process_info_vec.end(); itEnd != it; ++it)
	{
		if (0 != _tcsicmp(_exe_file_name, it->_exe_file_name)) {
			continue;
		}
		++count_found;
		if (NULL != _info_array && NULL != _array_length &&
			count_found <= *_array_length)
		{
			_info_array[count_store++] = *it;
		}
	}
	::LeaveCriticalSection(&m_impl->m_cs);

	if (NULL != _array_length) {
		*_array_length = count_store;
	}

	return count_found;
}


W2X_NAME_SPACE_END
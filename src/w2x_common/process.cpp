/******************************************************************************
 * 文件:		process.cpp
 * 描述:		参见 process.h
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 日期:		2014-03-27
 ******************************************************************************/

#include "stdafx.h"
#include "process.h"
#include "error.h"
#include "log.h"

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


W2X_NAME_SPACE_END
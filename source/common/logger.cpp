/******************************************************************************
 * 文件：	logger.cpp
 * 描述：	参见 logger.h
 * 作者：	wu.xiongxing
 * 时间：	2013-05-25
 ******************************************************************************/

#include "stdafx.h"
#include "logger.h"

W2X_NAMESPACE_BEGIN

static CRITICAL_SECTION gs_sys_log_file_mutex;

LPCTSTR CLogger::sm_kSysLogDir = TEXT("log\\sys\\");
CLogger::CHelper CLogger::sm_helper;


CLogger::CHelper::CHelper(void)
{
	InitializeCriticalSection(&gs_sys_log_file_mutex);
}


CLogger::CHelper::~CHelper(void)
{
	DeleteCriticalSection(&gs_sys_log_file_mutex);
}


CLogger::CLogger(void)
{
	/*NULL*/
}


CLogger::~CLogger(void)
{
	/*NULL*/
}

void CLogger::log(LPCTSTR msg)
{
	if (NULL == msg)
	{
		msg = TEXT("NULL");
	}

	/* LOG 文件是 ANSI 编码的，须转换 */
	const int len_of_msg 
		= WideCharToMultiByte(CP_ACP, 0, msg, -1, NULL, 0, NULL, NULL);
	LPSTR msg_mbs = new char[len_of_msg + 1];
	ZeroMemory(msg_mbs, len_of_msg + 1);
	WideCharToMultiByte(CP_ACP, 0, msg, -1, msg_mbs, len_of_msg, NULL, NULL);

	/* 对 LOG 信息进行格式化 */
	const int size_of_msg_to_write = len_of_msg + 32;
	LPSTR msg_to_write = new char[size_of_msg_to_write];
	ZeroMemory(msg_to_write, size_of_msg_to_write);

	SYSTEMTIME local_time;
	GetLocalTime(&local_time);

	_snprintf_s(msg_to_write, size_of_msg_to_write, 
		size_of_msg_to_write - 1, "[%02d:%02d:%02d:%03d] -- %s\r\n", 
		local_time.wHour, local_time.wMinute, local_time.wSecond, 
		local_time.wMilliseconds, msg_mbs);

	delete[] msg_mbs;
	msg_mbs = NULL;

#ifdef _DEBUG
	OutputDebugStringA(msg_to_write);
#endif

	/* 确保 LOG 目录存在 */
	TCHAR log_dir_path[MAX_PATH] = TEXT("");
	TCHAR log_file_name[MAX_PATH] = TEXT("");

	GetModuleFileName(NULL, log_dir_path, sizeof(log_dir_path));
	PathRemoveFileSpec(log_dir_path);
	lstrcat(log_dir_path, TEXT("\\"));
	lstrcat(log_dir_path, sm_kSysLogDir);
	if (FALSE == PathFileExists(log_dir_path))
	{
		SHCreateDirectoryEx(NULL, log_dir_path, NULL);
	}

	_sntprintf_s(log_file_name, sizeof(log_file_name) / sizeof(TCHAR) - 1, 
		TEXT("%s%02d-%02d-%02d.log"), log_dir_path, 
		local_time.wYear, local_time.wMonth, local_time.wDay);

	/* 创建/打开 LOG 文件 */
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;  
	sa.bInheritHandle = FALSE; 

	BOOL is_write_successed = FALSE;
	DWORD number_of_bytes_written = 0;
	HANDLE log_file_handle = NULL;

	EnterCriticalSection(&gs_sys_log_file_mutex);	// 同一时刻只运行一个线程写 LOG

	log_file_handle = CreateFile(log_file_name, GENERIC_WRITE, 0, &sa, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	/* 写入 LOG 信息 */
	if (INVALID_HANDLE_VALUE != log_file_handle)
	{
		SetFilePointer(log_file_handle, 0, NULL, FILE_END);

		is_write_successed = WriteFile(log_file_handle, msg_to_write, 
			strlen(msg_to_write) * sizeof(char), &number_of_bytes_written, NULL);

		CloseHandle(log_file_handle);
	}

	LeaveCriticalSection(&gs_sys_log_file_mutex);	// 交由其它线程写 LOG

	delete[] msg_to_write;
	msg_to_write = NULL;

	/* 输出 DEBUG 信息 */
#ifdef _DEBUG
	if (INVALID_HANDLE_VALUE == log_file_handle || FALSE == is_write_successed)
	{
		char debug_string[1024] = "";
		LPCSTR what_happend = "";

		if (INVALID_HANDLE_VALUE == log_file_handle)
		{
			what_happend = "create/open file failed";
		}
		else if (FALSE == is_write_successed)
		{
			what_happend = "write file failed";
		}

		_snprintf_s(debug_string, sizeof(debug_string) - 1, 
			"Error(%d) : %s [%s()]\n - %s : ", 
			GetLastError(), __FILE__, __FUNCTION__, what_happend);

		OutputDebugStringA(debug_string);
		OutputDebugString(log_file_name);
	}
#endif /* _DEBUG */
}


W2X_NAMESPACE_END
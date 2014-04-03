/******************************************************************************
 * 文件：	debug.cpp
 * 描述：	参见 debug.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2013-05-24
 ******************************************************************************/

#include "stdafx.h"
#include "debug.h"
#include "log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)


// 保存自定义异常处理器函数
static FExcptionHandler gs_handler_ptr = NULL;


W2X_COMMON_API void SetReportMode(EReportMode mode)
{
	if (kModeDebug == mode)
	{ 
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	}
	else if (kModeStderr == mode)
	{
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	}
}


W2X_COMMON_API void EnableLeakCheck(bool delay)
{
	// Get the current state of the flag and store it in a temporary variable.
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// Enable debug heap allocations and use of memory block type identifiers, 
	// such as _CLIENT_BLOCK.
	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;

	// ON: Keep freed memory blocks in the heap's linked list, assign them the 
	// _FREE_BLOCK type, and fill them with the byte value 0xDD. This will 
	// allow us to catch any inadvertent use of freed memory.
	// OFF: Do not keep freed blocks in the heap's linked list.
	// Can also be combined with any of the heap-frequency check macros.
	if (delay) {
		tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	} else {
		tmpDbgFlag &= ~_CRTDBG_DELAY_FREE_MEM_DF;
	}

	// Perform automatic leak checking at program exit through a call to 
	// _CrtDumpMemoryLeaks and generate an error report if the application 
	// failed to free all the memory it allocated.
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Set the new state for the flag.
	_CrtSetDbgFlag(tmpDbgFlag);
}


static LONG WINAPI HandleTopLevelException(EXCEPTION_POINTERS *_excption_ptr)
{
	PEXCEPTION_RECORD record_ptr = _excption_ptr->ExceptionRecord;

	/* 获取主程序根全路径 */
	TCHAR app_root_patch[MAX_PATH] = TEXT("");
	::GetCurrentDirectory(MAX_PATH, app_root_patch);

	/* 获取异常模块全路径 */ 
	HMODULE exception_module_handle = NULL;
	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		reinterpret_cast<LPCTSTR>(record_ptr->ExceptionAddress), 
		&exception_module_handle);

	TCHAR exception_module_path[MAX_PATH] = TEXT("");
	::GetModuleFileName(exception_module_handle, exception_module_path, 
		sizeof(exception_module_path));

	/* 生成 MiniDump 文件 */
	TCHAR dump_file_path[MAX_PATH] = TEXT("");
	_stprintf_s(dump_file_path, TEXT("%s\\%s.0x%08x_0x%08x.dmp"),
		app_root_patch, ::PathFindFileName(exception_module_path),
		record_ptr->ExceptionCode, record_ptr->ExceptionAddress);

	HANDLE dump_file_handle = ::CreateFile(dump_file_path, GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != dump_file_handle)
	{
		MINIDUMP_EXCEPTION_INFORMATION dump_info;
		dump_info.ThreadId = ::GetCurrentThreadId();
		dump_info.ExceptionPointers = _excption_ptr;
		dump_info.ClientPointers = FALSE;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), 
			dump_file_handle, MiniDumpNormal, &dump_info, NULL, NULL);
		::CloseHandle(dump_file_handle);
		dump_file_handle = NULL;
	}

	/* 回调自定义异常处理器函数 */
	if (NULL != gs_handler_ptr)
	{
		ExcptionInfo excption_info;
		excption_info.module_name = exception_module_path;
		excption_info.excption_code = record_ptr->ExceptionCode;
		excption_info.excption_address = record_ptr->ExceptionAddress;
		excption_info.excption_flags = record_ptr->ExceptionFlags;

		LONG return_value = (*gs_handler_ptr)(excption_info);
		if (kPassToNextHandler != return_value)
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}
		else {
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	/* 输出默认的异常信息 */
	TCHAR excption_str[1024] = TEXT("");
	_stprintf_s(excption_str,
		TEXT("A excption occurs in module(%s), code(0x%08x), ")
		TEXT("address(0x%08x), flags(0x%08x), %d parameters, dump(%s)"),
		exception_module_path, record_ptr->ExceptionCode, 
		record_ptr->ExceptionAddress, record_ptr->ExceptionFlags, 
		record_ptr->NumberParameters, dump_file_path);

	w2x::log::Custom custom;
	custom.category = w2x::log::kCategoryWarn;
	custom.priority = w2x::log::kPriorityUrgent;
	custom.is_immediately = true;

	w2x::log::Log(&custom, excption_str);
	::MessageBox(NULL, excption_str, TEXT("Excption"), MB_ICONERROR);

	return EXCEPTION_CONTINUE_SEARCH;
}

static void HandleInvalidParameter(
	LPCTSTR expression,
	LPCTSTR function_name,
	LPCTSTR file_name,
	UINT line_code,
	UINT_PTR reserved_ptr)
{
	// function_name、file_name、line_code 在 Release 下无效

	TCHAR excption_str[512] = TEXT("");

#ifdef _DEBUG
	_stprintf_s(excption_str,
		TEXT("Invalid parameter detected in function(%s), ")
		TEXT("file(%s), line(%d), expression: %s"), 
		function_name, file_name, line_code, expression);
#else  /* !_DEBUG */
	_stprintf_s(excption_str, TEXT("Invalid parameter detected in function."));
#endif /* _DEBUG */

	w2x::log::Custom custom;
	custom.category = w2x::log::kCategoryWarn;
	custom.priority = w2x::log::kPriorityUrgent;
	custom.is_immediately = true;

	w2x::log::Log(&custom, excption_str);
	::MessageBox(NULL, excption_str, TEXT("Excption"), MB_ICONERROR);

	// 必须抛出异常，否则无法定位错误位置
	throw 1;
}

static void HandlePurecall(void)
{
	TCHAR excption_str[1024] = TEXT("");
	_stprintf_s(excption_str, TEXT("Pure virtual function call."));

	w2x::log::Custom custom;
	custom.category = w2x::log::kCategoryWarn;
	custom.priority = w2x::log::kPriorityUrgent;
	custom.is_immediately = true;

	w2x::log::Log(&custom, excption_str);
	::MessageBox(NULL, excption_str, TEXT("Excption"), MB_ICONERROR);

	// 必须抛出异常，否则无法定位错误位置
	throw 1;
}

W2X_COMMON_API void EnableExcptionHandle(FExcptionHandler _handler_ptr)
{
	gs_handler_ptr = _handler_ptr;
	::SetUnhandledExceptionFilter(HandleTopLevelException);
	::_set_invalid_parameter_handler(HandleInvalidParameter);
	::_set_purecall_handler(HandlePurecall);
}


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END
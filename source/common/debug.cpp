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


void SetReportMode(EReportMode mode)
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


void EnableLeakCheck(bool delay)
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

	HMODULE module_handle = NULL; 
	TCHAR module_name[MAX_PATH] = TEXT("");  
	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
		reinterpret_cast<LPCTSTR>(record_ptr->ExceptionAddress), &module_handle);
	::GetModuleFileName(module_handle, module_name, sizeof(module_name));

	if (NULL != gs_handler_ptr)
	{
		ExcptionInfo excption_info;
		excption_info.module_name = module_name;
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

	TCHAR excption_str[1024] = TEXT("");
	_stprintf_s(excption_str,
		TEXT("A excption occurs in module(%s), code(0x%08x), ")
		TEXT("address(0x%08x), flags(0x%08x), %d parameters."),
		module_name, record_ptr->ExceptionCode, record_ptr->ExceptionAddress,
		record_ptr->ExceptionFlags, record_ptr->NumberParameters);

	w2x::log::Custom custom;
	custom.category = w2x::log::kCategoryWarn;
	custom.priority = w2x::log::kPriorityUrgent;
	custom.is_immediately = true;

	w2x::log::Log(&custom, excption_str);

	::MessageBox(NULL, excption_str, TEXT("Excption"), MB_ICONERROR);

	return EXCEPTION_CONTINUE_SEARCH;
}


W2X_COMMON_API void EnableExcptionHandle(FExcptionHandler _handler_ptr)
{
	gs_handler_ptr = _handler_ptr;
	::SetUnhandledExceptionFilter(HandleTopLevelException);
}


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END
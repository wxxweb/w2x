/******************************************************************************
 * 文件：	crt_dbg.cpp
 * 描述：	参见 crt_dbg.h
 * 作者：	wu.xiongxing
 * 时间：	2013-05-24
 ******************************************************************************/

#include "stdafx.h"
#include "debug.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)

void set_report_mode(EReportMode mode)
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


void enable_leak_check(bool delay)
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

W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END
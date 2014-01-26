// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "common/common.h"
#include "common/command.h"

w2x::debug::EExcptionReturn CALLBACK HandleExcption(
	const w2x::debug::ExcptionInfo& _excption_info)
{
	MessageBox(NULL, _excption_info.module_name, TEXT("Excption"), MB_ICONERROR);
	return w2x::debug::kPassToNextHandler;
}

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle(/*HandleExcption*/);
	w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

// 	LPTSTR pszNull = NULL;
// 	*pszNull = TEXT('1');
// 	memset(pszNull, 0, 100);
// 	_tcscpy_s(pszNull, 100, TEXT("12234443"));

	ASSERT(true == w2x::version::IsWow64());
	ASSERT(false == w2x::version::IsWow64());
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_2000);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_XP);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_XP_SP2);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_XP_SP3);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_7);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_7_SP1);

	//char* pszNull = NULL;
	//IF_NULL_ASSERT_RETURN(pszNull, -1);
	char* psz1 = new char[100];
	memset(psz1, 0, 100);
	strcpy_s(psz1, 100, "12234443");
	//delete [] psz1;
	//SAFE_DELETE_ARRAY(psz1);
	ASSERT(NULL);
	TRACE("------");
	w2x::CCommand command;
	command.Execute(TEXT("C:\\ADB\\adb.exe"), TEXT("devices"));
	TSTDSTR cmd_output = command.GetOutput();
	w2x::log::LogInfo(cmd_output.c_str());
	w2x::log::Log(NULL, NULL);
	w2x::log::Log(NULL, TEXT("日志文件测试"));
	w2x::log::Log(NULL, TEXT("123988304890483"));
	w2x::log::Log(NULL, TEXT(""));
	w2x::log::Log(NULL, TEXT("SFSFSFDFJKLJSKLFJDSLKLDJSKLFJDKJDFD"));
	w2x::log::LogWarn(TEXT("Warn Log Test!"));
	w2x::log::LogError(TEXT("Error Log Test!"));
	w2x::log::LogDebug(TEXT("Debug Log Test!"));

	//MessageBox(NULL, NULL, NULL, 0);
	command.Execute(TEXT("D:\\Tools\\ADB\\adb.exe"), TEXT("devices"));
	cmd_output = command.GetOutput();
	w2x::log::LogInfo(cmd_output.c_str());

	system("pause");
	return 0;
}
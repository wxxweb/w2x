// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "common/debug.h"
#include "common/utility.h"
#include "common/logger.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//CDebug::set_report_mode(CDebug::kModeStderr);
	CDebug::enable_leak_check(true);

	//char* pszNull = NULL;
	//IF_NULL_ASSERT_RETURN(pszNull, -1);
	char* psz1 = new char[100];
	memset(psz1, 0, 100);
	strcpy_s(psz1, 100, "12234443");
	//delete [] psz1;
	//SAFE_DELETE_ARRAY(psz1);
	ASSERT(NULL);
	TRACE("------");
	w2x::CLogger::log(NULL);
	w2x::CLogger::log(TEXT("日志文件测试"));
	w2x::CLogger::log(TEXT("123988304890483"));
	w2x::CLogger::log(TEXT(""));
	w2x::CLogger::log(TEXT("SFSFSFDFJKLJSKLFJDSLKLDJSKLFJDKJDFD"));

	//MessageBox(NULL, NULL, NULL, 0);

	system("pause");
	return 0;
}
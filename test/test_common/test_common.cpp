// test_common.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "common/common.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//CDebug::set_report_mode(CDebug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

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
	w2x::log::Log(NULL, NULL);
	w2x::log::Log(NULL, TEXT("��־�ļ�����"));
	w2x::log::Log(NULL, TEXT("123988304890483"));
	w2x::log::Log(NULL, TEXT(""));
	w2x::log::Log(NULL, TEXT("SFSFSFDFJKLJSKLFJDSLKLDJSKLFJDKJDFD"));

	//MessageBox(NULL, NULL, NULL, 0);

	system("pause");
	return 0;
}
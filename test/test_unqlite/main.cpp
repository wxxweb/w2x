// test_common.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "w2x_common/debug.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);


	system("pause");
	return 0;
}
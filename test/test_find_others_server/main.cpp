// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "common\common.h"
#include "network\FindOthers.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	w2x::network::echoServer(22222, 1000000, 124);

	system("pause");
	return 0;
}
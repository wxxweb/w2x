// test_common.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "common\common.h"
#include "network\FindOthers.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	SOCKADDR_IN sock_addr_in = {0};
	w2x::network::findOthers(22222, 100000, &sock_addr_in);

	system("pause");
	return 0;
}
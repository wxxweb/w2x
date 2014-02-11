// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "common\common.h"
#include "network\lan_neighbor.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	SOCKADDR_IN sock_addr_in = {0};
	w2x::network::CLanNeighbor lan_neighbor;
	do {
		lan_neighbor.SayHello(22222);
		::Sleep(1000);
	} while (true);

	system("pause");
	return 0;
}
// test_common.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "common\common.h"
#include "network\tiny_socket.h"
#include "network\lan_neighbor.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	w2x::network::CLanNeighbor lan_neighbor;
	lan_neighbor.Listen(22221);

	system("pause");
	return 0;
}
// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "w2x_common\common.h"
#include "w2x_network\net_center.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	w2x::debug::EnableLeakCheck(true);

	setlocale(LC_ALL ,"chs");

// 	w2x::network::ITinySocketMsg::SetAuthCode(glk::MSG_AUTH_CODE);
// 
// 	w2x::network::CLanNeighbor lan_neighbor;
// 	w2x::network::ITinySocketMsg::InitUdp(glk::UDP_DATA_PORT);
// 	w2x::network::ITinySocketMsg::FilterOutLoopback(false);
// 
// 	//do {
// 		_tprintf_s(TEXT("Say Hello to everyone."));
// 		lan_neighbor.SayHello(glk::UDP_BROADCAST_PORT);
// 		::Sleep(2000);
// 	//} while (true);

	system("pause");
	return 0;
}
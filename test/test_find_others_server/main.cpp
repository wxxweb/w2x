// test_common.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "w2x_common/common.h"
#include "glk_logic/net_center.h"
#include "glk_logic/local_device.h"

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	setlocale(LC_ALL ,"chs");

	glk::CNetCenter::Start(glk::UDP_DATA_PORT, false);

	//LPCTSTR xml_str = TEXT("<ppt evt=\"1\"/>");

	//const int send_bytes = glk::CNetMsg::SendUtf8UdpMsg(glk::kNetMsgPpt, 
	//	glk::UDP_BROADCAST_PORT, NULL, xml_str);
	//const int send_bytes = glk::CNetCenter::SendUdpMsg(glk::kNetMsgHello);

	const int send_bytes = glk::CNetCenter::SendUtf8UdpMsg(
		glk::kNetMsgSms, glk::UDP_BROADCAST_PORT, NULL, 
		TEXT("<sms evt=\"1\" num=\"15240224599\" date=\"201403251421\">") 
		TEXT("<name>������</name>")
		TEXT("<msg><![CDATA[С��ͬѧ���쵽�ˣ��������ˣ�]]></msg></sms>"));

	system("pause");
	return 0;
}
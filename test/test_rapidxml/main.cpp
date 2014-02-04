// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_print.hpp"
#include "common\common.h"
#include <fstream>
#include <direct.h>

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle();
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	rapidxml::xml_document<TCHAR> xml_doc;
	rapidxml::xml_node<TCHAR>* xml_root = xml_doc.allocate_node(rapidxml::node_pi,
		xml_doc.allocate_string(TEXT("xml version='1.0' encoding='utf-8'")));
	xml_doc.append_node(xml_root);

	std::ofstream xml_file("test.xml");
	xml_file << xml_doc;

	system("pause");
	return 0;
}
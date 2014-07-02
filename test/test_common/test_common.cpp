// test_common.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "w2x_common/command.h"
#include "w2x_common/debug.h"
#include "w2x_common/log.h"
#include "w2x_common/variable.h"
#include "w2x_common/version.h"
#include "w2x_network/utility.h"
#include "w2x_network/tiny_socket.h"
#include "w2x_common/file_sys.h"
#include "w2x_common/encode.h"

class CDerived;
class CBase
{
public:
	CBase(CDerived *derived): m_pDerived(derived) {};
	~CBase();
	virtual void function(void) = 0;

	CDerived * m_pDerived;
};

class CDerived : public CBase
{
public:
	CDerived() : CBase(this) {};   // C4355
	virtual void function(void) {};
};

CBase::~CBase()
{
	m_pDerived->function();
}


void TestVariable(void)
{
	using namespace w2x;
	Var v1(TEXT("4.2234"));
	bool v1_bl = v1;
	int v1_nt = v1;
	UINT v1_unt = v1;
	DWORD v1_dw = v1;
	double v1_dbl = v1;
	LPCTSTR v1_str = v1;
	Var::Array* v1_ary = v1;
	Var::Object* v1_obj = v1;

	Var v2(TEXT("-4.2234"));
	bool v2_bl = v2;
	int v2_nt = v2;
	UINT v2_unt = v2;
	v2_nt = static_cast<int>(v2_unt);
	DWORD v2_dw = v2;
	double v2_dbl = v2;
	v2_dbl = static_cast<double>(v2_nt);
	v2_dbl = Var(v2_bl);
	v2_dbl = Var(v2_nt);
	v2_dbl = Var();
	v2_dbl = Var(false);
	v2_dbl = Var(UINT(4));
	v2_dbl = Var(DWORD(4));
	v2_dbl = Var(Var::Array());
	LPCTSTR v2_str = v2;
	Var::Array* v2_ary = v2;
	Var::Object* v2_obj = v2;

	Var v3(TEXT("3"));
	bool v3_bl = v3;
	int v3_nt = v3;
	UINT v3_unt = v3;
	DWORD v3_dw = v3;
	double v3_dbl = v3;
	LPCTSTR v3_str = v3;
	Var::Array* v3_ary = v3;
	Var::Object* v3_obj = v3;

	Var v4(TEXT("-3"));
	bool v4_bl = v4;
	int v4_nt = v4;
	UINT v4_unt = v4;
	DWORD v4_dw = v4;
	double v4_dbl = v4;
	LPCTSTR v4_str = v4;
	Var::Array* v4_ary = v4;
	Var::Object* v4_obj = v4;

	Var v5(TEXT("-3xy"));
	bool v5_bl = v5;
	int v5_nt = v5;
	UINT v5_unt = v5;
	DWORD v5_dw = v5;
	double v5_dbl = v5;
	LPCTSTR v5_str = v5;
	Var::Array* v5_ary = v5;
	Var::Object* v5_obj = v5;

	Var v6(TEXT("xy"));
	bool v6_bl = v6;
	int v6_nt = v6;
	UINT v6_unt = v6;
	DWORD v6_dw = v6;
	double v6_dbl = v6;
	LPCTSTR v6_str = v6;
	Var::Array* v6_ary = v6;
	Var::Object* v6_obj = v6;

	Var v7(4.324);
	bool v7_bl = v7;
	int v7_nt = v7;
	UINT v7_unt = v7;
	DWORD v7_dw = v7;
	double v7_dbl = v7;
	LPCTSTR v7_str = v7;
	Var::Array* v7_ary = v7;
	Var::Object* v7_obj = v7;

	Var v8(-4.324);
	bool v8_bl = v8;
	int v8_nt = v8;
	UINT v8_unt = v8;
	DWORD v8_dw = v8;
	double v8_dbl = v8;
	LPCTSTR v8_str = v8;
	Var::Array* v8_ary = v8;
	Var::Object* v8_obj = v8;

	Var v9(34);
	bool v9_bl = v9;
	int v9_nt = v9;
	UINT v9_unt = v9;
	DWORD v9_dw = v9;
	double v9_dbl = v9;
	LPCTSTR v9_str = v9;
	Var::Array* v9_ary = v9;
	Var::Object* v9_obj = v9;

	Var v10(-4.324);
	bool v10_bl = v10;
	int v10_nt = v10;
	UINT v10_unt = v10;
	DWORD v10_dw = v10;
	double v10_dbl = v10;
	LPCTSTR v10_str = v10;
	Var::Array* v10_ary = v10;
	Var::Object* v10_obj = v10;

	Var v11(-30);
	bool v11_bl = v11;
	int v11_nt = v11;
	UINT v11_unt = v11;
	DWORD v11_dw = v11;
	double v11_dbl = v11;
	LPCTSTR v11_str = v11;
	Var::Array* v11_ary = v11;
	Var::Object* v11_obj = v11;

	int x1 = 23;
	Var x1_blt = bool(true);
	x1 = x1_blt;
	Var x1_blf = bool(false);
	x1 = x1_blf;
	Var x1_unt = UINT(21);
	x1 = x1_unt;
	Var x1_dw = DWORD(21);
	x1 = x1_dw;
	Var x1_dbl = double(1.242);
	x1 = x1_dbl;
	Var x1_str = TEXT("sfg");
	x1 = x1_str;
	Var x1_strn = TEXT("-124");
	x1 = x1_strn;
	Var x1_ary = Var::Array();
	x1 = x1_ary;
	Var x1_obj = Var::Object();
	x1 = x1_obj;

	int x2 = -24;
	Var x2_blt = bool(true);
	x2 = x2_blt;
	Var x2_blf = bool(false);
	x2 = x2_blf;
	Var x2_unt = UINT(21);
	x2 = x2_unt;
	Var x2_dw = DWORD(21);
	x2 = x2_dw;
	Var x2_dbl = double(1.242);
	x2 = x2_dbl;
	Var x2_str = TEXT("sfg");
	x2 = x2_str;
	Var x2_strn = TEXT("-124");
	x2 = x2_strn;
	Var x2_ary = Var::Array();
	x2 = x2_ary;
	Var x2_obj = Var::Object();
	x2 = x2_obj;

	bool x3 = true;
	Var x3_nt = int(34);
	x3 = x3_nt;
	x3 = int(0);
	Var x3_nts = int(-34);
	x3 = x3_nts;
	Var x3_unt = UINT(21);
	x3 = x3_unt;
	x3 = UINT(0);
	Var x3_dw = DWORD(21);
	x3 = x3_dw;
	x3 = DWORD(0);
	Var x3_dbl = double(1.242);
	x3 = x3_dbl;
	x3 = double(0.0F);
	Var x3_str = TEXT("sfg");
	x3 = x3_str;
	x3 = TEXT("0");
	x3 = Var(TEXT("0"));
	Var x3_strn = TEXT("124");
	x3 = x3_strn;
	Var x3_ary = Var::Array();
	x3 = x3_ary;
	Var x3_obj = Var::Object();
	x3 = x3_obj;

	UINT x4 = 256;
	Var x4_nt = int(34);
	x4 = x4_nt;
	Var x4_nts = int(-34);
	x4 = x4_nts;
	Var x4_blt = bool(true);
	x4 = x4_blt;
	Var x4_blf = bool(false);
	x4 = x4_blf;
	Var x4_dw = DWORD(21);
	x4 = x4_dw;
	Var x4_dbl = double(1.242);
	x4 = x4_dbl;
	Var x4_str = TEXT("sfg");
	x4 = x4_str;
	Var x4_strn = TEXT("-124");
	x4 = x4_strn;
	x4 = Var(TEXT("0"));
	Var x4_ary = Var::Array();
	x4 = x4_ary;
	Var x4_obj = Var::Object();
	x4 = x4_obj;

	DWORD x5 = 234663;
	Var x5_blt = bool(true);
	x5 = x5_blt;
	Var x5_blf = bool(false);
	x5 = x5_blf;
	Var x5_unt = UINT(21);
	x5 = x5_unt;
	Var x5_nt = int(21);
	x5 = x5_nt;
	Var x5_nts = int(-21);
	x5 = x5_nts;
	Var x5_dbl = double(1.242);
	x5 = x5_dbl;
	Var x5_str = TEXT("sfg");
	x5 = x5_str;
	Var x5_strn = TEXT("-124");
	x5 = x5_strn;
	Var x5_ary = Var::Array();
	x5 = x5_ary;
	Var x5_obj = Var::Object();
	x5 = x5_obj;

	double x6 = 234.24324;
	Var x6_blt = bool(true);
	x6 = x6_blt;
	Var x6_blf = bool(false);
	x6 = x6_blf;
	Var x6_unt = UINT(21);
	x6 = x6_unt;
	Var x6_nt = int(21);
	x6 = x6_nt;
	Var x6_nts = int(-21);
	x6 = x6_nts;
	Var x6_dw = DWORD(1334);
	x6 = x6_dw;
	Var x6_str = TEXT("sfg");
	x6 = x6_str;
	Var x6_strn = TEXT("124.66");
	x6 = x6_strn;
	Var x6_ary = Var::Array();
	x6 = x6_ary;
	Var x6_obj = Var::Object();
	x6 = x6_obj;

	double x7 = -2.34;
	Var x7_blt = bool(true);
	x7 = x7_blt;
	Var x7_blf = bool(false);
	x7 = x7_blf;
	Var x7_unt = UINT(21);
	x7 = x7_unt;
	Var x7_nt = int(21);
	x7 = x7_nt;
	Var x7_nts = int(-21);
	x7 = x7_nts;
	Var x7_dw = DWORD(1334);
	x7 = x7_dw;
	Var x7_str = TEXT("sfg");
	x7 = x7_str;
	Var x7_strn = TEXT("-124.8");
	x7 = x7_strn;
	Var x7_ary = Var::Array();
	x7 = x7_ary;
	Var x7_obj = Var::Object();
	x7 = x7_obj;

	LPCTSTR x8 = TEXT("23");
	x8 = Var(true);
	Var x8_blt(true);
	x8 = x8_blt;
	x8 = Var(false);
	x8 = Var(int(0));
	x8 = Var(int(23));
	x8 = Var(int(-23));
	x8 = Var(UINT(0));
	x8 = Var(UINT(3442));
	x8 = Var(DWORD(0));
	x8 = Var(DWORD(3423442));
	x8 = Var(double(0));
	x8 = Var(double(234.2422));
	x8 = Var(double(-234.2422));
	x8 = Var(Var::Array());
	x8 = Var(Var::Object());
}

w2x::debug::EExcptionReturn CALLBACK HandleExcption(
	const w2x::debug::ExcptionInfo& _excption_info)
{
	//MessageBox(NULL, _excption_info.module_name, TEXT("Excption"), MB_ICONERROR);
	return w2x::debug::kPassToNextHandler;
}


void TestVersion(void)
{
	ASSERT(true);
	ASSERT(true == w2x::version::IsWow64());
	ASSERT(false == w2x::version::IsWow64());
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_2000);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_XP);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_XP_SP2);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_XP_SP3);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_7);
	ASSERT(w2x::version::GetOsVersion() > w2x::version::OS_VER_WIN_7_SP1);
}


void TestMemLeak(void)
{
	//char* pszNull = NULL;
	//IF_NULL_ASSERT_RETURN(pszNull, -1);
	char* psz1 = new char[100];
	memset(psz1, 0, 100);
	strcpy_s(psz1, 100, "12234443");
	//delete [] psz1;
	//SAFE_DELETE_ARRAY(psz1);
	ASSERT(NULL);
}


void TestLog(void)
{
	TRACE("------");
	w2x::log::Log(NULL, NULL);
	w2x::log::Log(NULL, TEXT("日志文件测试"));
	w2x::log::Log(NULL, TEXT("123988304890483"));
	w2x::log::Log(NULL, TEXT(""));
	w2x::log::Log(NULL, TEXT("SFSFSFDFJKLJSKLFJDSLKLDJSKLFJDKJDFD"));
	w2x::log::LogWarn(TEXT("Warn Log Test!"));
	w2x::log::LogError(TEXT("Error Log Test!"));
	w2x::log::LogDebug(TEXT("Debug Log Test!"));
}


void TestCommand(void)
{
	w2x::CCommand command;
	command.Execute(TEXT("C:\\ADB\\adb.exe"), TEXT("devices"));
	TSTDSTR cmd_output = command.GetOutput();
	w2x::log::LogInfo(cmd_output.c_str());

	//MessageBox(NULL, NULL, NULL, 0);
	command.Execute(TEXT("D:\\Tools\\ADB\\adb.exe"), TEXT("devices"));
	cmd_output = command.GetOutput();
	w2x::log::LogInfo(cmd_output.c_str());
}

void TestExcption(void)
{
	// 无效参数调用测试
// 	LPSTR format_string = NULL;
// 	printf(format_string);  // 发生异常

	// 纯虚函数调用测试
	CDerived myDerived;

	TCHAR app_path[MAX_PATH] = TEXT("");
	::GetModuleFileName(NULL, app_path, sizeof(app_path));
		w2x::version::ModuleVersionInfo version_info = {0};
		w2x::version::GetModuleVersionInfo(version_info,
			TEXT("F:\\Workspace\\Project\\cmccsuite3.1.0\\Version3.1.0.0_07\\uu.exe"));

	/* 测试异常捕获 */
	w2x::Var* var_ptr = NULL;
	var_ptr->GetLength();
	LPTSTR pszNull = NULL;
	*pszNull = TEXT('1');
	memset(pszNull, 0, 100);
	_tcscpy_s(pszNull, 100, TEXT("12234443"));
	printf("%s", 1);
}

void TestNetwork(void)
{
	w2x::net::CTinySocket tiny_socket;
	tiny_socket.CreateUdp();

	TCHAR ip_str[w2x::net::MAX_IP_ADDR_STR] = TEXT("012.022.0.1");
	w2x::net::W2X_IP_ADDR_STR ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	DWORD ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	_tcscpy_s(ip_str, TEXT("0.0.0.0"));
	ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	_tcscpy_s(ip_str, TEXT("255.255.255.255"));
	ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	_tcscpy_s(ip_str, TEXT("256.255.255.255"));
	ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	_tcscpy_s(ip_str, TEXT("001.110.101.010"));
	ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	_tcscpy_s(ip_str, TEXT("0.0.0.1"));
	ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	_tcscpy_s(ip_str, TEXT("1.0.0.0"));
	ip_addr_str = 
		*reinterpret_cast<const w2x::net::W2X_IP_ADDR_STR*>(ip_str);
	ip_addr = INADDR_ANY;
	w2x::net::Str2IpAddr(ip_addr, ip_addr_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);

	w2x::net::Name2IpAddr(ip_addr, ip_str);
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);
	//w2x::net::Name2IpAddr(ip_addr, TEXT("juz.cmgame.com"));
	w2x::net::IpAddr2Str(ip_addr_str, ip_addr);
}


void TestFileSys(void)
{
	TSTDSTR strWorkDir = w2x::file::GetWorkingDirectoryPath();
	TSTDSTR strModDir = w2x::file::GetModuleDirectoryPath();

	::SetCurrentDirectory(TEXT("C:\\"));
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	::GetCurrentDirectory(MAX_PATH, szWorkDir);

	strWorkDir = w2x::file::GetWorkingDirectoryPath();
	TSTDSTR strModDir2 = w2x::file::GetModuleDirectoryPath();
}

void TestEncode(void)
{
	LPCWSTR pszWdie = L"大写aBCd中文";
	LPCSTR pszAscii = "小写edzD英文";
	LPCTSTR pszType = TEXT("字母dddAd评语");

	std::wstring strWide = W2X_A2W(pszAscii);
	std::string strAscii = W2X_W2A(pszWdie);
	TSTDSTR strType = W2X_A2T(pszAscii);

	strAscii = W2X_T2A(pszType);
	strWide = W2X_T2W(pszType);
	strType = W2X_W2T(pszWdie);
}

int _tmain(int argc, _TCHAR* argv[])
{
	w2x::debug::EnableExcptionHandle(/*HandleExcption*/);
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

	//TestVariable();
	//TestExcption();
	//TestNetwork();
	//TestLog();
	//TestFileSys();
	TestEncode();
	
	system("pause");
	return 0;
}
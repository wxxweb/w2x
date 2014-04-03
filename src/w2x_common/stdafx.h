// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

// ֻ���ر�Ҫ��ϵͳ DLL
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "DbgHelp.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
#define _CSTDIO_						// ��ֹ���� C ����ͷ�ļ� cstdio
#define _CSTRING_						// ��ֹ���� C ����ͷ�ļ� cstring 
#define _CWCHAR_						// ��ֹ���� C ����ͷ�ļ� cwchar

// ���� DEBUG ģʽ�¶��ڴ�й©���
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// Windows ͷ�ļ�:
#include <ShlObj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include <tchar.h>
#include <Windows.h>
#include <Wincrypt.h>
#include <strsafe.h>
#include <Dbghelp.h>

// C++�⺯��
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>

// C ����ʱͷ�ļ�
//#include <cassert>
#include <ctime>
#include <process.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "debug.h"


#ifdef W2X_COMMON_EXPORTS
	W2X_NAME_SPACE_BEGIN
	W2X_NAME_SPACE_END
	namespace internal = w2x;
#endif
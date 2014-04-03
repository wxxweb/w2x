// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// 只加载必要的系统 DLL
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "DbgHelp.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的信息
#define _CSTDIO_						// 禁止加载 C 语言头文件 cstdio
#define _CSTRING_						// 禁止加载 C 语言头文件 cstring 
#define _CWCHAR_						// 禁止加载 C 语言头文件 cwchar

// 开启 DEBUG 模式下堆内存泄漏检测
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// Windows 头文件:
#include <ShlObj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include <tchar.h>
#include <Windows.h>
#include <Wincrypt.h>
#include <strsafe.h>
#include <Dbghelp.h>

// C++库函数
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>

// C 运行时头文件
//#include <cassert>
#include <ctime>
#include <process.h>

// TODO: 在此处引用程序需要的其他头文件
#include "debug.h"


#ifdef W2X_COMMON_EXPORTS
	W2X_NAME_SPACE_BEGIN
	W2X_NAME_SPACE_END
	namespace internal = w2x;
#endif
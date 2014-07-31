// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// 只加载必要的系统 DLL
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的信息
#define _CSTDIO_						// 禁止加载 C 语言头文件 cstdio
#define _CSTRING_						// 禁止加载 C 语言头文件 cstring 
#define _CWCHAR_						// 禁止加载 C 语言头文件 cwchar

// 开启 DEBUG 模式下堆内存泄漏检测
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// C 运行时头文件
#include <stdlib.h>
#include <stdio.h>
#include <crtdbg.h>

// Windows 头文件:
#include <winsock2.h>
#include <tchar.h>
#include <Iphlpapi.h>
#include <Windows.h>
#include <Ws2tcpip.h>

// C++库函数
#include <map>
#include <list>
#include <vector>

// TODO: 在此处引用程序需要的其他头文件
#include "w2x_common/macros.h"
#include "third_party/include/rapidxml/rapidxml.hpp"
#include "w2x_common/debug.h"

#ifdef W2X_NETWORK_EXPORTS
W2X_NAME_SPACE_BEGIN
	W2X_DEFINE_NAME_SPACE_BEGIN(net)
	W2X_DEFINE_NAME_SPACE_END(net)
	W2X_NAME_SPACE_END
	namespace internal = w2x::net;
#endif
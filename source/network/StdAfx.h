// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的信息

// 开启 DEBUG 模式下堆内存泄漏检测
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// C 运行时头文件
#include <stdlib.h>
#include <crtdbg.h>

// Windows 头文件:
#include <tchar.h>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <Windows.h>

// C++库函数
#include <list>
#include <set>

// C 运行时头文件
#include <stdio.h>


// TODO: 在此处引用程序需要的其他头文件
#include "..\common\debug.h"
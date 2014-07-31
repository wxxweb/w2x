// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

// ֻ���ر�Ҫ��ϵͳ DLL
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
#define _CSTDIO_						// ��ֹ���� C ����ͷ�ļ� cstdio
#define _CSTRING_						// ��ֹ���� C ����ͷ�ļ� cstring 
#define _CWCHAR_						// ��ֹ���� C ����ͷ�ļ� cwchar

// ���� DEBUG ģʽ�¶��ڴ�й©���
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <stdio.h>
#include <crtdbg.h>

// Windows ͷ�ļ�:
#include <winsock2.h>
#include <tchar.h>
#include <Iphlpapi.h>
#include <Windows.h>
#include <Ws2tcpip.h>

// C++�⺯��
#include <map>
#include <list>
#include <vector>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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
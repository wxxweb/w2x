// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

// ���� DEBUG ģʽ�¶��ڴ�й©���
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <crtdbg.h>

// Windows ͷ�ļ�:
#include <tchar.h>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <Windows.h>
#include <Ws2tcpip.h>

// C++�⺯��
#include <list>
#include <set>

// C ����ʱͷ�ļ�
#include <stdio.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "..\common\debug.h"
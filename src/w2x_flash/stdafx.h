// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

// ֻ���ر�Ҫ��ϵͳ DLL
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Shlwapi.lib")

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
#include <crtdbg.h>

// Windows ͷ�ļ�:
#include <Shlwapi.h>
#include <tchar.h>
#include <Windows.h>
#include <strsafe.h>

// C++�⺯��
#include <map>
#include <string>
#include <vector>

// C ����ʱͷ�ļ�

// ����� debug.h ǰ������಻��ȥ
#include "third-party/include/rapidxml/rapidxml.hpp"

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "..\w2x_common\debug.h"
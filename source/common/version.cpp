/******************************************************************************
 * 文件：	version.h
 * 描述：	参见 version.cpp
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2014-01-23
 ******************************************************************************/

#include "stdafx.h"
#include "version.h"
#include "macros.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(version)


EOsVersion GetOsVersion(void)
{
	static DWORD s_os_version = OS_VER_EARLIER;

	if (OS_VER_EARLIER < s_os_version)
	{
		return static_cast<EOsVersion>(s_os_version);
	}

	OSVERSIONINFO os_version_info = {0};
	os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&os_version_info);

	if (VER_PLATFORM_WIN32_NT != os_version_info.dwPlatformId)
	{
		return OS_VER_EARLIER;
	}

	DWORDLONG condition_mask = 0;
	OSVERSIONINFOEX	os_version_info_ex = {0};
	os_version_info_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	os_version_info_ex.dwMajorVersion = os_version_info.dwMajorVersion;
	os_version_info_ex.dwMinorVersion = os_version_info.dwMinorVersion;

	VER_SET_CONDITION(condition_mask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	for (WORD service_major = 0; service_major < 10; ++service_major)
	{
		os_version_info_ex.wServicePackMajor = service_major;
		if (TRUE == VerifyVersionInfo(&os_version_info_ex, 
			VER_SERVICEPACKMAJOR, condition_mask))
		{
			break;
		}
	}

	VER_SET_CONDITION(condition_mask, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL);
	for (WORD service_minor = 0; service_minor < 10; ++service_minor)
	{
		os_version_info_ex.wServicePackMinor = service_minor;
		if (TRUE == VerifyVersionInfo(&os_version_info_ex,
			VER_SERVICEPACKMINOR, condition_mask))
		{
			break;
		}
	}

	s_os_version 
		= (os_version_info_ex.dwMajorVersion << 24)
		| (os_version_info_ex.dwMinorVersion << 16)
		| (os_version_info_ex.wServicePackMajor << 8)
		| (os_version_info_ex.wServicePackMinor);

	if (OS_VER_WIN_2000 > s_os_version)
	{
		s_os_version = OS_VER_EARLIER;
		return OS_VER_EARLIER;
	}
	if (OS_VER_WIN_8_SP4 < s_os_version)
	{
		s_os_version = OS_VER_LASTER;
		return OS_VER_LASTER;
	}

	return static_cast<EOsVersion>(s_os_version);
}


W2X_DEFINE_NAME_SPACE_END(version)
W2X_NAME_SPACE_END
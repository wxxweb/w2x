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
#include "log.h"

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(version)


W2X_COMMON_API EOsVersion GetOsVersion(void)
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


W2X_COMMON_API bool IsWow64(void)
{
	static int s_wow64_flag = -1;
	
	if (-1 != s_wow64_flag)
	{
		return TRUE == s_wow64_flag;
	}

	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.

	LPFN_ISWOW64PROCESS IsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(
		::GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	IF_NULL_ASSERT (IsWow64Process)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(
			TEXT("Get 'IsWow64Process' from kernel32.dll faild(%d)."), last_error);
		return false;
	}

	IF_FALSE_ASSERT (FALSE != IsWow64Process(::GetCurrentProcess(), &s_wow64_flag))
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("Call 'IsWow64Process' faild(%d)."), last_error);
	}

	return TRUE == s_wow64_flag;
}


W2X_COMMON_API bool GetModuleVersionInfo(
	OUT ModuleVersionInfo& _module_ver_info_ref,
	LPCTSTR _module_path,
	size_t _language_index)
{
	memset(&_module_ver_info_ref, 0, sizeof(ModuleVersionInfo));

	IF_NULL_ASSERT_RETURN_VALUE(_module_path, false);
	IF_FALSE_ASSERT_RETURN_VALUE(FALSE != ::PathFileExists(_module_path), false);

	DWORD ver_info_size = ::GetFileVersionInfoSize(_module_path, NULL);
	IF_FALSE_ASSERT_RETURN_VALUE(0 != ver_info_size, false);

	BYTE* ver_info_ptr = new BYTE[ver_info_size];
	IF_NULL_ASSERT_RETURN_VALUE(ver_info_ptr, false);

	memset(ver_info_ptr, 0, ver_info_size);

	IF_FALSE_ASSERT (FALSE != ::GetFileVersionInfo(
		_module_path, 0L, ver_info_size, ver_info_ptr))
	{
		SAFE_DELETE_ARRAY(ver_info_ptr);
		return false;
	}

	TCHAR sub_block[MAX_PATH] = TEXT("");
	UINT str_len_read = 0;

	// Structure used to store enumerated languages and code pages.
  
	struct LanguageInfo {
		WORD language;
		WORD code_page;
	} *language_info_ptr;
  
	// Read the list of languages and code pages.

	BOOL is_query_successed = ::VerQueryValue(
		ver_info_ptr, TEXT("\\VarFileInfo\\Translation"),
		reinterpret_cast<LPVOID*>(&language_info_ptr), &str_len_read);
	IF_FALSE_ASSERT (FALSE != is_query_successed)
	{
		SAFE_DELETE_ARRAY(ver_info_ptr);
		return false;
	}

	enum {
		kCompanyName,
		kFileDescription,
		kFileVersion,
		kInternalName,
		kLegalCopyright,
		kOriginalFileName,
		kProductName,
		kProductVersion,
		kVersionInfoKeyTotal,
	};

	LPCTSTR version_info_keys[kVersionInfoKeyTotal] = {0};
	version_info_keys[kCompanyName]			= TEXT("CompanyName");
	version_info_keys[kFileDescription]		= TEXT("FileDescription");
	version_info_keys[kFileVersion]			= TEXT("FileVersion");
	version_info_keys[kInternalName]		= TEXT("InternalName");
	version_info_keys[kLegalCopyright]		= TEXT("LegalCopyright");
	version_info_keys[kOriginalFileName]	= TEXT("OriginalFilename");
	version_info_keys[kProductName]			= TEXT("ProductName");
	version_info_keys[kProductVersion]		= TEXT("ProductVersion");

	LPTSTR ver_str_ptr = NULL;

	// Read the version info for each language and code page.

	const size_t language_count = str_len_read / sizeof(LanguageInfo);
	IF_FALSE_ASSERT_RETURN_VALUE(language_count >= _language_index, false);

	_module_ver_info_ref.language_id = language_info_ptr[_language_index].language;
	_module_ver_info_ref.code_page = language_info_ptr[_language_index].code_page;

	::VerLanguageName(_module_ver_info_ref.language_id, 
		_module_ver_info_ref.language_name, 
		_countof(_module_ver_info_ref.language_name));

	for (size_t key_index = 0; key_index < kVersionInfoKeyTotal; ++key_index)
	{
		HRESULT result_handle = ::StringCchPrintf(sub_block, MAX_PATH,
			TEXT("\\StringFileInfo\\%04x%04x\\%s"),
			language_info_ptr[_language_index].language,
			language_info_ptr[_language_index].code_page,
			version_info_keys[key_index]);

		IF_FALSE_ASSERT (SUCCEEDED(result_handle))
		{
			continue;
		}

		LPTSTR query_str_ptr = NULL;
		is_query_successed = ::VerQueryValue(ver_info_ptr, sub_block,
			reinterpret_cast<LPVOID*>(&query_str_ptr), &str_len_read);

		IF_FALSE_ASSERT (FALSE != is_query_successed)
		{
			continue;
		}

		switch (key_index)
		{
		case kCompanyName:
			_tcscpy_s(_module_ver_info_ref.company_name, query_str_ptr);
			break;
		case kFileDescription:
			_tcscpy_s(_module_ver_info_ref.file_description, query_str_ptr);
			break;
		case kFileVersion:
			break;
		case kInternalName:
			_tcscpy_s(_module_ver_info_ref.internal_name, query_str_ptr);
			break;
		case kLegalCopyright:
			_tcscpy_s(_module_ver_info_ref.legal_copyright, query_str_ptr);
			break;
		case kOriginalFileName:
			_tcscpy_s(_module_ver_info_ref.original_file_name, query_str_ptr);
			break;
		case kProductName:
			_tcscpy_s(_module_ver_info_ref.product_name, query_str_ptr);
			break;
		case kProductVersion:
			ver_str_ptr = query_str_ptr;
			break;
		default: break;
		}
	}

	// 解析版本号
	if (NULL != ver_str_ptr)
	{
		::VerQueryValue(ver_info_ptr, sub_block,
			reinterpret_cast<LPVOID*>(&ver_str_ptr), &str_len_read);
			
		TCHAR ver_code_str[8] = TEXT("");
		LPCTSTR curr_ver_ptr = ver_str_ptr;
		LPCTSTR curr_sep_ptr = ver_str_ptr;
		WORD ver_code_array[4] = {0};
		const INT_PTR ver_str_len = static_cast<INT_PTR>(_tcslen(ver_str_ptr));
		for (size_t i = 0;
				curr_ver_ptr - ver_str_ptr < ver_str_len; 
				++i, curr_ver_ptr = ++curr_sep_ptr)
		{
			curr_sep_ptr = _tcschr(curr_ver_ptr, TEXT('.'));
			if (NULL == curr_sep_ptr)
			{
				break;
			}
			_tcsncpy_s(ver_code_str, _countof(ver_code_str), 
				curr_ver_ptr, curr_sep_ptr - curr_ver_ptr);
			ver_code_array[i] = static_cast<WORD>(_ttoi(ver_code_str));
		}
		_module_ver_info_ref.major		= ver_code_array[0];
		_module_ver_info_ref.minor		= ver_code_array[1];
		_module_ver_info_ref.revision	= ver_code_array[2];
		_module_ver_info_ref.build		= ver_code_array[3];
	}

	SAFE_DELETE_ARRAY(ver_info_ptr);
	return true;
}


W2X_DEFINE_NAME_SPACE_END(version)
W2X_NAME_SPACE_END
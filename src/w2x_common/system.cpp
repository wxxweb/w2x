/*******************************************************************************
 * �ļ�:		system.cpp
 * ����:		�μ� system.h
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-03-28
 ******************************************************************************/

#include "stdafx.h"
#include "system.h"
#include "encode.h"
#include "error.h"
#include "log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(sys)


W2X_COMMON_API FARPROC GetLibraryProcAddress(
	OUT HMODULE* _lib_handle_ptr, 
	LPCTSTR _lib_name, 
	LPCSTR _proc_name
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_lib_handle_ptr, false);
	IF_NULL_ASSERT_RETURN_VALUE(_lib_name, false);
	IF_NULL_ASSERT_RETURN_VALUE(_proc_name, false);

	(*_lib_handle_ptr) = ::LoadLibraryEx(_lib_name, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (NULL == (*_lib_handle_ptr))
	{
		const DWORD last_error = ::GetLastError();
		TCHAR err_str[MAX_PATH] = TEXT("");
		debug::FormatError(err_str, MAX_PATH, last_error);
		log::LogError(TEXT("[GetLibraryProc] LoadLibrary: %s - (%d) %s"), 
			_lib_name, last_error, err_str);
		ASSERT(NULL != (*_lib_handle_ptr));
		return NULL;
	}

	FARPROC proc_addr = ::GetProcAddress((*_lib_handle_ptr), _proc_name);
	if (NULL == proc_addr)
	{
		const DWORD last_error = ::GetLastError();
		TCHAR err_str[MAX_PATH] = TEXT("");
		debug::FormatError(err_str, MAX_PATH, last_error);
		log::LogError(TEXT("[GetLibraryProc] GetProcAddress: %s: %s - (%d) %s"), 
			_lib_name, _proc_name, last_error, err_str);
		ASSERT(NULL != proc_addr);

		::FreeLibrary((*_lib_handle_ptr));
		(*_lib_handle_ptr) = NULL;
		return NULL;
	}

	return proc_addr;
}


W2X_COMMON_API bool IsOcxRegistered(CLSID _cls_id)
{
	TCHAR cls_id_str[W2X_GUID_STR_LEN] = TEXT("");
	encode::Guid2String(cls_id_str, W2X_GUID_STR_LEN, _cls_id);

	TCHAR sub_key_str[MAX_PATH] = TEXT("");
	_stprintf_s(sub_key_str, TEXT("CLSID\\{%s}\\InprocServer32"), cls_id_str);

	HKEY key_handle = NULL;
	LSTATUS status = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, 
		sub_key_str, 0, KEY_READ, &key_handle);

	IF_FALSE_ASSERT (ERROR_SUCCESS == status) 
	{
		TCHAR err_str[MAX_PATH] = TEXT("");
		debug::FormatError(err_str, MAX_PATH, status);
		log::LogWarn(TEXT("[IsOcxRegistered] %s - (%d) %s"), 
			cls_id_str, status, err_str);
		return false;
	}

	DWORD data_type = 0;
	DWORD data_bytes = MAX_PATH * sizeof(TCHAR);
	TCHAR ocx_path[MAX_PATH] = TEXT("");
	status = ::RegQueryValueEx(key_handle, NULL, NULL, &data_type,  
		reinterpret_cast<LPBYTE>(ocx_path), &data_bytes);

	IF_FALSE_ASSERT (ERROR_SUCCESS == status) 
	{
		TCHAR err_str[MAX_PATH] = TEXT("");
		debug::FormatError(err_str, MAX_PATH, status);
		log::LogWarn(TEXT("[IsOcxRegistered] %s - (%d) %s"), 
			cls_id_str, status, err_str);

		::RegCloseKey(key_handle);
		key_handle = NULL;
		return false;
	}

	::RegCloseKey(key_handle);
	key_handle = NULL;

	IF_FALSE_ASSERT (FALSE != ::PathFileExists(ocx_path))
	{
		return false;
	}
	
	return true;
}


W2X_COMMON_API bool RegisterOcx(LPCTSTR _ocx_path)
{
	IF_NULL_ASSERT_RETURN_VALUE(_ocx_path, false);

	HMODULE lib_handle = NULL;
	FARPROC DllRegisterServer 
		= GetLibraryProcAddress(&lib_handle, _ocx_path, "DllRegisterServer");
	if (NULL != DllRegisterServer)
	{
		HRESULT call_result = DllRegisterServer();
		if (FAILED(call_result))
		{
			const DWORD last_error = ::GetLastError();
			TCHAR err_str[MAX_PATH] = TEXT("");
			debug::FormatError(err_str, MAX_PATH, last_error);
			log::LogError(TEXT("[RegisterOcx] %s - (%d) %s"), 
				_ocx_path, last_error, err_str);

			ASSERT(SUCCEEDED(call_result));
		}

		::FreeLibrary(lib_handle);
		lib_handle = NULL;

		return SUCCEEDED(call_result);
	}
	
	return false;
}

W2X_COMMON_API bool UnregisterOcx(LPCTSTR _ocx_path)
{
	IF_NULL_ASSERT_RETURN_VALUE(_ocx_path, false);

	HMODULE lib_handle = NULL;
	FARPROC DllUnregisterServer 
		= GetLibraryProcAddress(&lib_handle, _ocx_path, "DllUnregisterServer");
	if (NULL != DllUnregisterServer)
	{
		HRESULT call_result = DllUnregisterServer();
		if (FAILED(call_result))
		{
			const DWORD last_error = ::GetLastError();
			TCHAR err_str[MAX_PATH] = TEXT("");
			debug::FormatError(err_str, MAX_PATH, last_error);
			log::LogError(TEXT("[RegisterOcx] %s - (%d) %s"), 
				_ocx_path, last_error, err_str);

			ASSERT(SUCCEEDED(call_result));
		}

		::FreeLibrary(lib_handle);
		lib_handle = NULL;

		return SUCCEEDED(call_result);
	}

	return false;
}


W2X_DEFINE_NAME_SPACE_END(sys)
W2X_NAME_SPACE_END
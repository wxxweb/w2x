/******************************************************************************
 * 文件：	encode.cpp
 * 描述：	参见 encode.h
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2014-01-23
 ******************************************************************************/

#include "stdafx.h"
#include "encode.h"
#include "macros.h"
#include "log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


W2X_COMMON_API LPWSTR AllocStringA2W(
	LPCSTR _a_str,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_a_str, 0);

	if (0 >= _mbs_bytes) {
		_mbs_bytes = -1;
	}

	int w_str_count = 
		::MultiByteToWideChar(CP_ACP, 0, _a_str, -1, NULL, 0);  
	IF_FALSE_ASSERT (0 < w_str_count)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("convert the ASCII to UNICODE failed(%d)."),
			last_error);
		return 0;
	}

	WCHAR* w_buffer = new wchar_t[w_str_count + 1];
	IF_NULL_ASSERT_RETURN_VALUE(w_buffer, 0);

	::MultiByteToWideChar(CP_ACP, 0, _a_str, -1, w_buffer, w_str_count);

	w_buffer[w_str_count] = 0;

	return w_buffer;
}


W2X_COMMON_API LPSTR AllocStringW2A(
	LPCWSTR _w_str,
	int _wbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_w_str, 0);

	if (0 >= _wbs_bytes) {
		_wbs_bytes = -1;
	}
	int a_str_bytes = ::WideCharToMultiByte(
		CP_ACP, 0, _w_str, _wbs_bytes, NULL, 0, NULL, NULL);  
	IF_FALSE_ASSERT (0 < a_str_bytes)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("Convert the UNICODE to ASCII failed(%d)."),
			last_error);
		return 0;
	}

	LPSTR a_buffer = new char[a_str_bytes + 1];
	IF_NULL_ASSERT_RETURN_VALUE(a_buffer, 0);

	int w_str_count = 
		-1 == _wbs_bytes ? -1 : _wbs_bytes * sizeof(TCHAR);

	::WideCharToMultiByte(CP_ACP, 0, _w_str, 
		w_str_count, a_buffer, a_str_bytes, NULL, NULL);

	a_buffer[a_str_bytes] = 0;

	return a_buffer;
}


W2X_COMMON_API LPWSTR AllocStringUTF2W(
	LPCSTR _utf8_str,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_utf8_str, 0);

	if (0 >= _mbs_bytes) {
		_mbs_bytes = -1;
	}

	int w_str_count = ::MultiByteToWideChar(
		CP_UTF8, 0, _utf8_str, _mbs_bytes, NULL, 0);
	IF_FALSE_ASSERT (0 < w_str_count)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("convert the UTF-8 to UNICODE failed(%d)."),
			last_error);
		return 0;
	}

	LPWSTR w_buffer = new wchar_t[w_str_count + 1];
	IF_NULL_ASSERT_RETURN_VALUE(w_buffer, 0);

	::MultiByteToWideChar(CP_UTF8, 0, _utf8_str, 
		_mbs_bytes, w_buffer, w_str_count);
	w_buffer[w_str_count] = 0;

	return w_buffer;
}


W2X_COMMON_API LPSTR AllocStringW2UTF(
	LPCTSTR _w_str,
	int _wbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_w_str, 0);

	if (0 >= _wbs_bytes) {
		_wbs_bytes = -1;
	}

	int utf8_str_bytes = ::WideCharToMultiByte(
		CP_UTF8, 0, _w_str, -1, NULL, 0, NULL, NULL);  
	IF_FALSE_ASSERT (0 < utf8_str_bytes)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("Convert the UNICODE to ASCII failed(%d)."),
			last_error);
		return 0;
	}

	LPSTR utf8_buffer = new char[utf8_str_bytes + 1];
	IF_NULL_ASSERT_RETURN_VALUE(utf8_buffer, 0);

	int w_str_count = 
		-1 == _wbs_bytes ? -1 : _wbs_bytes * sizeof(TCHAR);

 	::WideCharToMultiByte(CP_UTF8, 0, _w_str, 
 		w_str_count, utf8_buffer, utf8_str_bytes, NULL, NULL);

	utf8_buffer[utf8_str_bytes] = 0;

	return utf8_buffer;
}


W2X_COMMON_API LPSTR AllocStringUTF2A(
	LPCSTR _utf8_str,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_utf8_str, 0);

	LPTSTR w_str = AllocStringUTF2W(_utf8_str, _mbs_bytes);
	LPSTR  a_str = AllocStringW2A(w_str);
	SAFE_DELETE_ARRAY(w_str);
	IF_NULL_ASSERT (a_str)
	{
		w2x::log::LogError(TEXT("convert the UTF-8 to ASCII failed(%d)."));
	}
	return a_str;
}


W2X_COMMON_API LPSTR AllocStringA2UTF(
	LPCSTR _a_str,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_a_str, 0);

	LPTSTR w_str = AllocStringA2W(_a_str);
	LPSTR utf8_str = AllocStringW2UTF(w_str);
	SAFE_DELETE_ARRAY(w_str);
	IF_NULL_ASSERT (utf8_str)
	{
		w2x::log::LogError(TEXT("convert the ASCII to UTF-8 failed(%d)."));
	}
	return utf8_str;
}

W2X_COMMON_API LPTSTR AllocString(LPCTSTR _t_str)
{
	if (NULL == _t_str) {
		_t_str = TEXT("");
	}
	const size_t len = _tcslen(_t_str);
	LPTSTR buffer = NULL;
	try {
		buffer = new TCHAR[len + 1];
		IF_NULL_ASSERT_RETURN_VALUE(buffer, NULL);
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:AllocString", MB_ICONERROR);
		return NULL;
	}
	memcpy(buffer, _t_str, len * sizeof(TCHAR));
	buffer[len] = TEXT('\0');
	return buffer;
}

W2X_COMMON_API bool FreeStringA(LPSTR* _a_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_a_pptr, false);
	SAFE_DELETE_ARRAY(*_a_pptr);
	return true;
}

W2X_COMMON_API bool FreeStringA(LPCSTR* _a_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_a_pptr, false);
	SAFE_DELETE_ARRAY(*_a_pptr);
	return true;
}

W2X_COMMON_API bool FreeStringW(LPWSTR* _w_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_w_pptr, false);
	SAFE_DELETE_ARRAY(*_w_pptr);
	return true;
}

W2X_COMMON_API bool FreeStringW(LPCWSTR* _w_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_w_pptr, false);
	SAFE_DELETE_ARRAY(*_w_pptr);
	return true;
}


W2X_COMMON_API bool Guid2String(
	OUT LPTSTR _str_buffer, 
	size_t _size_in_words, 
	GUID _guid
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_str_buffer, false);
	IF_FALSE_ASSERT_RETURN_VALUE(W2X_GUID_STR_LEN >= _size_in_words, false);

	_stprintf_s(
		_str_buffer, _size_in_words, 
		TEXT("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
		_guid.Data1, _guid.Data2, _guid.Data3, 
		_guid.Data4[0], _guid.Data4[1], _guid.Data4[2], _guid.Data4[3], 
		_guid.Data4[4], _guid.Data4[5], _guid.Data4[6], _guid.Data4[7]);

	return true;
}


W2X_COMMON_API bool GenerateFileMD5(
	OUT TSTDSTR _md5_digest,
	LPCTSTR _file_path
	)
{
	_md5_digest.clear();

	IF_NULL_ASSERT_RETURN_VALUE(_file_path, false);

	HANDLE file_handle = ::CreateFile(_file_path, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	IF_FALSE_ASSERT_RETURN_VALUE(INVALID_HANDLE_VALUE != file_handle, false);

	// 获得 CSP (Cryptographic Service Provider) 中一个密钥容器的句柄
	HCRYPTPROV prov_handle = NULL;
	IF_FALSE_ASSERT (FALSE != ::CryptAcquireContext(&prov_handle, 
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		::CloseHandle(file_handle);
		return false;
	}

	// 初始化数据流 hash，创建一个 CSP 的 hash 对象并返回它的句柄，
	// 这个句柄接下来将被 CryptHashData 调用来对数据流做 hash 运算。
	HCRYPTHASH hash_handle = NULL;
	IF_FALSE_ASSERT (FALSE != ::CryptCreateHash(
		prov_handle, CALG_MD5, NULL, 0, &hash_handle))
	{
		::CloseHandle(file_handle);
		::CryptReleaseContext(prov_handle, 0);

		return false;
	}

	bool is_generate_successed = true;
	const int STREAM_BUFFER_SIZE = 4096;
	DWORD bytes_read = 0;
	BYTE stream_buffer[STREAM_BUFFER_SIZE] = {0};

	do {
		IF_FALSE_ASSERT (FALSE != ::ReadFile(file_handle, 
			stream_buffer, STREAM_BUFFER_SIZE, &bytes_read, NULL))
		{
			is_generate_successed = false;
			goto generate_md5_digest_error;
		}

		if (0 == bytes_read)
		{
			break;
		}

		IF_FALSE_ASSERT (FALSE != ::CryptHashData(
			hash_handle, stream_buffer, bytes_read, 0))
		{
			is_generate_successed = false;
			goto generate_md5_digest_error;
		}
	} while (true);

	const DWORD HASH_DATA_SIZE = 32;
	bytes_read = HASH_DATA_SIZE;
	BYTE hash_data[HASH_DATA_SIZE] = {0};

	IF_FALSE_ASSERT (FALSE != ::CryptGetHashParam(
		hash_handle, HP_HASHVAL, hash_data, &bytes_read, 0))
	{
		TCHAR unit_str[3] = {0};
		for (DWORD i = 0; i < bytes_read; i++)
		{
			_sntprintf_s(unit_str, 2, TEXT("%02x"), hash_data[i]);
			_md5_digest.append(unit_str);
		}
	}

generate_md5_digest_error:
	::CryptDestroyHash(hash_handle);
	::CryptReleaseContext(prov_handle, 0);
	::CloseHandle(file_handle);

	return is_generate_successed;
}


W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END
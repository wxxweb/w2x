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


W2X_COMMON_API int Ascii2Unicode(
	OUT std::wstring& _unicode_str,
	LPCSTR _ascii_str)
{
	IF_NULL_ASSERT_RETURN_VALUE(_ascii_str, 0);

	int unicode_str_length = ::MultiByteToWideChar(CP_ACP, 0, _ascii_str, -1, NULL, 0);  
	IF_FALSE_ASSERT (0 < unicode_str_length)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("convert the ASCII to UNICODE faild(%d)."),
			last_error);
		return 0;
	}

	LPWSTR unicode_buffer = new wchar_t[unicode_str_length + 1];
	IF_NULL_ASSERT_RETURN_VALUE(unicode_buffer, 0);

	int ascii_str_bytes = strlen(_ascii_str) + 1;
	::MultiByteToWideChar(CP_ACP, 0, _ascii_str, 
		ascii_str_bytes, unicode_buffer, unicode_str_length); 
	unicode_buffer[unicode_str_length] = 0;

	_unicode_str = unicode_buffer;
	SAFE_DELETE_ARRAY(unicode_buffer);

	return unicode_str_length;
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
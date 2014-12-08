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


/** 将指定代码页的多字节字符串转换为宽字节字符串，可指定被转换的字节数 */
static inline LPWSTR _AllocStringMBS2WCS(
	UINT _code_page,
	LPCSTR _mbs,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_mbs, 0);

	if (0 >= _mbs_bytes) {
		_mbs_bytes = -1;
	}

	int wcs_count = 
		::MultiByteToWideChar(_code_page, 0, _mbs, _mbs_bytes, NULL, 0);  
	IF_FALSE_ASSERT (0 < wcs_count)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("convert the mbs(%u) to wcs failed(%u)."),
			_code_page, last_error);
		return 0;
	}

	WCHAR* wcs_buffer = new wchar_t[wcs_count + 1];
	IF_NULL_ASSERT_RETURN_VALUE(wcs_buffer, 0);

	::MultiByteToWideChar(_code_page, 0, _mbs, _mbs_bytes, wcs_buffer, wcs_count);

	wcs_buffer[wcs_count] = 0;

	return wcs_buffer;
}


/** 将宽字节字符串转换为指定代码页的多字节字符串，可指定被转换的字符数 */
static inline LPSTR _AllocStringWCS2MBS(
	UINT _code_page,
	LPCWSTR _wcs,
	int _wcs_char_count
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_wcs, 0);

	if (0 >= _wcs_char_count) {
		_wcs_char_count = -1;
	}
	int mbs_bytes = ::WideCharToMultiByte(
		_code_page, 0, _wcs, _wcs_char_count, NULL, 0, NULL, NULL);  
	IF_FALSE_ASSERT (0 < mbs_bytes)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("Convert the wcs to mbs(%u) failed(%u)."),
			_code_page, last_error);
		return 0;
	}

	LPSTR mbs_buffer = new char[mbs_bytes + 1];
	IF_NULL_ASSERT_RETURN_VALUE(mbs_buffer, 0);

	::WideCharToMultiByte(_code_page, 0, _wcs, 
		_wcs_char_count, mbs_buffer, mbs_bytes, NULL, NULL);

	mbs_buffer[mbs_bytes] = 0;

	return mbs_buffer;
}


/* Portable character check (remember EBCDIC). Do not use isalnum() because
   its behavior is altered by the current locale.
   See http://tools.ietf.org/html/rfc3986#section-2.3
*/
static bool Curl_isunreserved(unsigned char in)
{
  switch (in) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '-': case '.': case '_': case '~':
      return true;
    default:
      break;
  }
  return false;
}


static char *Curl_escape(const char *string, int inlength)
{
	size_t alloc = (inlength?(size_t)inlength:strlen(string))+1;
	char *ns;
	char *testing_ptr = NULL;
	unsigned char in; /* we need to treat the characters unsigned */
	size_t newlen = alloc;
	size_t strindex=0;
	size_t length;

	ns = new char[alloc];
	if(!ns)
		return NULL;

	length = alloc-1;
	while(length--) {
		in = *string;

		if(Curl_isunreserved(in))
			/* just copy this */
			ns[strindex++]=in;
		else {
			/* encode it */
			newlen += 2; /* the size grows with two, since this'll become a %XX */
			if(newlen > alloc) {
				alloc *= 2;
				testing_ptr = new char[alloc];
				if(!testing_ptr) {
					delete [] ns;
					return NULL;
				}
				else {
					memcpy_s(testing_ptr, alloc, ns, strindex);
					delete [] ns;
					ns = testing_ptr;
				}
			}

			_snprintf_s(&ns[strindex], 5, 4, "%%%02X", in);
			strindex+=3;
		}
		string++;
	}
	ns[strindex]=0; /* terminate it */
	return ns;
}

/** URL decodes the given string. */
static bool Curl_urldecode(const char *string, size_t length,
                    char **ostring, size_t *olen)
{
  size_t alloc = (length?length:strlen(string))+1;
  char *ns = (char *)malloc(alloc);
  unsigned char in;
  size_t strindex=0;
  unsigned long hex;

  if(!ns)
    return false;

  while(--alloc > 0) {
    in = *string;
    if(('%' == in) && (alloc > 2) &&
       isxdigit((int) ((unsigned char)string[1])) &&
	   isxdigit((int) ((unsigned char)string[2]))) {
      /* this is two hexadecimal digits following a '%' */
      char hexstr[3];
      char *ptr;
      hexstr[0] = string[1];
      hexstr[1] = string[2];
      hexstr[2] = 0;

      hex = strtoul(hexstr, &ptr, 16);

	  /* this long is never bigger than 255 anyway */
      in = (unsigned char)(hex & (unsigned long) 0xFF);

      string+=2;
      alloc-=2;
    }

    ns[strindex++] = in;
    string++;
  }
  ns[strindex]=0; /* terminate it */

  if(olen)
    /* store output size */
    *olen = strindex;

  if(ostring)
    /* store output string */
    *ostring = ns;

  return true;
}


W2X_COMMON_API LPSTR AllocStringA(LPCSTR _mbs, int _bytes)
{
	if (NULL == _mbs) {
		_mbs = "";
	}
	size_t copy_bytes = strlen(_mbs);
	if (-1 < _bytes && static_cast<size_t>(_bytes) < copy_bytes) {
		copy_bytes = static_cast<size_t>(_bytes);
	}
	LPSTR buffer = NULL;
	try {
		buffer = new char[copy_bytes + 1];
		IF_NULL_ASSERT_RETURN_VALUE(buffer, NULL);
	}
	catch (std::exception e) {
		MessageBoxA(NULL, e.what(), "w2x:AllocStringA", MB_ICONERROR);
		return NULL;
	}
	memcpy(buffer, _mbs, copy_bytes);
	buffer[copy_bytes] = '\0';
	return buffer;
}


W2X_COMMON_API LPWSTR AllocStringW(LPCWSTR _wcs, int _char_count)
{
	if (NULL == _wcs) {
		_wcs = L"";
	}
	size_t copy_count = wcslen(_wcs);
	if (-1 < _char_count && static_cast<size_t>(_char_count) < copy_count) {
		copy_count = static_cast<size_t>(_char_count);
	}
	LPWSTR buffer = NULL;
	try {
		buffer = new WCHAR[copy_count + 1];
		IF_NULL_ASSERT_RETURN_VALUE(buffer, NULL);
	}
	catch (std::exception e) {
		LPCWSTR error_msg = AllocStringA2W(e.what());
		MessageBoxW(NULL, 
			error_msg, L"w2x:AllocStringW", MB_ICONERROR);
		FreeStringW(&error_msg);
		return NULL;
	}
	memcpy(buffer, _wcs, copy_count * sizeof(WCHAR));
	buffer[copy_count] = L'\0';
	return buffer;
}


W2X_COMMON_API bool FreeStringA(LPSTR* _mbs_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_mbs_pptr, false);
	SAFE_DELETE_ARRAY(*_mbs_pptr);
	return true;
}

W2X_COMMON_API bool FreeStringA(LPCSTR* _mbs_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_mbs_pptr, false);
	SAFE_DELETE_ARRAY(*_mbs_pptr);
	return true;
}

W2X_COMMON_API bool FreeStringW(LPWSTR* _wcs_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_wcs_pptr, false);
	SAFE_DELETE_ARRAY(*_wcs_pptr);
	return true;
}

W2X_COMMON_API bool FreeStringW(LPCWSTR* _wcs_pptr)
{
	IF_NULL_ASSERT_RETURN_VALUE(_wcs_pptr, false);
	SAFE_DELETE_ARRAY(*_wcs_pptr);
	return true;
}


W2X_COMMON_API LPWSTR AllocStringM2W(
	UINT _code_page,
	LPCSTR _mbs,
	int _mbs_bytes
	)
{
	return _AllocStringMBS2WCS(_code_page, _mbs, _mbs_bytes);
}


W2X_COMMON_API LPWSTR AllocStringA2W(
	LPCSTR _mbs,
	int _mbs_bytes
	)
{
	return _AllocStringMBS2WCS(CP_ACP, _mbs, _mbs_bytes);
}


W2X_COMMON_API LPSTR AllocStringW2M(
	UINT _code_page,
	LPCWSTR _wcs,
	int _wcs_char_count
	)
{
	return _AllocStringWCS2MBS(_code_page, _wcs, _wcs_char_count);
}


W2X_COMMON_API LPSTR AllocStringW2A(
	LPCWSTR _wcs,
	int _wcs_char_count
	)
{
	return _AllocStringWCS2MBS(CP_ACP, _wcs, _wcs_char_count);
}


W2X_COMMON_API LPSTR AllocStringW2GBK(
	LPCWSTR _wcs,
	int _wcs_char_count
	)
{
	return _AllocStringWCS2MBS(936, _wcs, _wcs_char_count);
}


W2X_COMMON_API LPWSTR AllocStringGBK2W(
	LPCSTR _gbk,
	int _gbk_bytes
	)
{
	return _AllocStringMBS2WCS(936, _gbk, _gbk_bytes);
}


W2X_COMMON_API LPSTR AllocStringA2GBK(
	LPCSTR _mbs,
	int _mbs_bytes
	)
{
	if (936 == ::GetACP()) {
		return AllocStringA(_mbs, _mbs_bytes);
	}
	LPWSTR wcs = _AllocStringMBS2WCS(CP_ACP, _mbs, _mbs_bytes);
	LPSTR gbk = _AllocStringWCS2MBS(936, wcs, -1);
	SAFE_DELETE_ARRAY(wcs);
	return gbk;
}


W2X_COMMON_API LPSTR AllocStringGBK2A(
	LPCSTR _gbk,
	int _gbk_bytes
	)
{
	if (936 == ::GetACP()) {
		return AllocStringA(_gbk, _gbk_bytes);
	}
	LPWSTR wcs = _AllocStringMBS2WCS(936, _gbk, _gbk_bytes);
	LPSTR mbs = _AllocStringWCS2MBS(CP_ACP, wcs, -1);
	SAFE_DELETE_ARRAY(wcs);
	return mbs;
}


W2X_COMMON_API LPSTR AllocStringUTF2GBK(
	LPCSTR _utf8,
	int _utf8_bytes
	)
{
	LPWSTR wcs = _AllocStringMBS2WCS(CP_UTF8, _utf8, _utf8_bytes);
	LPSTR mbs = _AllocStringWCS2MBS(936, wcs, -1);
	SAFE_DELETE_ARRAY(wcs);
	return mbs;
}


W2X_COMMON_API LPSTR AllocStringGBK2UTF(
	LPCSTR _gbk,
	int _gbk_bytes
	)
{
	LPWSTR wcs = _AllocStringMBS2WCS(936, _gbk, _gbk_bytes);
	LPSTR mbs = _AllocStringWCS2MBS(CP_UTF8, wcs, -1);
	SAFE_DELETE_ARRAY(wcs);
	return mbs;
}


W2X_COMMON_API LPWSTR AllocStringUTF2W(
	LPCSTR _utf8_str,
	int _utf8_bytes
	)
{
	return _AllocStringMBS2WCS(CP_UTF8, _utf8_str, _utf8_bytes);
}


W2X_COMMON_API LPSTR AllocStringW2UTF(
	LPCTSTR _wcs,
	int _wcs_char_count
	)
{
	return _AllocStringWCS2MBS(CP_UTF8, _wcs, _wcs_char_count);
}


W2X_COMMON_API LPSTR AllocStringUTF2A(
	LPCSTR _utf8_str,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_utf8_str, 0);

	LPTSTR wcs = _AllocStringMBS2WCS(CP_UTF8, _utf8_str, _mbs_bytes);
	LPSTR  mbs = _AllocStringWCS2MBS(CP_ACP, wcs, -1);
	SAFE_DELETE_ARRAY(wcs);
	IF_NULL_ASSERT (mbs)
	{
		w2x::log::LogError(TEXT("convert the UTF-8 to ASCII failed(%d)."));
	}
	return mbs;
}


W2X_COMMON_API LPSTR AllocStringA2UTF(
	LPCSTR _mbs,
	int _mbs_bytes
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_mbs, 0);

	LPTSTR wcs = _AllocStringMBS2WCS(CP_ACP, _mbs, _mbs_bytes);
	LPSTR utf8_str = _AllocStringWCS2MBS(CP_UTF8, wcs, -1);
	SAFE_DELETE_ARRAY(wcs);
	IF_NULL_ASSERT (utf8_str)
	{
		w2x::log::LogError(TEXT("convert the ASCII to UTF-8 failed(%d)."));
	}
	return utf8_str;
}


W2X_COMMON_API LPSTR AllocStringUrlEncodeA(LPCSTR _mbs)
{
	LPSTR utf_str = AllocStringA2UTF(_mbs);
	LPSTR utf_str_encode = Curl_escape(utf_str, 0);
	FreeStringA(&utf_str);
	return AllocStringUTF2A(utf_str_encode);
}


W2X_COMMON_API LPWSTR AllocStringUrlEncodeW(LPCWSTR _wcs)
{
	LPSTR utf_str = _AllocStringWCS2MBS(936, _wcs, -1);
	LPSTR utf_str_encode = Curl_escape(utf_str, 0);
	FreeStringA(&utf_str);
	return AllocStringUTF2W(utf_str_encode);
}


W2X_COMMON_API LPSTR AllocStringUrlEncodeUTF8(LPCSTR _utf8_str)
{
	return Curl_escape(_utf8_str, 0);
}


W2X_COMMON_API LPSTR AllocStringUrlDecodeA(LPCSTR _mbs)
{
	char* out_str = NULL;
	size_t out_len = 0;

	LPSTR utf_str = AllocStringA2UTF(_mbs);
	Curl_urldecode(utf_str, 0, &out_str, &out_len);
	FreeStringA(&utf_str);
	return AllocStringUTF2A(out_str);
}


W2X_COMMON_API LPWSTR AllocStringUrlDecodeW(LPCWSTR _wcs)
{
	char* out_str = NULL;
	size_t out_len = 0;

	LPSTR utf_str = _AllocStringWCS2MBS(936, _wcs, -1);
	Curl_urldecode(utf_str, 0, &out_str, &out_len);
	FreeStringA(&utf_str);
	return AllocStringUTF2W(out_str);
}


W2X_COMMON_API LPSTR AllocStringUrlDecodeUTF8(LPCSTR _utf8_str)
{
	char* out_str = NULL;
	size_t out_len = 0;
	Curl_urldecode(_utf8_str, 0, &out_str, &out_len);
	return out_str;
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
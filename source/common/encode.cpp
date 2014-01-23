/******************************************************************************
 * 文件：	encode.cpp
 * 描述：	参见 encode.h
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2014-01-23
 ******************************************************************************/

#include "stdafx.h"
#include "encode.h"
#include "log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


W2X_COMMON_API int Ascii2Unicode(
	OUT std::wstring& _unicode_str,
	LPCSTR _ascii_str,
	int _ascii_str_bytes)
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

	::MultiByteToWideChar(CP_ACP, 0, _ascii_str, 
		_ascii_str_bytes, unicode_buffer, unicode_str_length); 
	unicode_buffer[unicode_str_length] = 0;

	_unicode_str = unicode_buffer;
	SAFE_DELETE_ARRAY(unicode_buffer);

	return unicode_str_length;
}


W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END
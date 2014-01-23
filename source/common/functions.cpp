/******************************************************************************
 * 文件：	functions.cpp
 * 描述：
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2013-01-21
 ******************************************************************************/

#include "stdafx.h"
#include "functions.h"
#include "macros.h"
#include "log.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(fun)


int Ascii2Unicode(OUT std::wstring& unicode_str,
				  LPCSTR ascii_str,
				  int ascii_str_bytes)
{
	IF_NULL_ASSERT_RETURN_VALUE(ascii_str, 0);

	int unicode_str_length = ::MultiByteToWideChar(CP_ACP, 0, ascii_str, -1, NULL, 0);  
	IF_FALSE_ASSERT (0 < unicode_str_length)
	{
		const DWORD last_error = ::GetLastError();
		w2x::log::LogError(TEXT("convert the ASCII to UNICODE faild(%d)."),
			last_error);
		return 0;
	}

	LPWSTR unicode_buffer = new wchar_t[unicode_str_length + 1];
	IF_NULL_ASSERT_RETURN_VALUE(unicode_buffer, 0);

	::MultiByteToWideChar(CP_ACP, 0, ascii_str, 
		ascii_str_bytes, unicode_buffer, unicode_str_length); 
	unicode_buffer[unicode_str_length] = 0;

	unicode_str = unicode_buffer;
	SAFE_DELETE_ARRAY(unicode_buffer);

	return unicode_str_length;
}


W2X_DEFINE_NAME_SPACE_END(fun)
W2X_NAME_SPACE_END
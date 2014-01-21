/******************************************************************************
 * 文件：	functions.h
 * 描述：
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2013-01-21
 ******************************************************************************/

#ifndef __W2X_COMMON_FUNCTIONS_H__
#define __W2X_COMMON_FUNCTIONS_H__

#include "exports.h"
#include <WTypes.h>
#include <string>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(fun)

// 将 ASCII 字符编码转换为 UNICODE 字符串编码，
// 成功返回转换的 UNICODE 字符数，失败返回 0
W2X_COMMON_API int Ascii2Unicode(
	OUT std::wstring& unicode_str, 
	LPCSTR ascii_str, 
	int ascii_str_bytes
);

W2X_DEFINE_NAME_SPACE_END(fun)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_FUNCTIONS_H__ */
/******************************************************************************
 * 文件：	encode.h
 * 描述：	定义与编码相关的函数、常量等，如字符编码转换、二进制数据转码等。
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2014-01-23
 ******************************************************************************/

#ifndef __W2X_COMMON_ENCODE_H__
#define __W2X_COMMON_ENCODE_H__

#include "exports.h"
#include "macros.h"
#include <WTypes.h>
#include <string>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


// 将 ASCII 字符编码转换为 UNICODE 字符串编码，
// 成功返回转换的 UNICODE 字符数，失败返回 0
W2X_COMMON_API int Ascii2Unicode(
	OUT std::wstring& _unicode_str, 
	LPCSTR _ascii_str, 
	int _ascii_str_bytes
);


// 生成文件 MD5 消息摘要
W2X_COMMON_API bool GenerateFileMD5(
	OUT TSTDSTR _md5_digest,
	LPCTSTR _file_path
);

W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_ENCODE_H__ */
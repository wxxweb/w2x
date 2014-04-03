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
#include <memory>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


enum EncodeType {
	kAscii,
	kUnicode,
	kUtf8,
};

/** 将 ASCII 字符编码转换为 UNICODE 字符串编码 */
W2X_COMMON_API LPWSTR AllocStringA2W(
	LPCSTR _a_str,
	int _mbs_bytes = -1
);

#define W2X_ALLOC_STR_A2W		w2x::encode::AllocStringA2W

#ifdef UNICODE
#  define W2X_ALLOC_STR_A2T		w2x::encode::AllocStringA2W
#else
#  define W2X_ALLOC_STR_A2T(s)	(s)
#endif

/** 将 UNICODE 字符编码转换为 ASCII 字符串编码 */
W2X_COMMON_API LPSTR AllocStringW2A(
	LPCWSTR _w_str,
	int _wbs_bytes = -1
);

#define W2X_ALLOC_STR_W2A		w2x::encode::AllocStringW2A

#ifdef UNICODE
#  define W2X_ALLOC_STR_T2A		w2x::encode::AllocStringW2A
#else
#  define W2X_ALLOC_STR_T2A(s)	(s)
#endif

/** 将 UTF-8 字符编码转换为 ASCII 字符串编码 */
W2X_COMMON_API LPSTR AllocStringUTF2A(
	LPCSTR _utf8_str,
	int _mbs_bytes = -1
);

/** 将 UTF-8 字符编码转换为 UNICODE 字符串编码 */
W2X_COMMON_API LPWSTR AllocStringUTF2W(
	LPCSTR _utf8_str,
	int _mbs_bytes = -1
);

#define W2X_ALLOC_STR_UTF2A		w2x::encode::AllocStringUTF2A
#define W2X_ALLOC_STR_UTF2W		w2x::encode::AllocStringUTF2W

#ifdef UNICODE
#  define W2X_ALLOC_STR_UTF2T	w2x::encode::AllocStringUTF2W
#else
#  define W2X_ALLOC_STR_UTF2T	w2x::encode::AllocStringUTF2A
#endif

/** 将 UNICODE 字符编码转换为 UTF-8 字符串编码 */
W2X_COMMON_API LPSTR AllocStringW2UTF(
	LPCTSTR _w_str,
	int _wbs_bytes = -1
);

/** 将 ASCII 字符编码转换为 UTF-8 字符串编码 */
W2X_COMMON_API LPSTR AllocStringA2UTF(
	LPCSTR _a_str,
	int _mbs_bytes = -1
);

#define W2X_ALLOC_STR_A2UTF		w2x::encode::AllocStringA2UTF
#define W2X_ALLOC_STR_W2UTF		w2x::encode::AllocStringW2UTF

#ifdef UNICODE
#  define W2X_ALLOC_STR_T2UTF	w2x::encode::AllocStringW2UTF
#else
#  define W2X_ALLOC_STR_T2UTF	w2x::encode::AllocStringA2UTF
#endif

/** 单纯的拷贝一个字符串到栈中，如果传入 NULL，返回带结束符的空字符串 */
W2X_COMMON_API LPTSTR AllocString(LPCTSTR _t_str);

/** 是否由 AllocStringXXX 分配的多字节字符串栈空间 */
W2X_COMMON_API bool FreeStringA(LPSTR* _a_pptr);
W2X_COMMON_API bool FreeStringA(LPCSTR* _a_pptr);

/** 是否由 AllocStringXXX 分配的宽字节字符串栈空间 */
W2X_COMMON_API bool FreeStringW(LPWSTR* _w_pptr);
W2X_COMMON_API bool FreeStringW(LPCWSTR* _w_pptr);

#define W2X_FREE_STR_A(p)	w2x::encode::FreeStringA(&(p))
#define W2X_FREE_STR_W(p)	w2x::encode::FreeStringW(&(p))

#ifdef UNICODE
#  define W2X_FREE_STR(p)	w2x::encode::FreeStringW(&(p))
#else
#  define W2X_FREE_STR(p)	w2x::encode::FreeStringA(&(p))
#endif


/**
 * 将 16字节(128位) 的 GUID 转换成字符串表现形式，例如：
 * BF3EE013-F387-4BCA-9599-581546EAB6DC，
 * 该字符串包含 36 字符（32 个十六进制字符 + 4 个连字符，不含结束符 '\0'）
 */
W2X_COMMON_API bool Guid2String(
	OUT LPTSTR _str_buffer, 
	size_t _size_in_words, 
	GUID _guid
);

#ifndef W2X_GUID_STR_LEN
#  define W2X_GUID_STR_LEN	37
#endif

/*
 * 生成文件 MD5 消息摘要
 */
W2X_COMMON_API bool GenerateFileMD5(
	OUT TSTDSTR _md5_digest,
	LPCTSTR _file_path
);


W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_ENCODE_H__ */
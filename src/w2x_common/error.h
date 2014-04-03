/******************************************************************************
 * 文件:		error.h
 * 描述:		自定义错误编码，并提供对错误编码进行格式化输出。
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 日期:		2014-04-01
 ******************************************************************************/

#ifndef __W2X_COMMON_ERROR_H__
#define __W2X_COMMON_ERROR_H__

#include "exports.h"
#include "macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)


/* 将系统给出的出错编码 _error_code 格式化成出错描述信息，
 * 保存在字符串缓存 _msg_buffer 中，成功则返回保存的描述
 * 信息的字符个数（不包括 NULL），否则返回 0。
 */
W2X_COMMON_API size_t FormatError(
	LPTSTR _msg_buffer,
	size_t _size_in_words,
	DWORD _error_code
);


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_ERROR_H__ */
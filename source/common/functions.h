/******************************************************************************
 * �ļ���	functions.h
 * ������
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2013-01-21
 ******************************************************************************/

#ifndef __W2X_COMMON_FUNCTIONS_H__
#define __W2X_COMMON_FUNCTIONS_H__

#include "exports.h"
#include <WTypes.h>
#include <string>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(fun)


// �� ASCII �ַ�����ת��Ϊ UNICODE �ַ������룬
// �ɹ�����ת���� UNICODE �ַ�����ʧ�ܷ��� 0
W2X_COMMON_API int Ascii2Unicode(
	OUT std::wstring& unicode_str, 
	LPCSTR ascii_str, 
	int ascii_str_bytes
);


W2X_DEFINE_NAME_SPACE_END(fun)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_FUNCTIONS_H__ */
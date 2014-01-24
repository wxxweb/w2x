/******************************************************************************
 * �ļ���	encode.h
 * ������	�����������صĺ����������ȣ����ַ�����ת��������������ת��ȡ�
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2014-01-23
 ******************************************************************************/

#ifndef __W2X_COMMON_ENCODE_H__
#define __W2X_COMMON_ENCODE_H__

#include "exports.h"
#include "macros.h"
#include <WTypes.h>
#include <string>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


// �� ASCII �ַ�����ת��Ϊ UNICODE �ַ������룬
// �ɹ�����ת���� UNICODE �ַ�����ʧ�ܷ��� 0
W2X_COMMON_API int Ascii2Unicode(
	OUT std::wstring& _unicode_str, 
	LPCSTR _ascii_str, 
	int _ascii_str_bytes
);


W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_ENCODE_H__ */
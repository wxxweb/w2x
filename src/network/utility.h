/*******************************************************************************
 * �ļ���	utility.h
 * ������	����һЩ��������ص�ʵ�õĺ������ꡢ�����ȡ�
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-04-05
 ******************************************************************************/

#ifndef __W2X_NETWORK_UTILITY_H__
#define __W2X_NETWORK_UTILITY_H__

#include "exports.h"
#include "..\common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


enum {
	MAX_IP_ADDR_STR = 16,	// IP ��ַ�ַ�������������ַ���
};

/*
 * �ж�һ���ַ����Ƿ�Ϊ��Ч�� IP ��ַ����Ч���� true, ���򷵻� false��
 */
W2X_NETWORK_API bool IsValidIpAddress(
	LPCTSTR _ip_addr_str
);

/*
 * ��һ���ַ�����ʽ����������������������ֵ�� IP ��ַ����Ч���� true, ���򷵻� false��
 */
W2X_NETWORK_API bool ParseHostToIpAddress(
	OUT DWORD& _ip_address,
	LPCTSTR _host
);

/*
 * ��IP��ַ���л�ȡ����IP��ַ���������룬
 * �����м�������������㲥IP��ַ��
 * _ip_address ��Ź㲥��ַ��
 * ���ɹ����� true, ���򷵻� false��
 */
W2X_NETWORK_API bool GetBroadcastIpAddress(
	OUT DWORD& _ip_address
);

/*
 * ���� IP ��ַ�ַ������ַ�����������С _size_in_words ����С�� MAX_IP_ADDR_STR��
 */
W2X_NETWORK_API int ParseIpAddressString(
	OUT	LPTSTR _str_buffer,
	const DWORD _size_in_words,
	const DWORD _ip_address
);


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_UTILITY_H__ */
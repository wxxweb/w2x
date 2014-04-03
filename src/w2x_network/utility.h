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
#include "..\w2x_common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)


enum {
	MAX_IP_ADDR_STR = 16,	// IP ��ַ�ַ�������������ַ���
};


struct W2X_IP_ADDR_STR {
	TCHAR str[MAX_IP_ADDR_STR];
};


/** �������ֽ��� IP ��ַת��Ϊ�ַ�����ʽ */
W2X_NETWORK_API void IpAddr2Str(
	OUT	W2X_IP_ADDR_STR& _str,
	DWORD _ip_addr
);


/** �� IP ��ַ�ַ���ת��Ϊ�����ֽ��� IP ��ַ */
W2X_NETWORK_API bool Str2IpAddr(
	OUT DWORD& _ip_addr,
	IN const W2X_IP_ADDR_STR& _str
);


/** 
 * ��������(����)�� IP ��ַ�ַ��������������ֽ��� IP ��ַ��
 * ��Ϊ������Ҫ���� DNS �������Ƚ�����
 */
W2X_NETWORK_API bool Name2IpAddr(
	OUT DWORD& _ip_addr,
	LPCTSTR _name
);


/*
 * ��ȡ��ǰ���ڹ����� IP ��ַ��
 * ���ɹ����� true, ���򷵻� false��
 */
W2X_NETWORK_API bool GetWorkIpAddr(
	OUT DWORD& _ip_address
);

/*
 * ��IP��ַ���л�ȡ����IP��ַ���������룬
 * �����м�������������㲥IP��ַ��
 * _ip_address ��Ź㲥��ַ��
 * ���ɹ����� true, ���򷵻� false��
 */
W2X_NETWORK_API bool GetBroadcastIpAddr(
	OUT DWORD& _ip_addr
);


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_UTILITY_H__ */
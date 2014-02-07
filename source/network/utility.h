/*******************************************************************************
 * 文件：	utility.h
 * 描述：	定义一些与网络相关的实用的函数、宏、常量等。
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-04-05
 ******************************************************************************/

#ifndef __W2X_NETWORK_UTILITY_H__
#define __W2X_NETWORK_UTILITY_H__

#include "exports.h"
#include "..\common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


enum {
	MAX_IP_ADDR_STR = 16,	// IP 地址字符串缓冲区最大字符数
};

/*
 * 从IP地址表中获取本机IP地址和子网掩码，
 * 并从中计算出本局域网广播IP地址。
 * _ip_address 存放广播地址。
 * 若成功返回 true, 否则返回 false。
 */
W2X_NETWORK_API bool GetBroadcastIpAddress(
	OUT DWORD& _ip_address
);

/*
 * 解析 IP 地址字符串，字符串缓冲区大小 _size_in_words 不得小于 MAX_IP_ADDR_STR。
 */
W2X_NETWORK_API int ParseIpAddressString(
	OUT	LPTSTR _str_buffer,
	const DWORD _size_in_words,
	const DWORD _ip_address
);


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_UTILITY_H__ */
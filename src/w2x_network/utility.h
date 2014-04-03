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
#include "..\w2x_common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)


enum {
	MAX_IP_ADDR_STR = 16,	// IP 地址字符串缓冲区最大字符数
};


struct W2X_IP_ADDR_STR {
	TCHAR str[MAX_IP_ADDR_STR];
};


/** 将网络字节序 IP 地址转换为字符串形式 */
W2X_NETWORK_API void IpAddr2Str(
	OUT	W2X_IP_ADDR_STR& _str,
	DWORD _ip_addr
);


/** 将 IP 地址字符串转换为网络字节序 IP 地址 */
W2X_NETWORK_API bool Str2IpAddr(
	OUT DWORD& _ip_addr,
	IN const W2X_IP_ADDR_STR& _str
);


/** 
 * 将主机名(域名)或 IP 地址字符串解析成网络字节序 IP 地址。
 * 若为域名需要进行 DNS 解析，比较慢。
 */
W2X_NETWORK_API bool Name2IpAddr(
	OUT DWORD& _ip_addr,
	LPCTSTR _name
);


/*
 * 获取当前正在工作的 IP 地址。
 * 若成功返回 true, 否则返回 false。
 */
W2X_NETWORK_API bool GetWorkIpAddr(
	OUT DWORD& _ip_address
);

/*
 * 从IP地址表中获取本机IP地址和子网掩码，
 * 并从中计算出本局域网广播IP地址。
 * _ip_address 存放广播地址。
 * 若成功返回 true, 否则返回 false。
 */
W2X_NETWORK_API bool GetBroadcastIpAddr(
	OUT DWORD& _ip_addr
);


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_UTILITY_H__ */
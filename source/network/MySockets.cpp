//******************************************************************************
// File Name		: MySockets.cpp
// Description		: 自定义的Sockets函数，提供给FindOthers和echoServer调用。
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:56 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/


#include "StdAfx.h"		// 标准头文件
#include "MyDefine.h"
#include "HandleError.h"
#include "MySockets.h"


/**********************************************************
   methor 		: initSocket
   description	: 初始化WinSock DLL。
   parameters	: 无
   return value	: 若成功则返回TRUE; 若失败则释放WinSock
  				  资源并返回FALSE。
***********************************************************/
extern BOOL 
initSocket(
	void
	)
{
	int		iRetVal;
	WORD	wVersion;
	WSADATA	wsaData;		// 初始化数据

	wVersion = MAKEWORD(2, 2);						// 取得WinSock DLL版本号2.2
	iRetVal = WSAStartup(wVersion, &wsaData);		// 初始化WinSock DLL 2.2

	/* 初始化失败 */
	if (NO_ERROR != iRetVal)
	{
		putLastError();		// 输出系统出错消息
		WSACleanup();		// 释放WinSock DLL资源
		return FALSE;
	}

	/* 初始化成功 */
	return TRUE;
} /* initSocket */


/**********************************************************
   methor 		: releaseSocket
   description	: 释放WinSock资源。
   parameters	: 
	[IN]	localSocket:SOCKET
  				-- 本地Socket。

   return value	: 无
***********************************************************/
extern void
releaseSocket(
	IN	const SOCKET localSocket  
	)
{
	shutdown(localSocket, SD_BOTH);		// 停止发送数据
	closesocket(localSocket);			// 关闭Socket
	WSACleanup();						// 释放WinSock DLL
}


/**********************************************************
   methor 		: createUDPSocket
   description	: 创建用于收发UDP数据包的本地Socket。
   parameters	: 
  	[OUT]	localSocket:SOCKET * 
  				-- 存放创建的本地Socket。
  
   return value	: 若成功则返回TRUE; 若失败则释放WinSock
  				  资源并返回FALSE。
**********************************************************/
extern BOOL
createUDPSocket(
	OUT	SOCKET * pLocalSocket
	)
{
	/* 创建Socket */
	*pLocalSocket = WSASocket(AF_INET,SOCK_DGRAM, IPPROTO_UDP, 
							NULL, 0, WSA_FLAG_OVERLAPPED);

	/* 创建失败 */
	if (INVALID_SOCKET == *pLocalSocket)
	{
		putLastError();		// 输出系统出错消息
		WSACleanup();		// 释放WinSock DLL资源
		return FALSE;
	}

	/* 创建成功 */
	return TRUE;
} /* createUDPSocket */


/**********************************************************
   methor 		: bindLocalAddr
   description	: 将本地Socket绑定到本机地址。
   parameters	: 
  	[IN]	localSocket:SOCKET
  				-- 用于收发UDP数据包的本地Socket。
  	[IN]	 wPort:WORD 
  				-- 端口号，为0则自动分配1024-5000间端口号。
  
   return value	: 若成功则返回TRUE; 若失败则释放Socket资源
  				  并释放WinSock资源并返回FALSE。
***********************************************************/
extern BOOL 
bindLocalAddr(
	IN	const SOCKET localSocket,
	IN	const WORD wPort
	)
{
	int			iRetVal;
	SOCKADDR_IN	saLocalAddr;

	/* 配置本地地址信息 */
	saLocalAddr.sin_family		= AF_INET;
	saLocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// 自动分配主机地址
	saLocalAddr.sin_port		= htons(wPort);			// 分配端口号

	/* 绑定Socket */
	iRetVal = bind(localSocket, (sockaddr *)&saLocalAddr, sizeof(SOCKADDR));

	/* 绑定失败 */
	if (SOCKET_ERROR == iRetVal)	
	{
		putLastError();					// 输出系统出错消息
		releaseSocket(localSocket);		// 释放Socket资源	
		return FALSE;					// 返回错误代码
	}

	return TRUE;
} /* bindLocalAddr */


/**********************************************************
   methor 		: sycRecvUDPPacket
   description	: 同步接收UDP数据包.
   parameters	: 
  	[IN]	localSocket : SOCKET 
  				-- 用于收发UDP数据包的本地Socket。
  	[OUT]	psaRemoteAddr : PSOCKADDR_IN 
  				-- 存放远程主机地址。
  	[OUT]	lpstrRecvBuf : LPSTR 
  				-- 存放接收到的数据。
	[OUT]	dwBytesRecv : DOWRD 
  				-- 接收数据字节数。

  return value	: 若接收到数据包，则返回数据字节数，若接收
				  过程中出错则返回SOCKET_ERROR。
***********************************************************/
extern int
sycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecv
	)
{
	static int	iRetVal;
	static int	cRemoteAddrSize;	// 远程主机地址结构字节数

	cRemoteAddrSize = sizeof(SOCKADDR);

	/* 同步接收UDP数据包 */ 
	iRetVal = recvfrom(localSocket, lpstrRecvBuf,  dwBytesRecv, 
						0, (PSOCKADDR)psaRemoteAddr,  &cRemoteAddrSize);

	/* 接收失败 */
	if (SOCKET_ERROR == iRetVal)
	{
		putLastError();						// 输出系统出错消息
		releaseSocket(localSocket);			// 释放Socket资源
		return SOCKET_ERROR;				// 返回错误代码
	}

	/* 接收成功 */
	return iRetVal;
} /* sycRecvUDPPacket */


/**********************************************************
   methor 		: asycRecvUDPPacket
   description	: 异步接收UDP数据包。
   parameters	: 
  	[IN]	localSocket : SOCKET 
				-- 用于收发UDP数据包的本地Socket。
  	[OUT]	psaRemoteAddr : PSOCKADDR_IN 
  				-- 存放远程主机地址。
  	[OUT]	lpstrRecvBuf : LPSTR 
  				-- 存放接收到的数据。
	[IN]	dwBytesRecvBuf : DOWRD 
  				-- 接收数据包的缓冲区字节数。

   return value	: 若接收到数据包则返回TRUE，若接收过程中
				  出错则返回FALSE。
**********************************************************/
extern BOOL 
asycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecvBuf
	)
{
	static int				cRemoteAddrSize;			// 远程主机地址结构字节数
	static int				iRetVal;					// 返回值
	static DWORD			dwFlags = 0;				// WSARecvFrom的Flags参数
	static DWORD			dwBytesRecv;				// 要接收的字节数，异步可忽略
	static WSABUF			wsaDataBuf;					// 数据缓存
	static WSAOVERLAPPED	wsaOverlapped;				// 重叠结构
		
	wsaDataBuf.len = dwBytesRecvBuf;
	wsaDataBuf.buf = lpstrRecvBuf;
	cRemoteAddrSize = sizeof(SOCKADDR);
	ZeroMemory(&wsaOverlapped, sizeof(WSAOVERLAPPED));	// 必须置空，否则会出错

	/* 异步接收UDP消息 */
	iRetVal = WSARecvFrom(localSocket, &wsaDataBuf, 1, &dwBytesRecv, 
						   &dwFlags, (PSOCKADDR)psaRemoteAddr,
							&cRemoteAddrSize, &wsaOverlapped,  NULL);

	if(SOCKET_ERROR == iRetVal)
	{
		/* 接收过程中出错 */
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			putLastError();						// 输出系统出错消息
			releaseSocket(localSocket);			// 释放Socket资源
			return FALSE;
		}
	}

	/* 接收到回应消息 */
	return TRUE;
} /* asycRecvUDPPacket */


/**********************************************************
   methor 		: sendUDPPacket
   description	: 发送UDP数据包。
   parameters	: 
  	[IN]	localSocket:SOCKET
				-- 用于收发UDP数据包的本地Socket。
  	[IN]	saRemoteAddr:SOCKADDR_IN 
				-- 远程主机地址。
  	[IN]	lpszSendBuf : LPSTR 
				-- 要发送数据的缓冲区。
	[IN]	dwBytesSendBuf : DWORD
				-- 要发送的字节数。

   return value	: 若成功则返回发送字节数，若失败则释放
  				  Socket和WinSock资源并返回SOCKET_ERROR。
**********************************************************/
extern int
sendUDPPacket(
	IN	const SOCKET localSocket,	
	IN	const PSOCKADDR_IN psaRemoteAddr,
	IN	const LPSTR lpstrSendBuf,
	IN	const DWORD dwBytesSendBuf
	)
{
	int iRetVal;

	/* 发送消息 */
	iRetVal = sendto(localSocket, lpstrSendBuf, dwBytesSendBuf, 0, 
				     (PSOCKADDR)psaRemoteAddr, sizeof(SOCKADDR));
	
	/* 发送失败 */
	if (SOCKET_ERROR == iRetVal)
	{
		putLastError();						// 输出系统错误信息
		releaseSocket(localSocket);			// 释放Socket资源
		return SOCKET_ERROR;				// 返回错误代码
	}

	/* 发送成功 */
	return iRetVal;
} /* sendUDPPacket */


/**********************************************************
   methor 		: getBCastIPAddr
   description	: 从IP地址表中获取本机IP地址和子网掩码，
				  并从中计算出本局域网广播IP地址。
   parameters	: 
  	[OUT]	pdwAddr:PDWORD 
			-- 存放广播地址。

   return value	: 若成功返回TRUE, 否则返回FALSE。
***********************************************************/
extern BOOL
getBCastIPAddr(
	OUT	PDWORD pdwBCastIPAddr
	)
{
	int		iRetVal;
	DWORD	dwSize = 0;
	DWORD	dwIPAddr;				// IP地址
	DWORD	dwMask;					// 子网掩码
	PMIB_IPADDRTABLE pIPAddrTable;	// IP地址表结构

	pIPAddrTable = (MIB_IPADDRTABLE*)GlobalAlloc(GMEM_FIXED, 
			sizeof(MIB_IPADDRTABLE));					// 为IP地址表分配内存

	/* IP地址表结构内存分配成功 */
	if (pIPAddrTable)
	{	
		iRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0); // 获取IP地址表

		/* 分配给IP地址表结构的缓存不足 */
		if (ERROR_INSUFFICIENT_BUFFER == iRetVal)
		{	
			/* 重新分配内存 */
			GlobalFree(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *)GlobalAlloc(GMEM_FIXED, dwSize);
		}
	}
	/* IP地址表结构内存分配失败 */
	else 
	{	
		putLastError();		// 输出系统错误消息
		return FALSE;
	}

	/* IP地址表结构内存重新分配成功 */
	if (pIPAddrTable)
	{
		/* IP地址表获取成功 */
		if (NO_ERROR == GetIpAddrTable(pIPAddrTable, &dwSize, 0))
		{
			dwIPAddr = pIPAddrTable->table[0].dwAddr;				// 获取IP地址
			dwMask = pIPAddrTable->table[0].dwMask;					// 获得子网掩码
			*pdwBCastIPAddr = (dwIPAddr & dwMask) | ~dwMask;		// 计算广播地址
			GlobalFree(pIPAddrTable);								// 释放内存空间
		}
		/* IP地址表获取失败 */
		else
		{
			putLastError();	// 输出系统错误消息
			return FALSE;
		}
	}
	// IP地址表内存重新分配失败
	else
	{
		putLastError();		// 输出系统错误消息
		return FALSE;
	}

	/* 成功获得本局域网广播地址 */
	return TRUE;
} /* getBCastIPAddr */


/**********************************************************
   methor 		: enableBCast
   description	: 启用Socket广播功能。
   parameters	: 
  	[IN]	localSocket:SOCKET 
			-- 用于收发UDP数据包的本地Socket。

   return value	: 若成功则返回TRUE，若失败则释放Socket
  				  和WinSock资源并返回FALSE。
**********************************************************/
extern BOOL
enableBCast(
	IN	const SOCKET localSocket,
	OUT	PDWORD pdwBCastIPAddr
	)
{
	BOOL	bBroadcast = TRUE;			// setsockopt最后一参数
	int		iRetVal;

	/* 启用Socket广播功能 */
	iRetVal = setsockopt(localSocket, SOL_SOCKET, SO_BROADCAST, 
						  (char *)&bBroadcast,  sizeof(int));
	/* 启用失败 */
	if (SOCKET_ERROR == iRetVal)
	{
		putLastError();					// 输出系统出错消息
		releaseSocket(localSocket);		// 释放Socket资源
		return FALSE;					// 返回错误代码
	}
	/* 启用成功 */
	else
	{
		/* 获取局域网广播地址 */
		iRetVal = getBCastIPAddr(pdwBCastIPAddr);

		/* 获取成功 */
		if (FALSE == iRetVal) 
		{
			releaseSocket(localSocket);	// 释放Socket资源
			return FALSE;
		}
	}

	/* 成功启用广播功能 */
	return TRUE;
} /* enableBCast */


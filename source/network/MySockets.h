//******************************************************************************
// File Name		: MySockets.h
// Description		: 自定义的Sockets函数声明，提供给FindOthers和echoServer调用。
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:56 $
//******************************************************************************

#pragma once

/* 初始化WinSock DLL */
BOOL 
initSocket(
	void
	);

/* 释放Socket资源 */
void
releaseSocket(
	IN	const SOCKET localSocket  
	);

/* 创建UDP Socket */
BOOL
createUDPSocket(
	OUT	SOCKET * pLocalSocket
	);

/* 绑定Socket */
BOOL 
bindLocalAddr(
	IN	const SOCKET localSocket,
	IN	const WORD wPort
	);

/* 获取本局域网广播IP地址 */
BOOL
getBCastIPAddr(
	OUT	PDWORD pdwBCastIPAddr
	);

/* 启用Socket的广播功能，并获取本局域网广播IP地址 */
BOOL
enableBCast(
	IN	const SOCKET localSocket,
	OUT	PDWORD pdwBCastIPAddr
	);

/* 发送UDP消息 */
int
sendUDPPacket(
	IN	const SOCKET localSocket,	
	IN	const PSOCKADDR_IN psaRemoteAddr,
	IN	const LPSTR lpstrSendBuf,
	IN	const DWORD dwBytesSendBuf
	);

/* 同步接收UDP消息 */
int
sycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecv
	);

/* 异步接收UDP消息 */
BOOL 
asycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecvBuf
	);


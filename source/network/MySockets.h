//******************************************************************************
// File Name		: MySockets.h
// Description		: �Զ����Sockets�����������ṩ��FindOthers��echoServer���á�
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:56 $
//******************************************************************************

#pragma once

/* ��ʼ��WinSock DLL */
BOOL 
initSocket(
	void
	);

/* �ͷ�Socket��Դ */
void
releaseSocket(
	IN	const SOCKET localSocket  
	);

/* ����UDP Socket */
BOOL
createUDPSocket(
	OUT	SOCKET * pLocalSocket
	);

/* ��Socket */
BOOL 
bindLocalAddr(
	IN	const SOCKET localSocket,
	IN	const WORD wPort
	);

/* ��ȡ���������㲥IP��ַ */
BOOL
getBCastIPAddr(
	OUT	PDWORD pdwBCastIPAddr
	);

/* ����Socket�Ĺ㲥���ܣ�����ȡ���������㲥IP��ַ */
BOOL
enableBCast(
	IN	const SOCKET localSocket,
	OUT	PDWORD pdwBCastIPAddr
	);

/* ����UDP��Ϣ */
int
sendUDPPacket(
	IN	const SOCKET localSocket,	
	IN	const PSOCKADDR_IN psaRemoteAddr,
	IN	const LPSTR lpstrSendBuf,
	IN	const DWORD dwBytesSendBuf
	);

/* ͬ������UDP��Ϣ */
int
sycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecv
	);

/* �첽����UDP��Ϣ */
BOOL 
asycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecvBuf
	);


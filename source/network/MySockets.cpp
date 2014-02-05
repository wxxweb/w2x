//******************************************************************************
// File Name		: MySockets.cpp
// Description		: �Զ����Sockets�������ṩ��FindOthers��echoServer���á�
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:56 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/


#include "StdAfx.h"		// ��׼ͷ�ļ�
#include "MyDefine.h"
#include "HandleError.h"
#include "MySockets.h"


/**********************************************************
   methor 		: initSocket
   description	: ��ʼ��WinSock DLL��
   parameters	: ��
   return value	: ���ɹ��򷵻�TRUE; ��ʧ�����ͷ�WinSock
  				  ��Դ������FALSE��
***********************************************************/
extern BOOL 
initSocket(
	void
	)
{
	int		iRetVal;
	WORD	wVersion;
	WSADATA	wsaData;		// ��ʼ������

	wVersion = MAKEWORD(2, 2);						// ȡ��WinSock DLL�汾��2.2
	iRetVal = WSAStartup(wVersion, &wsaData);		// ��ʼ��WinSock DLL 2.2

	/* ��ʼ��ʧ�� */
	if (NO_ERROR != iRetVal)
	{
		putLastError();		// ���ϵͳ������Ϣ
		WSACleanup();		// �ͷ�WinSock DLL��Դ
		return FALSE;
	}

	/* ��ʼ���ɹ� */
	return TRUE;
} /* initSocket */


/**********************************************************
   methor 		: releaseSocket
   description	: �ͷ�WinSock��Դ��
   parameters	: 
	[IN]	localSocket:SOCKET
  				-- ����Socket��

   return value	: ��
***********************************************************/
extern void
releaseSocket(
	IN	const SOCKET localSocket  
	)
{
	shutdown(localSocket, SD_BOTH);		// ֹͣ��������
	closesocket(localSocket);			// �ر�Socket
	WSACleanup();						// �ͷ�WinSock DLL
}


/**********************************************************
   methor 		: createUDPSocket
   description	: ���������շ�UDP���ݰ��ı���Socket��
   parameters	: 
  	[OUT]	localSocket:SOCKET * 
  				-- ��Ŵ����ı���Socket��
  
   return value	: ���ɹ��򷵻�TRUE; ��ʧ�����ͷ�WinSock
  				  ��Դ������FALSE��
**********************************************************/
extern BOOL
createUDPSocket(
	OUT	SOCKET * pLocalSocket
	)
{
	/* ����Socket */
	*pLocalSocket = WSASocket(AF_INET,SOCK_DGRAM, IPPROTO_UDP, 
							NULL, 0, WSA_FLAG_OVERLAPPED);

	/* ����ʧ�� */
	if (INVALID_SOCKET == *pLocalSocket)
	{
		putLastError();		// ���ϵͳ������Ϣ
		WSACleanup();		// �ͷ�WinSock DLL��Դ
		return FALSE;
	}

	/* �����ɹ� */
	return TRUE;
} /* createUDPSocket */


/**********************************************************
   methor 		: bindLocalAddr
   description	: ������Socket�󶨵�������ַ��
   parameters	: 
  	[IN]	localSocket:SOCKET
  				-- �����շ�UDP���ݰ��ı���Socket��
  	[IN]	 wPort:WORD 
  				-- �˿ںţ�Ϊ0���Զ�����1024-5000��˿ںš�
  
   return value	: ���ɹ��򷵻�TRUE; ��ʧ�����ͷ�Socket��Դ
  				  ���ͷ�WinSock��Դ������FALSE��
***********************************************************/
extern BOOL 
bindLocalAddr(
	IN	const SOCKET localSocket,
	IN	const WORD wPort
	)
{
	int			iRetVal;
	SOCKADDR_IN	saLocalAddr;

	/* ���ñ��ص�ַ��Ϣ */
	saLocalAddr.sin_family		= AF_INET;
	saLocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// �Զ�����������ַ
	saLocalAddr.sin_port		= htons(wPort);			// ����˿ں�

	/* ��Socket */
	iRetVal = bind(localSocket, (sockaddr *)&saLocalAddr, sizeof(SOCKADDR));

	/* ��ʧ�� */
	if (SOCKET_ERROR == iRetVal)	
	{
		putLastError();					// ���ϵͳ������Ϣ
		releaseSocket(localSocket);		// �ͷ�Socket��Դ	
		return FALSE;					// ���ش������
	}

	return TRUE;
} /* bindLocalAddr */


/**********************************************************
   methor 		: sycRecvUDPPacket
   description	: ͬ������UDP���ݰ�.
   parameters	: 
  	[IN]	localSocket : SOCKET 
  				-- �����շ�UDP���ݰ��ı���Socket��
  	[OUT]	psaRemoteAddr : PSOCKADDR_IN 
  				-- ���Զ��������ַ��
  	[OUT]	lpstrRecvBuf : LPSTR 
  				-- ��Ž��յ������ݡ�
	[OUT]	dwBytesRecv : DOWRD 
  				-- ���������ֽ�����

  return value	: �����յ����ݰ����򷵻������ֽ�����������
				  �����г����򷵻�SOCKET_ERROR��
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
	static int	cRemoteAddrSize;	// Զ��������ַ�ṹ�ֽ���

	cRemoteAddrSize = sizeof(SOCKADDR);

	/* ͬ������UDP���ݰ� */ 
	iRetVal = recvfrom(localSocket, lpstrRecvBuf,  dwBytesRecv, 
						0, (PSOCKADDR)psaRemoteAddr,  &cRemoteAddrSize);

	/* ����ʧ�� */
	if (SOCKET_ERROR == iRetVal)
	{
		putLastError();						// ���ϵͳ������Ϣ
		releaseSocket(localSocket);			// �ͷ�Socket��Դ
		return SOCKET_ERROR;				// ���ش������
	}

	/* ���ճɹ� */
	return iRetVal;
} /* sycRecvUDPPacket */


/**********************************************************
   methor 		: asycRecvUDPPacket
   description	: �첽����UDP���ݰ���
   parameters	: 
  	[IN]	localSocket : SOCKET 
				-- �����շ�UDP���ݰ��ı���Socket��
  	[OUT]	psaRemoteAddr : PSOCKADDR_IN 
  				-- ���Զ��������ַ��
  	[OUT]	lpstrRecvBuf : LPSTR 
  				-- ��Ž��յ������ݡ�
	[IN]	dwBytesRecvBuf : DOWRD 
  				-- �������ݰ��Ļ������ֽ�����

   return value	: �����յ����ݰ��򷵻�TRUE�������չ�����
				  �����򷵻�FALSE��
**********************************************************/
extern BOOL 
asycRecvUDPPacket(
	IN	const SOCKET localSocket,
	OUT	PSOCKADDR_IN psaRemoteAddr,
	OUT	LPSTR lpstrRecvBuf,
	IN	const DWORD dwBytesRecvBuf
	)
{
	static int				cRemoteAddrSize;			// Զ��������ַ�ṹ�ֽ���
	static int				iRetVal;					// ����ֵ
	static DWORD			dwFlags = 0;				// WSARecvFrom��Flags����
	static DWORD			dwBytesRecv;				// Ҫ���յ��ֽ������첽�ɺ���
	static WSABUF			wsaDataBuf;					// ���ݻ���
	static WSAOVERLAPPED	wsaOverlapped;				// �ص��ṹ
		
	wsaDataBuf.len = dwBytesRecvBuf;
	wsaDataBuf.buf = lpstrRecvBuf;
	cRemoteAddrSize = sizeof(SOCKADDR);
	ZeroMemory(&wsaOverlapped, sizeof(WSAOVERLAPPED));	// �����ÿգ���������

	/* �첽����UDP��Ϣ */
	iRetVal = WSARecvFrom(localSocket, &wsaDataBuf, 1, &dwBytesRecv, 
						   &dwFlags, (PSOCKADDR)psaRemoteAddr,
							&cRemoteAddrSize, &wsaOverlapped,  NULL);

	if(SOCKET_ERROR == iRetVal)
	{
		/* ���չ����г��� */
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			putLastError();						// ���ϵͳ������Ϣ
			releaseSocket(localSocket);			// �ͷ�Socket��Դ
			return FALSE;
		}
	}

	/* ���յ���Ӧ��Ϣ */
	return TRUE;
} /* asycRecvUDPPacket */


/**********************************************************
   methor 		: sendUDPPacket
   description	: ����UDP���ݰ���
   parameters	: 
  	[IN]	localSocket:SOCKET
				-- �����շ�UDP���ݰ��ı���Socket��
  	[IN]	saRemoteAddr:SOCKADDR_IN 
				-- Զ��������ַ��
  	[IN]	lpszSendBuf : LPSTR 
				-- Ҫ�������ݵĻ�������
	[IN]	dwBytesSendBuf : DWORD
				-- Ҫ���͵��ֽ�����

   return value	: ���ɹ��򷵻ط����ֽ�������ʧ�����ͷ�
  				  Socket��WinSock��Դ������SOCKET_ERROR��
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

	/* ������Ϣ */
	iRetVal = sendto(localSocket, lpstrSendBuf, dwBytesSendBuf, 0, 
				     (PSOCKADDR)psaRemoteAddr, sizeof(SOCKADDR));
	
	/* ����ʧ�� */
	if (SOCKET_ERROR == iRetVal)
	{
		putLastError();						// ���ϵͳ������Ϣ
		releaseSocket(localSocket);			// �ͷ�Socket��Դ
		return SOCKET_ERROR;				// ���ش������
	}

	/* ���ͳɹ� */
	return iRetVal;
} /* sendUDPPacket */


/**********************************************************
   methor 		: getBCastIPAddr
   description	: ��IP��ַ���л�ȡ����IP��ַ���������룬
				  �����м�������������㲥IP��ַ��
   parameters	: 
  	[OUT]	pdwAddr:PDWORD 
			-- ��Ź㲥��ַ��

   return value	: ���ɹ�����TRUE, ���򷵻�FALSE��
***********************************************************/
extern BOOL
getBCastIPAddr(
	OUT	PDWORD pdwBCastIPAddr
	)
{
	int		iRetVal;
	DWORD	dwSize = 0;
	DWORD	dwIPAddr;				// IP��ַ
	DWORD	dwMask;					// ��������
	PMIB_IPADDRTABLE pIPAddrTable;	// IP��ַ��ṹ

	pIPAddrTable = (MIB_IPADDRTABLE*)GlobalAlloc(GMEM_FIXED, 
			sizeof(MIB_IPADDRTABLE));					// ΪIP��ַ������ڴ�

	/* IP��ַ��ṹ�ڴ����ɹ� */
	if (pIPAddrTable)
	{	
		iRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0); // ��ȡIP��ַ��

		/* �����IP��ַ��ṹ�Ļ��治�� */
		if (ERROR_INSUFFICIENT_BUFFER == iRetVal)
		{	
			/* ���·����ڴ� */
			GlobalFree(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *)GlobalAlloc(GMEM_FIXED, dwSize);
		}
	}
	/* IP��ַ��ṹ�ڴ����ʧ�� */
	else 
	{	
		putLastError();		// ���ϵͳ������Ϣ
		return FALSE;
	}

	/* IP��ַ��ṹ�ڴ����·���ɹ� */
	if (pIPAddrTable)
	{
		/* IP��ַ���ȡ�ɹ� */
		if (NO_ERROR == GetIpAddrTable(pIPAddrTable, &dwSize, 0))
		{
			dwIPAddr = pIPAddrTable->table[0].dwAddr;				// ��ȡIP��ַ
			dwMask = pIPAddrTable->table[0].dwMask;					// �����������
			*pdwBCastIPAddr = (dwIPAddr & dwMask) | ~dwMask;		// ����㲥��ַ
			GlobalFree(pIPAddrTable);								// �ͷ��ڴ�ռ�
		}
		/* IP��ַ���ȡʧ�� */
		else
		{
			putLastError();	// ���ϵͳ������Ϣ
			return FALSE;
		}
	}
	// IP��ַ���ڴ����·���ʧ��
	else
	{
		putLastError();		// ���ϵͳ������Ϣ
		return FALSE;
	}

	/* �ɹ���ñ��������㲥��ַ */
	return TRUE;
} /* getBCastIPAddr */


/**********************************************************
   methor 		: enableBCast
   description	: ����Socket�㲥���ܡ�
   parameters	: 
  	[IN]	localSocket:SOCKET 
			-- �����շ�UDP���ݰ��ı���Socket��

   return value	: ���ɹ��򷵻�TRUE����ʧ�����ͷ�Socket
  				  ��WinSock��Դ������FALSE��
**********************************************************/
extern BOOL
enableBCast(
	IN	const SOCKET localSocket,
	OUT	PDWORD pdwBCastIPAddr
	)
{
	BOOL	bBroadcast = TRUE;			// setsockopt���һ����
	int		iRetVal;

	/* ����Socket�㲥���� */
	iRetVal = setsockopt(localSocket, SOL_SOCKET, SO_BROADCAST, 
						  (char *)&bBroadcast,  sizeof(int));
	/* ����ʧ�� */
	if (SOCKET_ERROR == iRetVal)
	{
		putLastError();					// ���ϵͳ������Ϣ
		releaseSocket(localSocket);		// �ͷ�Socket��Դ
		return FALSE;					// ���ش������
	}
	/* ���óɹ� */
	else
	{
		/* ��ȡ�������㲥��ַ */
		iRetVal = getBCastIPAddr(pdwBCastIPAddr);

		/* ��ȡ�ɹ� */
		if (FALSE == iRetVal) 
		{
			releaseSocket(localSocket);	// �ͷ�Socket��Դ
			return FALSE;
		}
	}

	/* �ɹ����ù㲥���� */
	return TRUE;
} /* enableBCast */


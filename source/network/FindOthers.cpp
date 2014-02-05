//******************************************************************************
// File Name		: FindOthers.cpp
//
// Description		: ��Դ�ļ��ж����˸�ģ�������������Ľӿں���findOthers��
//					  echoServer������Ƶ���findOthers�����ı�������Ϊ�ͻ��ˣ�
//					  ����echoServer������Զ������Ϊ�������ˡ���ģ�鹦���ǿ�
//					  ����ͨ������findOthers��������UDP�㲥�������ڷ������˵�
//					  ��echoServer��������Ӧ�ͻ��˵Ĺ㲥������ʱ���ͻ��˺ͷ���
//					  ���˽���֪�Է��ĵ�ַ������IP��ַ�����ڷ���UDP���ݰ��Ķ�
//					  �ںš�
//
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:56 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#include "StdAfx.h"		// ��׼ͷ�ļ�
#include "MyDefine.h"
#include "MySockets.h"

/* ˫��Լ���õ��Զ����Ӧ��ϢGUID */
static const GUID gc_guidEchoMessage[] = {0x37ff70ac,					// data1
										  0x8215,						// data2
										  0x490c,						// data3
								          {0x81, 0xC8, 0xF4, 0x4B, 
								           0xF8, 0x59, 0xEB, 0x6F}};	// data4


/***********************************************************
   methor 		: findServer

   description	: ͨ������UDP�㲥���õ�Զ��������Ӧ���Ի�
				  ȡ�Է���ַ�������󶨵Ĺ̶��˿���ϵͳ���䡣
				  
   parameters	: 
  	[IN]	wAgreedPort : WORD
				-- Զ�������Ĺ㲥�����ն˿ںţ��ö˿ںű�
				   ����echoServer������wAgreedPort����ȡ
			       ֵ����һ�£�����Զ���������޷����յ���
				   UDP�㲥����
	[IN]	dwTimeOut : DWORD
				-- ��ʱʱ����������ͻ��˵ȴ��������˽�
				   �л�Ӧ���������ʱ�䣬��ͻ���ֹͣ�ȴ�
				   ����findOthers�����з��ء�
	[OUT]	pszRemoteAddr : PSOCKADDR
				-- ���ڴ�Ż�ȡ���ķ�������������ַ������
				   IP��ַ�ͷ������ݰ��Ķ˿ںš����������
				   �ں��Ѿ��ڷ������˰󶨣��ʿ���Ϊ����UDP
				   ��TCP�������շ��˿ڡ�

   return value	: ����õ��������˵Ļ�Ӧ����ȡ���ַ������
				  TRUE���෴�ģ�����ڴ˹����г����򷵻�
				  FALSE�����У��ڷ���֮ǰ�����ͷŵ�ռ�õ�
				  Socket�����Դ��
************************************************************/
extern BOOL 
findOthers(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
   OUT	PSOCKADDR_IN psaRemoteAddr
	)
{
	int			nLoopCount = 0;					// ѭ������
	int			iRetVal;						// ����ֵ
	char		szIPAddrString[_MAX_STRING];	// ��ַ�ַ���
	char		szRecvBuf[SO_MAX_MSG_SIZE];		// �������ݻ���
	DWORD		dwBeginTime;					// ��ʼʱ��
	DWORD		dwEndTime;						// ����ʱ��
	DWORD		dwBCastIPAddr;					// ���������㲥IP��ַ
	SOCKET		localSocket;					// ����Socket������
	SOCKADDR_IN	saBCastAddr;					// �㲥��ַ
	
	dwBeginTime = GetTickCount();	// ȡ���״εȴ���Ӧ��ʼʱ��
	
	/* ��ʼ��WinSock DLL */
	if (FALSE == initSocket()){
		return FALSE;
	}
		
	/* ���������շ�UDP���ݰ��ı���Socket */
	if (FALSE == createUDPSocket(&localSocket)){
		return FALSE;
	}

	/* ��localSocket�󶨵�������ַ����1024-5000֮��Ĺ̶��˿� */
	if (FALSE == bindLocalAddr(localSocket, 0)){
		return FALSE;
	}

	/* ����localSocket�㲥���ܣ�����ȡ���������㲥IP��ַ */
	if (FALSE == enableBCast(localSocket, &dwBCastIPAddr)){
		return FALSE;
	}

	/* ���ý���UDP�㲥���Ĺ㲥��ַ */
	saBCastAddr.sin_family		= AF_INET;
	saBCastAddr.sin_addr.s_addr	= dwBCastIPAddr;		// ���������㲥IP��ַ
	saBCastAddr.sin_port		= htons(wAgreedPort);	// Զ�������㲥�����ն˿�

	// [--
	// ���͹㲥��Ϣ���ȴ�Զ��������Ӧ
	for (dwEndTime = dwBeginTime;
			dwEndTime - dwBeginTime < dwTimeOut;		// ��ʱʱ��Ϊ���뼶
			dwEndTime = GetTickCount()					// ���ĳ�εȴ���Ӧ�Ľ���ʱ��
		 )
	{
		printf("[%d] ", ++nLoopCount);

		/* ���͹㲥��Ϣ */
		iRetVal = sendUDPPacket(localSocket, &saBCastAddr, 
								 (LPSTR)&gc_guidEchoMessage, 
								 sizeof(GUID));
		
		/* ����ʧ�� */
		if (SOCKET_ERROR == iRetVal) 
		{
			printf("��Ϣ����ʧ��.\n");
			releaseSocket(localSocket);		// �ͷ�Socket��Դ
			return FALSE;
		}
		/* ���ͳɹ� */
		else
		{
			printf("���͹㲥��Ϣ[%d Bytes].\n", iRetVal);
		}
		
		/* ����Զ��������Ϣ */
		iRetVal = asycRecvUDPPacket(localSocket, psaRemoteAddr, 
									szRecvBuf, sizeof(szRecvBuf));

		/* ���չ����г��� */
		if (FALSE == iRetVal) 
		{
			printf("��Ϣ���չ����г���.\n");
			releaseSocket(localSocket);		// �ͷ�Socket��Դ
			return FALSE;
		}
		/* ���յ���Ϣ */
		else 
		{
			/* ���յ���Ӧ��Ϣ */
			if (0 == memcmp(&gc_guidEchoMessage, szRecvBuf, sizeof(GUID)))
			{
				getIPAddrString(psaRemoteAddr->sin_addr.s_addr, 
								szIPAddrString, sizeof(szIPAddrString));

				printf("���յ���Ӧ��Ϣ.\nԶ��������ַ��%s:%d\n", 
						szIPAddrString, ntohs(psaRemoteAddr->sin_port));

				releaseSocket(localSocket);
				return TRUE;
			}
			/* �ȴ����ջ�Ӧ��Ϣ */
			else
			{
				printf("�ȴ����ջ�Ӧ��Ϣ.\n");
			}
		}

		Sleep(500);		// �ӳ�0.5��
	} // for
	// ���͹㲥��Ϣ���ȴ�Զ��������Ӧ
	// --]

	/* ��ʱ */
	printf("���ճ�ʱ.\n");
	releaseSocket(localSocket);
	return FALSE;
} /* findServer */


/**********************************************************
   methor 		: EchoServer

   description	: �������˽��յ��ͻ��˷��͵�UDP�㲥����ȷ
				  ������ݺ󣬽��ù㲥�����ݰ��ش����ͻ�
				  ����Ϊ��Ӧ���������˰󶨵Ĺ̶��˿��ɿ���
				  ��Ա���䡣
   parameters	: 
  	[IN]	wAgreedPort : WORD
				-- �������˵Ĺ㲥�����ն˿ںţ� findOthers
				   ������wAgreedPort����ȡֵ������ö˿ں�
				   ����һ�£�����������˽��޷����յ��ͻ�
				   �˷��͵�UDP�㲥����
	[IN]	dwTimeOut : DWORD
				-- ��ʱʱ����������������˵ȴ��������Կ�
				   ���˵�UDP�㲥�����������ʱ���������
				   ������ֹͣ�ȴ�����echoServer�����з��ء�
	[IN]	dwNumEcho : DWORD
				-- �������ƻش�UDP���ݰ���Ŀ�����������ڳ�
				   ʱʱ����֮�ڽ��յ������Ŀ�Ŀͻ��˷���
				   ��UDP�㲥������ͬʱ�ش���Ӧ��Ŀ��UDP����
				   ���������ֵΪ0����ʾ�����ƻش���Ŀ��һ
				   ֱ�ش����ݰ�֪����ʱΪֹ��

   return value	: ������������շ�������û�г����򷵻�
				  TRUE����ʹ�ȴ���ʱҲ��ˡ������������
				  ����FALSE�����У��ڷ���֮ǰ�����ͷŵ�ռ
				  �õ�Socket�����Դ��
************************************************************/
extern BOOL 
echoServer(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
	IN	const DWORD dwNumEcho
	)
{
	int			iRetVal;						// ����ֵ
	char		szIPAddrString[_MAX_STRING];	// ��ַ�ַ���
	char		szRecvBuf[SO_MAX_MSG_SIZE];		// �������ݻ���
	DWORD		cEchoCount = 0;					// ��Ӧ����
	DWORD		dwBeginTime;					// ��ʼʱ��
	DWORD		dwEndTime;						// ����ʱ��
	SOCKET		localSocket;					// ����Socket������
	SOCKADDR_IN	saRemoteAddr;					// Զ��������ַ

	dwBeginTime = GetTickCount();	// ȡ�ÿ�ʼʱ��
	
	/* ��ʼ��WinSock DLL */
	if (FALSE == initSocket()){
		return FALSE;
	}
		
	/* ��������Socket */
	if (FALSE == createUDPSocket(&localSocket)){
		return FALSE;
	}

	/* ��localSocket������������ַ��ʹ��Լ���˿ڽ������� */
	if (FALSE == bindLocalAddr(localSocket, wAgreedPort)) {
		return FALSE;
	}

	for (dwEndTime = dwBeginTime;  
			dwEndTime - dwBeginTime < dwTimeOut;	// ��ʱʱ��Ϊ����
			dwEndTime = GetTickCount()
		)
	{
		/* ����UDP��Ϣ */
		iRetVal = asycRecvUDPPacket(localSocket, &saRemoteAddr, 
									szRecvBuf, sizeof(szRecvBuf));

		/* ����ʧ�� */
		if (FALSE == iRetVal) 
		{
			printf("��Ϣ���չ����г���.\n");
			return FALSE;
		}
		/* ���ճɹ� */ 
		else
		{
			/* ���յ�����Ӧ��Ϣ */
			if (0 == memcmp(&gc_guidEchoMessage, szRecvBuf, sizeof(GUID)))
			{
				getIPAddrString(saRemoteAddr.sin_addr.s_addr, 
								szIPAddrString, sizeof(szIPAddrString));

				printf("���յ�����Ӧ��Ϣ.\nԶ��������ַ��%s:%d\n", 
						szIPAddrString, ntohs(saRemoteAddr.sin_port));

				/* ���ͻ�Ӧ��Ϣ */
				iRetVal = sendUDPPacket(localSocket, &saRemoteAddr, 
										 szRecvBuf, sizeof(GUID));

				/* ����ʧ�� */
				if (SOCKET_ERROR == iRetVal)
				{
					printf("��Ϣ����ʧ��.\n");
					return FALSE;
				}
				/* ���ͳɹ� */
				else
				{
					printf("���ͻ�Ӧ��Ϣ[%d Bytes].\n", iRetVal);

					/* ��Ӧ���� */
					if (++cEchoCount == dwNumEcho)
					{
						releaseSocket(localSocket);
						return TRUE;
					}
				}
			}
			/* �ȴ����մ���Ӧ��Ϣ */
			else
			{
				printf("�ȴ����մ���Ӧ��Ϣ.\n");
			}
		}
		Sleep(500);		// �ӳ�0.5��
	} // for

	/* ��ʱ */
	printf("���ճ�ʱ.\n");
	releaseSocket(localSocket);
	return TRUE;
} /* echoServer */
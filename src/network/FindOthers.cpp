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
#include "tiny_socket.h"
#include "utility.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


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
	TCHAR		szIPAddrString[MAX_IP_ADDR_STR];	// ��ַ�ַ���
	char		szRecvBuf[SO_MAX_MSG_SIZE];		// �������ݻ���
	DWORD		dwBeginTime;					// ��ʼʱ��
	DWORD		dwEndTime;						// ����ʱ��
	DWORD		dwBCastIPAddr;					// ���������㲥IP��ַ
	SOCKADDR_IN	saBCastAddr;					// �㲥��ַ
	
	dwBeginTime = GetTickCount();	// ȡ���״εȴ���Ӧ��ʼʱ��
	
	/* ��ʼ��WinSock DLL */
	if (FALSE == CTinySocket::Initialize()){
		return FALSE;
	}

	CTinySocket tiny_socket;
		
	// ���������շ� UDP ���ݰ��� Socket��
	// �� 1024-5000 ֮��Ĺ̶��˿ڣ�
	// ���þ������㲥���ݰ����͹���
	if (false == tiny_socket.CreateUdp(0, true)){
		return FALSE;
	}

	// ��ȡ���������㲥 IP ��ַ
	if (FALSE == w2x::network::GetBroadcastIpAddress(dwBCastIPAddr)){
		return FALSE;
	}

	DWORD ip_addr = 0;
	internal::GetBroadcastIpAddress(ip_addr);
	internal::ParseIpAddressString(szIPAddrString, 
		_countof(szIPAddrString), ip_addr);

	/* ���ý���UDP�㲥���Ĺ㲥��ַ */
	saBCastAddr.sin_family		= AF_INET;
	saBCastAddr.sin_addr.s_addr	= dwBCastIPAddr;		// ���������㲥IP��ַ
	saBCastAddr.sin_port		= htons(wAgreedPort);	// Զ�������㲥�����ն˿�

	CTinySocket::ERecvStatus recv_status = CTinySocket::kAsyncRecvComplete;

	// [--
	// ���͹㲥��Ϣ���ȴ�Զ��������Ӧ
	for (dwEndTime = dwBeginTime;
			dwEndTime - dwBeginTime < dwTimeOut;		// ��ʱʱ��Ϊ���뼶
			dwEndTime = GetTickCount()					// ���ĳ�εȴ���Ӧ�Ľ���ʱ��
		 )
	{
		_tprintf_s(TEXT("[%d] "), ++nLoopCount);

		/* ���͹㲥��Ϣ */
		iRetVal = tiny_socket.SendUdpPacket(&saBCastAddr, 
								 (PBYTE)&gc_guidEchoMessage, 
								 sizeof(GUID));
		
		/* ����ʧ�� */
		if (SOCKET_ERROR == iRetVal) 
		{
			_tprintf_s(TEXT("Send UDP packet faild.\n"));
			return FALSE;
		}
		/* ���ͳɹ� */
		else
		{
			_tprintf_s(TEXT("Send broadcast msg [%d Bytes].\n"), iRetVal);
		}

		Sleep(500); // �ӳ�0.5��
		
		/* ����Զ��������Ϣ */
		recv_status = tiny_socket.AsyncRecvUdpPacket(psaRemoteAddr, 
			(PBYTE)szRecvBuf, sizeof(szRecvBuf));

		/* ���չ����г��� */
		if (CTinySocket::kAsyncRecvError == recv_status) 
		{
			_tprintf_s(TEXT("Async receive UDP packet faild.\n"));
			return FALSE;
		}
		/* ���յ���Ϣ */
		else if (CTinySocket::kAsyncRecvComplete == recv_status)
		{
			/* ���յ���Ӧ��Ϣ */
			if (0 == memcmp(&gc_guidEchoMessage, szRecvBuf, sizeof(GUID)))
			{
				DWORD ip_addr = 0;
				internal::GetBroadcastIpAddress(ip_addr);
				internal::ParseIpAddressString(szIPAddrString, 
					_countof(szIPAddrString), ip_addr);
				_tprintf_s(TEXT("Received ack msg.\nRemote host IP: %s:%d\n"), 
						szIPAddrString, ntohs(psaRemoteAddr->sin_port));

				CTinySocket::Uninitialize();
				return TRUE;
			}
			/* �ȴ����ջ�Ӧ��Ϣ */
			else
			{
				_tprintf_s(TEXT("Wait for ack msg.\n"));
			}
		}
	} // for
	// ���͹㲥��Ϣ���ȴ�Զ��������Ӧ
	// --]

	/* ��ʱ */
	_tprintf_s(TEXT("Wait time out for ack msg.\n"));
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
	TCHAR		szIPAddrString[internal::MAX_IP_ADDR_STR];	// ��ַ�ַ���
	char		szRecvBuf[SO_MAX_MSG_SIZE];		// �������ݻ���
	DWORD		cEchoCount = 0;					// ��Ӧ����
	DWORD		dwBeginTime;					// ��ʼʱ��
	DWORD		dwEndTime;						// ����ʱ��
	SOCKADDR_IN	saRemoteAddr;					// Զ��������ַ

	dwBeginTime = GetTickCount();	// ȡ�ÿ�ʼʱ��
	
	/* ��ʼ��WinSock DLL */
	if (false == CTinySocket::Initialize()){
		return FALSE;
	}

	CTinySocket tiny_socket;
		
	/* ��������Socket */
	if (FALSE == tiny_socket.CreateUdp()){
		return FALSE;
	}

	/* ��localSocket������������ַ��ʹ��Լ���˿ڽ������� */
	if (FALSE == tiny_socket.Bind(wAgreedPort)) {
		return FALSE;
	}

	for (dwEndTime = dwBeginTime;  
			dwEndTime - dwBeginTime < dwTimeOut;	// ��ʱʱ��Ϊ����
			dwEndTime = GetTickCount()
		)
	{
		_tprintf_s(TEXT("Receiving UDP packet...\n"));

		/* ����UDP��Ϣ */
		const int recv_status 
			= tiny_socket.SyncRecvUdpPacket(&saRemoteAddr, 
				(PBYTE)szRecvBuf, sizeof(szRecvBuf));

		/* ����ʧ�� */
		if (SOCKET_ERROR == recv_status) 
		{
			_tprintf_s(TEXT("Receive UDP packet faild.\n"));
			return FALSE;
		}
		/* ���ճɹ� */ 
		else
		{
			/* ���յ�����Ӧ��Ϣ */
			if (0 == memcmp(&gc_guidEchoMessage, szRecvBuf, sizeof(GUID)))
			{
				internal::ParseIpAddressString(szIPAddrString, 
					sizeof(szIPAddrString), saRemoteAddr.sin_addr.s_addr);

				_tprintf_s(
					TEXT("Received the ack msg.\nRemote host IP: %s:%d\n"), 
					szIPAddrString, ntohs(saRemoteAddr.sin_port));

				/* ���ͻ�Ӧ��Ϣ */
				iRetVal = tiny_socket.SendUdpPacket(&saRemoteAddr, 
										 (PBYTE)szRecvBuf, sizeof(GUID));

				/* ����ʧ�� */
				if (SOCKET_ERROR == iRetVal)
				{
					_tprintf_s(TEXT("Send UDP packet faild.\n"));
					return FALSE;
				}
				/* ���ͳɹ� */
				else
				{
					_tprintf_s(TEXT("Send response msg [%d Bytes].\n"), iRetVal);

					/* ��Ӧ���� */
					if (++cEchoCount == dwNumEcho)
					{
						return TRUE;
					}
				}
			}
			/* �ȴ����մ���Ӧ��Ϣ */
			else
			{
				_tprintf_s(TEXT("wait for receive ack.\n"));
			}
		}
		Sleep(1000);		// �ӳ�0.5��
	} // for

	/* ��ʱ */
	_tprintf_s(TEXT("���ճ�ʱ.\n"));

	return TRUE;
} /* echoServer */


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
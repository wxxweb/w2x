//******************************************************************************
// File Name		: FindOthers.cpp
//
// Description		: 本源文件中定义了该模块中两个导出的接口函数findOthers和
//					  echoServer。下面称调用findOthers函数的本地主机为客户端，
//					  调用echoServer函数的远程主机为服务器端。该模块功能是客
//					  户端通过调用findOthers函数发送UDP广播包，并在服务器端调
//					  用echoServer函数来回应客户端的广播包。此时，客户端和服务
//					  器端将获知对方的地址，包括IP地址和用于发送UDP数据包的端
//					  口号。
//
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:56 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#include "StdAfx.h"		// 标准头文件
#include "tiny_socket.h"
#include "utility.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


/* 双方约定好的自定义回应消息GUID */
static const GUID gc_guidEchoMessage[] = {0x37ff70ac,					// data1
										  0x8215,						// data2
										  0x490c,						// data3
								          {0x81, 0xC8, 0xF4, 0x4B, 
								           0xF8, 0x59, 0xEB, 0x6F}};	// data4


/***********************************************************
   methor 		: findServer

   description	: 通过发送UDP广播包得到远程主机回应，以获
				  取对方地址。本机绑定的固定端口由系统分配。
				  
   parameters	: 
  	[IN]	wAgreedPort : WORD
				-- 远程主机的广播包接收端口号，该端口号必
				   须与echoServer函数的wAgreedPort参数取
			       值保持一致，否则远程主机将无法接收到该
				   UDP广播包。
	[IN]	dwTimeOut : DWORD
				-- 超时时间间隔，如果客户端等待服务器端进
				   行回应而超过这个时间，则客户端停止等待
				   并从findOthers函数中返回。
	[OUT]	pszRemoteAddr : PSOCKADDR
				-- 用于存放获取到的服务器端主机地址，包含
				   IP地址和发送数据包的端口号。由于这个端
				   口号已经在服务器端绑定，故可作为后期UDP
				   和TCP的数据收发端口。

   return value	: 如果得到服务器端的回应并获取其地址，返回
				  TRUE。相反的，如果在此过程中出错则返回
				  FALSE。其中，在返回之前将先释放掉占用的
				  Socket相关资源。
************************************************************/
extern BOOL 
findOthers(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
   OUT	PSOCKADDR_IN psaRemoteAddr
	)
{
	int			nLoopCount = 0;					// 循环计数
	int			iRetVal;						// 返回值
	TCHAR		szIPAddrString[MAX_IP_ADDR_STR];	// 地址字符串
	char		szRecvBuf[SO_MAX_MSG_SIZE];		// 接收数据缓存
	DWORD		dwBeginTime;					// 开始时间
	DWORD		dwEndTime;						// 结束时间
	DWORD		dwBCastIPAddr;					// 本局域网广播IP地址
	SOCKADDR_IN	saBCastAddr;					// 广播地址
	
	dwBeginTime = GetTickCount();	// 取得首次等待回应开始时间
	
	/* 初始化WinSock DLL */
	if (FALSE == CTinySocket::Initialize()){
		return FALSE;
	}

	CTinySocket tiny_socket;
		
	/* 创建用于收发UDP数据包的本地Socket */
	if (false == tiny_socket.CreateUdp()){
		return FALSE;
	}

	/* 将localSocket绑定到本机地址，绑定1024-5000之间的固定端口 */
	if (false == tiny_socket.Bind()){
		return FALSE;
	}

	/* 启用localSocket广播功能，并获取本局域网广播IP地址 */
	if (FALSE == tiny_socket.EnableBroadcast(dwBCastIPAddr)){
		return FALSE;
	}

	DWORD ip_addr = 0;
	internal::GetBroadcastIpAddress(ip_addr);
	internal::ParseIpAddressString(szIPAddrString, 
		_countof(szIPAddrString), ip_addr);

	/* 设置接收UDP广播包的广播地址 */
	saBCastAddr.sin_family		= AF_INET;
	saBCastAddr.sin_addr.s_addr	= dwBCastIPAddr;		// 本局域网广播IP地址
	saBCastAddr.sin_port		= htons(wAgreedPort);	// 远程主机广播包接收端口

	CTinySocket::EAsyncRecvStatus recv_status = CTinySocket::kAsyncRecvComplete;

	// [--
	// 发送广播消息，等待远程主机回应
	for (dwEndTime = dwBeginTime;
			dwEndTime - dwBeginTime < dwTimeOut;		// 超时时间为毫秒级
			dwEndTime = GetTickCount()					// 获得某次等待回应的结束时间
		 )
	{
		_tprintf_s(TEXT("[%d] "), ++nLoopCount);

		/* 发送广播消息 */
		iRetVal = tiny_socket.SendUdpPacket(&saBCastAddr, 
								 (PBYTE)&gc_guidEchoMessage, 
								 sizeof(GUID));
		
		/* 发送失败 */
		if (SOCKET_ERROR == iRetVal) 
		{
			_tprintf_s(TEXT("Send UDP packet faild.\n"));
			return FALSE;
		}
		/* 发送成功 */
		else
		{
			_tprintf_s(TEXT("Send broadcast msg [%d Bytes].\n"), iRetVal);
		}

		Sleep(500); // 延迟0.5秒
		
		/* 接收远程主机消息 */
		//if (CTinySocket::kAsyncRecvComplete == recv_status)
		//{
			//::Sleep(1000);
			recv_status = tiny_socket.AsyncRecvUdpPacket(psaRemoteAddr, 
				(PBYTE)szRecvBuf, sizeof(szRecvBuf));
		//}

		/* 接收过程中出错 */
		if (CTinySocket::kAsyncRecvError == recv_status) 
		{
			_tprintf_s(TEXT("Async receive UDP packet faild.\n"));
			return FALSE;
		}
		/* 接收到消息 */
		else if (CTinySocket::kAsyncRecvComplete == recv_status)
		{
			/* 接收到回应消息 */
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
			/* 等待接收回应消息 */
			else
			{
				_tprintf_s(TEXT("Wait for ack msg.\n"));
			}
		}
	} // for
	// 发送广播消息，等待远程主机回应
	// --]

	/* 超时 */
	_tprintf_s(TEXT("Wait time out for ack msg.\n"));
	return FALSE;
} /* findServer */


/**********************************************************
   methor 		: EchoServer

   description	: 服务器端接收到客户端发送的UDP广播包，确
				  认其身份后，将该广播的数据包回传给客户
				  端作为回应。服务器端绑定的固定端口由开发
				  人员分配。
   parameters	: 
  	[IN]	wAgreedPort : WORD
				-- 服务器端的广播包接收端口号， findOthers
				   函数的wAgreedPort参数取值必须与该端口号
				   保持一致，否则服务器端将无法接收到客户
				   端发送的UDP广播包。
	[IN]	dwTimeOut : DWORD
				-- 超时时间间隔，如果服务器端等待接收来自客
				   户端的UDP广播包超过了这个时间间隔，则服
				   务器端停止等待并从echoServer函数中返回。
	[IN]	dwNumEcho : DWORD
				-- 用于限制回传UDP数据包数目，当服务器在超
				   时时间间隔之内接收到这个数目的客户端发来
				   的UDP广播包，则同时回传相应数目的UDP数据
				   包。如果该值为0，表示不限制回传数目，一
				   直回传数据包知道超时为止。

   return value	: 如果服务器在收发过程中没有出错，则返回
				  TRUE，即使等待超时也如此。如果出错，否则
				  返回FALSE。其中，在返回之前将先释放掉占
				  用的Socket相关资源。
************************************************************/
extern BOOL 
echoServer(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
	IN	const DWORD dwNumEcho
	)
{
	int			iRetVal;						// 返回值
	TCHAR		szIPAddrString[internal::MAX_IP_ADDR_STR];	// 地址字符串
	char		szRecvBuf[SO_MAX_MSG_SIZE];		// 接收数据缓存
	DWORD		cEchoCount = 0;					// 回应计数
	DWORD		dwBeginTime;					// 开始时间
	DWORD		dwEndTime;						// 结束时间
	SOCKADDR_IN	saRemoteAddr;					// 远程主机地址

	dwBeginTime = GetTickCount();	// 取得开始时间
	
	/* 初始化WinSock DLL */
	if (false == CTinySocket::Initialize()){
		return FALSE;
	}

	CTinySocket tiny_socket;
		
	/* 创建本地Socket */
	if (FALSE == tiny_socket.CreateUdp()){
		return FALSE;
	}

	/* 绑定localSocket至本地主机地址，使用约定端口接收数据 */
	if (FALSE == tiny_socket.Bind(wAgreedPort)) {
		return FALSE;
	}

	for (dwEndTime = dwBeginTime;  
			dwEndTime - dwBeginTime < dwTimeOut;	// 超时时间为毫秒
			dwEndTime = GetTickCount()
		)
	{
		_tprintf_s(TEXT("Receiving UDP packet...\n"));

		/* 接收UDP消息 */
		const int recv_status 
			= tiny_socket.SyncRecvUdpPacket(&saRemoteAddr, 
				(PBYTE)szRecvBuf, sizeof(szRecvBuf));

		/* 接收失败 */
		if (SOCKET_ERROR == recv_status) 
		{
			_tprintf_s(TEXT("Receive UDP packet faild.\n"));
			return FALSE;
		}
		/* 接收成功 */ 
		else
		{
			/* 接收到待回应消息 */
			if (0 == memcmp(&gc_guidEchoMessage, szRecvBuf, sizeof(GUID)))
			{
				internal::ParseIpAddressString(szIPAddrString, 
					sizeof(szIPAddrString), saRemoteAddr.sin_addr.s_addr);

				_tprintf_s(
					TEXT("Received the ack msg.\nRemote host IP: %s:%d\n"), 
					szIPAddrString, ntohs(saRemoteAddr.sin_port));

				/* 发送回应消息 */
				iRetVal = tiny_socket.SendUdpPacket(&saRemoteAddr, 
										 (PBYTE)szRecvBuf, sizeof(GUID));

				/* 发送失败 */
				if (SOCKET_ERROR == iRetVal)
				{
					_tprintf_s(TEXT("Send UDP packet faild.\n"));
					return FALSE;
				}
				/* 发送成功 */
				else
				{
					_tprintf_s(TEXT("Send response msg [%d Bytes].\n"), iRetVal);

					/* 回应计数 */
					if (++cEchoCount == dwNumEcho)
					{
						return TRUE;
					}
				}
			}
			/* 等待接收待回应消息 */
			else
			{
				_tprintf_s(TEXT("wait for receive ack.\n"));
			}
		}
		Sleep(1000);		// 延迟0.5秒
	} // for

	/* 超时 */
	_tprintf_s(TEXT("接收超时.\n"));

	return TRUE;
} /* echoServer */


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
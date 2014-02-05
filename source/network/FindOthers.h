//******************************************************************************
// File Name		: FindOthers.h
//
// Description		: 该模块中包含两个导出的接口函数findOthers和echoServer。下
//					  面称调用findOthers函数的本地主机为客户端，调用echoServer
//					  函数的远程主机为服务器端。该模块功能是客户端通过调用
//					  findOthers函数发送UDP广播包，并在服务器端调用echoServer
//					  函数来回应客户端的广播包。此时，客户端和服务器端将获知对
//					  方的地址，包括IP地址和用于发送UDP数据包的端口号。
//
// date created		: 2010-11-21
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:53 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#pragma once

#include "exports.h"

/* 入口参数标志 */
#ifndef IN
#define IN
#endif

/* 出口参数标志 */
#ifndef OUT
#define OUT
#endif

/* 出入口参数标志 */
#ifndef IN_OUT
#define IN_OUT
#endif

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
W2X_NETWORK_API BOOL
findOthers(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
   OUT	PSOCKADDR_IN psaRemoteAddr
	);


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
W2X_NETWORK_API BOOL 
echoServer(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
	IN	const DWORD dwNumEcho
	);
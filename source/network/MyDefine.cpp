//******************************************************************************
// File Name		: MyDefine.cpp
// Description		: 自定义的一些宏和函数
// date created		: 2010-11-23
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:00 $
// revision			: $ rev : 1 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#include "StdAfx.h"		// 标准头文件


/**********************************************************
   methor 		: getIPAddrString
   description	: 通过二进制网络字节序IP地址求得其主机字节
				  序的字符串形式。
   parameters	: 
  	[IN]	dwIPAddr : DWORD 
				-- 二进制网络字节序IP地址。
	[IN]	lpszIPAddr : LPSTR
				-- 存放IP地址字符串的缓冲区。
	[IN]	dwBytesBuf : DWORD
				-- 缓冲区字节数。

   return value	: 若成功则返回存储的字节数，不包括NULL字符，
				  若失败则返回-1。
***********************************************************/
extern int
getIPAddrString(
	IN	const DWORD dwIPAddr, 
	OUT	LPSTR lpszIPAddr,
	IN	const DWORD dwBytesBuf
	)
{
	return sprintf_s(lpszIPAddr, dwBytesBuf,"%d.%d.%d.%d",
				(dwIPAddr & 0x000000FF),
				(dwIPAddr & 0x0000FF00) >> 8,
				(dwIPAddr & 0x00FF0000) >> 16,
				(dwIPAddr & 0xFF000000) >> 24);
}
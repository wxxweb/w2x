//******************************************************************************
// File Name		: MyDefine.h
// Description		: 自定义的一些宏和函数
// date created		: 2010-11-19
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:53 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#pragma once

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

/* 字符串最大长度 */
#ifndef _MAX_STRING
#define _MAX_STRING	256
#endif

/* 取得IP地址字符串 */
int
getIPAddrString(
	IN	const DWORD dwIPAddr, 
	OUT	LPSTR lpszIPAddr,
	IN	const DWORD dwBytesBuf
	);
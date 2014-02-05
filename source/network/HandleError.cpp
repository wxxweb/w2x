//******************************************************************************
// File Name		: HandleError.cpp
// Description		: 捕获并输出错误信息
// date created		: 2010-11-20
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-21 20:25 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/


#include "StdAfx.h"		// 标准头文件

/**********************************************************
 * methor 		: putLastError
 * description	: 输出最后一次错误消息
 * parameters	: 无
 * return value	: 无
**********************************************************/
void 
putLastError(
	void
	)
{
	DWORD dwError;
	PVOID lpMsgBuf;

	if (FormatMessageA( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER |			// 自动分配缓存
			FORMAT_MESSAGE_FROM_SYSTEM |				// 系统消息
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError = GetLastError(),					// 错误代码
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// 默认语言
			(LPSTR)&lpMsgBuf,
			0,
			NULL))
	{
		printf("[%d] 出错：%s\n", dwError, lpMsgBuf);
	}
	LocalFree(lpMsgBuf);	// 释放内存
} /* putLastError */

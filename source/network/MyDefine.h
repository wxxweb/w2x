//******************************************************************************
// File Name		: MyDefine.h
// Description		: �Զ����һЩ��ͺ���
// date created		: 2010-11-19
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:53 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#pragma once

/* ��ڲ�����־ */
#ifndef IN
#define IN
#endif

/* ���ڲ�����־ */
#ifndef OUT
#define OUT
#endif

/* ����ڲ�����־ */
#ifndef IN_OUT
#define IN_OUT
#endif

/* �ַ�����󳤶� */
#ifndef _MAX_STRING
#define _MAX_STRING	256
#endif

/* ȡ��IP��ַ�ַ��� */
int
getIPAddrString(
	IN	const DWORD dwIPAddr, 
	OUT	LPSTR lpszIPAddr,
	IN	const DWORD dwBytesBuf
	);
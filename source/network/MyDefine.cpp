//******************************************************************************
// File Name		: MyDefine.cpp
// Description		: �Զ����һЩ��ͺ���
// date created		: 2010-11-23
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-24 16:00 $
// revision			: $ rev : 1 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/

#include "StdAfx.h"		// ��׼ͷ�ļ�


/**********************************************************
   methor 		: getIPAddrString
   description	: ͨ�������������ֽ���IP��ַ����������ֽ�
				  ����ַ�����ʽ��
   parameters	: 
  	[IN]	dwIPAddr : DWORD 
				-- �����������ֽ���IP��ַ��
	[IN]	lpszIPAddr : LPSTR
				-- ���IP��ַ�ַ����Ļ�������
	[IN]	dwBytesBuf : DWORD
				-- �������ֽ�����

   return value	: ���ɹ��򷵻ش洢���ֽ�����������NULL�ַ���
				  ��ʧ���򷵻�-1��
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
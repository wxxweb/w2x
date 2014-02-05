//******************************************************************************
// File Name		: HandleError.cpp
// Description		: �������������Ϣ
// date created		: 2010-11-20
// Last Changed By	: $ Author : wxxweb $
// Last Changed On	: $ Date : 2010-11-21 20:25 $
//******************************************************************************

/*
Copyright (C) 2010 - 2011 By wxxweb, wxxweb@163.com
*/


#include "StdAfx.h"		// ��׼ͷ�ļ�

/**********************************************************
 * methor 		: putLastError
 * description	: ������һ�δ�����Ϣ
 * parameters	: ��
 * return value	: ��
**********************************************************/
void 
putLastError(
	void
	)
{
	DWORD dwError;
	PVOID lpMsgBuf;

	if (FormatMessageA( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER |			// �Զ����仺��
			FORMAT_MESSAGE_FROM_SYSTEM |				// ϵͳ��Ϣ
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError = GetLastError(),					// �������
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// Ĭ������
			(LPSTR)&lpMsgBuf,
			0,
			NULL))
	{
		printf("[%d] ����%s\n", dwError, lpMsgBuf);
	}
	LocalFree(lpMsgBuf);	// �ͷ��ڴ�
} /* putLastError */

//******************************************************************************
// File Name		: FindOthers.h
//
// Description		: ��ģ���а������������Ľӿں���findOthers��echoServer����
//					  ��Ƶ���findOthers�����ı�������Ϊ�ͻ��ˣ�����echoServer
//					  ������Զ������Ϊ�������ˡ���ģ�鹦���ǿͻ���ͨ������
//					  findOthers��������UDP�㲥�������ڷ������˵���echoServer
//					  ��������Ӧ�ͻ��˵Ĺ㲥������ʱ���ͻ��˺ͷ������˽���֪��
//					  ���ĵ�ַ������IP��ַ�����ڷ���UDP���ݰ��Ķ˿ںš�
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
W2X_NETWORK_API BOOL
findOthers(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
   OUT	PSOCKADDR_IN psaRemoteAddr
	);


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
W2X_NETWORK_API BOOL 
echoServer(
	IN	const WORD wAgreedPort,
	IN	const DWORD dwTimeOut,
	IN	const DWORD dwNumEcho
	);
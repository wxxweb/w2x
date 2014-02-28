/*******************************************************************************
 * �ļ���	FindOthers.h
 * 
 * ������	��ģ���а������������Ľӿں���findOthers��echoServer������Ƶ���
 *          findOthers�����ı�������Ϊ�ͻ��ˣ�����echoServer ������Զ������Ϊ
 *          �������ˡ���ģ�鹦���ǿͻ���ͨ������ findOthers��������UDP�㲥����
 *          ���ڷ������˵��� echoServer ��������Ӧ�ͻ��˵Ĺ㲥������ʱ���ͻ���
 *          �ͷ������˽���֪�Է��ĵ�ַ������IP��ַ�����ڷ���UDP���ݰ��Ķ˿ںš�
 * 
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-04-05
 ******************************************************************************/

#pragma once

#include "exports.h"
#include "..\common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


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

W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
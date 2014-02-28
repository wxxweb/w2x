/*******************************************************************************
 * �ļ���	lan_neighbor.h
 * ������	��ģ���а������������Ľӿں���findOthers��echoServer������Ƶ���
 *          findOthers�����ı�������Ϊ�ͻ��ˣ�����echoServer ������Զ������Ϊ
 *          �������ˡ���ģ�鹦���ǿͻ���ͨ������ findOthers��������UDP�㲥����
 *          ���ڷ������˵��� echoServer ��������Ӧ�ͻ��˵Ĺ㲥������ʱ���ͻ���
 *          �ͷ������˽���֪�Է��ĵ�ַ������IP��ַ�����ڷ���UDP���ݰ��Ķ˿ںš���
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-02-10
 ******************************************************************************/

#ifndef __W2X_NETWORK_LAN_NEIGHBOR_H__
#define __W2X_NETWORK_LAN_NEIGHBOR_H__

#include "exports.h"
#include "..\common\macros.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class W2X_NETWORK_API CLanNeighbor
{
public:
	CLanNeighbor(void);
	virtual ~CLanNeighbor(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CLanNeighbor)

public:
	bool SayHello(WORD _remote_port);
	bool Listen(WORD _local_port);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_LAN_NEIGHBOR_H__ */
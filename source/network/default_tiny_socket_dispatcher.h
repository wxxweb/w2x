/*******************************************************************************
 * �ļ���	default_tiny_socket.h
 * ������	����Ĭ�ϵ� CTinySocket ��Ϣ�ַ����� CDefaultTinySocketDispatcher��
 *			�̳��� ITinySocketDispatcher��
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-02-08
 ******************************************************************************/

#ifndef __W2X_NETWORK_DEFAULT_TINY_SOCKET_DISPATCHER_H__
#define __W2X_NETWORK_DEFAULT_TINY_SOCKET_DISPATCHER_H__

#include "interfaces_of_tiny_socket.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


class W2X_NETWORK_API CDefaultTinySocketDispatcher: public ITinySocketDispatcher
{
public:
	CDefaultTinySocketDispatcher(void);
	virtual ~CDefaultTinySocketDispatcher(void);

public:
	virtual void DispatchReceviedMessage(
		const BYTE* _msg_ptr, 
		size_t _msg_bytes
	);

	virtual bool RegisterListener(
		ITinySocketListener* _listener_ptr
	);

	virtual bool UnregisterListener(
		ITinySocketListener* _listener_ptr
	);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END


#endif /* __W2X_NETWORK_DEFAULT_TINY_SOCKET_DISPATCHER_H__ */
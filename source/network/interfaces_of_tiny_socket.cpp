/*******************************************************************************
 * 文件：	interfacess_of_tiny_socket.cpp
 * 描述：	参见 interfacess_of_tiny_socket.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-02-08
 ******************************************************************************/

#include "stdafx.h"
#include "interfaces_of_tiny_socket.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


ITinySocketDispatcher::ITinySocketDispatcher(void)
{
	CTinySocket::RegisterDispatcher(this);
}


ITinySocketDispatcher::~ITinySocketDispatcher(void)
{
	CTinySocket::UnregisterDispatcher(this);
}


ITinySocketMessage::ITinySocketMessage(UINT _msg_id)
	: m_msg_id(_msg_id)
{
}


ITinySocketMessage::~ITinySocketMessage(void)
{

}


UINT ITinySocketMessage::GetMsgId(void) const
{
	return m_msg_id;
}


ITinySocketListener::ITinySocketListener(ITinySocketDispatcher* _dispatcher_ptr)
	: m_tiny_socket_dispatcher_ptr(_dispatcher_ptr)
{
	IF_NULL_ASSERT (m_tiny_socket_dispatcher_ptr)
	{
		// 啥都不做
	}
	else {
		m_tiny_socket_dispatcher_ptr->RegisterListener(this);
	}
}


ITinySocketListener::~ITinySocketListener(void)
{
	IF_NULL_ASSERT (m_tiny_socket_dispatcher_ptr)
	{
		// 啥都不做
	}
	else {
		m_tiny_socket_dispatcher_ptr->UnregisterListener(this);
	}
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
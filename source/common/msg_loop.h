/******************************************************************************
 * 文件：msg_loop.h
 * 描述：	定义了一个消息循环类，使用线程处理消息队列的方式来实现异步处理消息。
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 日期：2013-12-10
 ******************************************************************************/

#ifndef __W2X_COMMON_MSG_LOOP_H__
#define __W2X_COMMON_MSG_LOOP_H__

#include "exports.h"
#include "utility.h"
#include <wtypes.h>

class W2X_COMMON_API CMsgLoop
{
	// 消息处理函数类型
	typedef bool (*MsgHandler)(
		PVOID,		// proc_param
		LPCTSTR,	// msg
		PVOID);		// msg_param

public:
	CMsgLoop(void);
	virtual ~CMsgLoop(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CMsgLoop)

public:
	bool AddMsg(LPCTSTR _msg, PVOID _msg_param);
	bool StartLoopThread(MsgHandler _msg_handler, PVOID _handler_param);
	bool StopLoopThread(void);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

#endif /* __W2X_COMMON_MSG_LOOP_H__ */
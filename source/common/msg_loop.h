/******************************************************************************
 * 文件：msg_loop.h
 * 描述：	定义了一个消息循环类，使用线程循环读取消息队列，以此来实现异步消息处理。
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 日期：2013-12-10
 ******************************************************************************/

#ifndef __W2X_COMMON_MSG_LOOP_H__
#define __W2X_COMMON_MSG_LOOP_H__

#include "exports.h"
#include "macros.h"
#include <wtypes.h>

class W2X_COMMON_API CMsgLoop
{
	// 消息处理器，消息被传入消息处理器中处理，
	// 处理器函数体由用户自定，返回值目前无意义
	typedef bool (CALLBACK *MsgHandler)(
		PVOID /* _handler_param */,	// 处理器参数
		LPCTSTR /* _msg */,			// 字符串消息
		PVOID /* _msg_param */);	// 消息参数

public:
	CMsgLoop(void);
	virtual ~CMsgLoop(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CMsgLoop)

public:
	// 开启消息循环，消息处理器 _msg_handler 必须指向一个已定义的回调函数，
	// 处理器参数 _handler_param 将被传入消息处理器回调函数中，可为 NULL，
	// 如果成功创建了消息循环线程返回 true，否则返回 false
	bool StartLoopThread(MsgHandler _msg_handler, PVOID _handler_param);

	// 停止循环处理消息，但非马上结束消息循环线程，如果有消息正在处理，将等这
	// 个消息处理完成后，才正常退出消息循环线程，未处理的消息将从队列中丢弃
	void StopLoopThread(void);

	// 向队列中添加消息，字符串消息 _msg 和消息参数 _msg_param 将会被传入
	// 消息处理器回调函数中，_msg_param 可为 NULL，消息进入队列后返回 true
	bool AddMsg(LPCTSTR _msg, PVOID _msg_param);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

#endif /* __W2X_COMMON_MSG_LOOP_H__ */
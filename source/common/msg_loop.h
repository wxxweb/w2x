/******************************************************************************
 * �ļ���msg_loop.h
 * ������	������һ����Ϣѭ���࣬ʹ���̴߳�����Ϣ���еķ�ʽ��ʵ���첽������Ϣ��
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�2013-12-10
 ******************************************************************************/

#ifndef __W2X_COMMON_MSG_LOOP_H__
#define __W2X_COMMON_MSG_LOOP_H__

#include "exports.h"
#include "utility.h"
#include <wtypes.h>

class W2X_COMMON_API CMsgLoop
{
	// ��Ϣ����������
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
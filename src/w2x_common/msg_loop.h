/******************************************************************************
 * �ļ���msg_loop.h
 * ������	������һ����Ϣѭ���࣬ʹ���߳�ѭ����ȡ��Ϣ���У��Դ���ʵ���첽��Ϣ����
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�2013-12-10
 ******************************************************************************/

#ifndef __W2X_COMMON_MSG_LOOP_H__
#define __W2X_COMMON_MSG_LOOP_H__

#include "exports.h"
#include "macros.h"
#include <wtypes.h>


W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API CMsgLoop
{
	// ��Ϣ����������Ϣ��������Ϣ�������д���
	// ���������������û��Զ�������ֵĿǰ������
	typedef bool (CALLBACK *MsgHandler)(
		PVOID /* _handler_param */,		// ����������
		LPTSTR /* _msg */,				// ��Ϣ����������ֻΪ�ַ�����
		size_t /* _bytes = 0 */,		// ��Ϣ�ֽ���
		PVOID /* _msg_param = NULL*/	// ��Ϣ����
	);

public:
	CMsgLoop(void);
	virtual ~CMsgLoop(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CMsgLoop)

public:
	// ������Ϣѭ������Ϣ������ _msg_handler ����ָ��һ���Ѷ���Ļص�������
	// ���������� _handler_param ����������Ϣ�������ص������У���Ϊ NULL��
	// ����ɹ���������Ϣѭ���̷߳��� true�����򷵻� false
	bool StartLoopThread(MsgHandler _msg_handler, PVOID _handler_param);

	// ֹͣѭ��������Ϣ���������Ͻ�����Ϣѭ���̣߳��������Ϣ���ڴ���������
	// ����Ϣ������ɺ󣬲������˳���Ϣѭ���̣߳�δ�������Ϣ���Ӷ����ж���
	void StopLoopThread(void);

	// ������������Ϣ����Ϣ _msg����Ϣ�ֽ��� _bytes ����Ϣ���� _msg_param 
	// ���ᱻ������Ϣ�������ص������У�_msg ��ֻΪ�ַ�����Ҳ�����Ƕ��������ݣ�
	// ��� _msg Ϊ�ַ�����_bytes ��Ϊ 0�������������ַ������ȣ�_msg_param 
	// ��Ϊ NULL����Ϣ������к󷵻� true
	bool AddMsg(LPCTSTR _msg, size_t _bytes = 0, PVOID _msg_param = NULL);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_MSG_LOOP_H__ */
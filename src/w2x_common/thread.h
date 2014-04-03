/******************************************************************************
 * �ļ�:		thread.h
 * ����:		�ṩ���̵߳ķ�װ��������߳̽��д�����������
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-04-01
 ******************************************************************************/

#ifndef __W2X_COMMON_THREAD_H__
#define __W2X_COMMON_THREAD_H__

#include "exports.h"
#include "macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(thread)


class W2X_COMMON_API CThread
{
public:
	enum State {
		kNew,			// ֻ�Ƕ��󱻴����ˣ�����δ�����߳�
		kRunnable,		// �߳��Ѿ�����������δ��ʼִ��
		kRunning,		// ����ִ����
		kTerminated,	// �Ѿ�����ִ��
	};

	typedef UINT (CALLBACK* FnRoutine)(PVOID);

public:
	/**
	 * ֻ�ǹ����� CThread ���󣬵�����δ�����̡߳�_name ��Ϊ��������ƣ�
	 * ͬʱҲ������ʶ����Ҫ�������̣߳�����������
	 */
	CThread(LPCTSTR _name);

	/**
	 * CThread ����������ʱ��ֻ��֪ͨ�̣߳�������ǿ����ֹ�̣߳�
	 * �߳�������Ҫ���ݻ�õ�֪ͨ�����Ǻ�ʱ�˳��߳����̺�����
	 */
	virtual ~CThread(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CThread)

public:
	/**
	 * �����̣߳���ʹ֮Ϊ����״̬��_routine Ϊ�̵߳�ִ�����̺�����
	 * _arg_ptr ��Ϊ _routine �ĵ�һ�����������ݡ������� _routine
	 * ���ᱻ�ص���Ҫ���� _routine ִ����Ҫ���ú��� Start��
	 */
	bool Create(FnRoutine _routine, PVOID _arg_ptr);

	/** ��ȷ���߳��Ѿ�������������£����øú���ʹ���̱߳�ִ�С�*/
	bool Start(void);

	/** 
	 * ����߳��Ѿ���ʼִ�У��ȴ����߳���ֹ��ʱ���Ϊ _millis ���룬
	 * INFINITE ��ʾ���޵ȴ���ֱ���߳���ֹ��
	 */
	void Wait(DWORD _millis = INFINITE);

	/** 
	 * ����߳��Ѿ���ʼִ�У���ͣ���߳� _millis ���룬
	 * INFINITE ��ʾ���޵ȴ���ֱ���߳���ֹ��
	 */
	bool Pause(DWORD _millis = INFINITE);

	/** ��ȡ CThread ���� ID��ÿ�� CThread ������һ��Ψһ�ĵ��� ID�� */
	DWORD GetId(void) const;

	/** ��ȡ CThread ��������� */
	LPCTSTR GetName(void) const;
	
	/** ��ȡ���������̵߳��ں˶��� ID */
	DWORD GetThreadId(void) const;

	/** ��ȡ�������̵߳��ں˶����� */
	HANDLE GetThreadHandle(void) const;

	/** ��ȡ�̵߳�����״̬ */
	State GetState(void) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(thread)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_THREAD_H__ */
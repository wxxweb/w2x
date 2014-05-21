/*******************************************************************************
文件:	thread.h
描述:	提供对线程的封装，方便对线程进行创建、操作。
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2014-04-01
修改:	2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_THREAD_H__
#define __W2X_COMMON_THREAD_H__

#include "macros.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(thread)


class W2X_COMMON_API CThread
{
public:
	enum State {
		kNew,			// 只是对象被创建了，但尚未创建线程
		kRunnable,		// 线程已经创建，但尚未开始执行
		kRunning,		// 正在执行中
		kTerminated,	// 已经结束执行
	};

	typedef UINT (CALLBACK* FnRoutine)(PVOID);

public:
	/**
	 * 只是构建了 CThread 对象，但是尚未创建线程。_name 作为对象的名称，
	 * 同时也用来标识即将要创建的线程，方便后面管理。
	 */
	CThread(LPCTSTR _name);

	/**
	 * CThread 对象析构的时候只会通知线程，并不会强制终止线程，
	 * 线程例程需要根据获得的通知，考虑何时退出线程例程函数。
	 */
	virtual ~CThread(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CThread)

public:
	/**
	 * 创建线程，并使之为挂起状态。_routine 为线程的执行例程函数，
	 * _arg_ptr 作为 _routine 的第一个参数被传递。创建后 _routine
	 * 不会被回调，要想让 _routine 执行需要调用函数 Start。
	 */
	bool Create(FnRoutine _routine, PVOID _arg_ptr);

	/** 在确保线程已经被创建的情况下，调用该函数使得线程被执行。*/
	bool Start(void);

	/** 
	 * 如果线程已经开始执行，等待该线程终止的时间最长为 _millis 毫秒，
	 * INFINITE 表示无限等待，直到线程终止。
	 */
	void Wait(DWORD _millis = INFINITE);

	/** 
	 * 如果线程已经开始执行，暂停该线程 _millis 毫秒，
	 * INFINITE 表示无限等待，直到线程终止。
	 */
	bool Pause(DWORD _millis = INFINITE);

	/** 获取 CThread 对象 ID，每个 CThread 对象都有一个唯一的递增 ID。 */
	DWORD GetId(void) const;

	/** 获取 CThread 对象的名称 */
	LPCTSTR GetName(void) const;
	
	/** 获取被创建的线程的内核对象 ID */
	DWORD GetThreadId(void) const;

	/** 获取被创建线程的内核对象句柄 */
	HANDLE GetThreadHandle(void) const;

	/** 获取线程的运行状态 */
	State GetState(void) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(thread)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_THREAD_H__ */
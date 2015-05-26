/******************************************************************************
�ļ�:	timer.h
����:	���Դ����� Timer �����Ա㰴ָ����ʱ��˳�����д��롣ʹ�� Start() ������������ʱ����
		Ϊ Timer �¼�����¼����������Ա㽫��������Ϊ����ʱ��������С����Դ��� Timer ����
		������һ�λ�ָ������ظ����У��Ӷ����ƻ�ִ�д��롣��ʱ������̸���ʱ��Ϊ 50 ���룬
		���ܻᰴ����ƫ��ļ�������¼���
����:	wxxweb
����:	wxxweb@gmail.com
����:	2015-04-10
�޸�:
*******************************************************************************/

#ifndef __W2X_COMMON_TIMER_H__
#define __W2X_COMMON_TIMER_H__
#pragma once


#include "event_dispatcher.h"
#include "timer_event.h"


W2X_NS_BEGIN
W2X_DEFINE_NS_BEGIN(utils)


class CTimer;
typedef CRefPtr<CTimer> TimerPtr;


class W2X_COMMON_API CTimer: public w2x::events::IEventDispatcher
{
private:
	CTimer(DWORD _delay, DWORD _repeat_count);
	virtual ~CTimer(void);

public:
	/**
	 * @brief ʹ��ָ���� _delay �� _repeat_ount ״̬�����µ� Timer ����
	 * @note ��ʱ�������Զ�������������� Start() ��������������
	 * @param _delay ��ʱ���¼�����ӳ٣��Ժ���Ϊ��λ�������С�� 50 ������Ϊ 50 ���롣
	 * @param _repeat_count ָ���ظ������� ����ָ��������Ȼ��ֹͣ�����Ϊ INFINITE��
	 *                      ���ʱ���ظ����޴��������Ϊ 0����ֻ����һ�Ρ�
	 */
	static TimerPtr Create(DWORD _delay, DWORD _repeat_count = INFINITE);

public: // ��ʱ����������
	/**
	 * �����ʱ���������У���ֹͣ��ʱ�������� currentCount �������Ϊ 0�����������������ð�ť��
	 * Ȼ���ڵ��� Start() �󣬽����м�ʱ��ʵ�������д���Ϊָ�����ظ��������� _repeat_count ֵ���ã���
	 */
	void Reset(void);

	/** �����ʱ����δ���У���������ʱ���� */
	bool Start(void);

	/** 
	 * @brief ֹͣ��ʱ����
	 * ����ڵ��� Stop() ����� Start()���򽫼������м�ʱ��ʵ����
	 * ���д���Ϊʣ����ظ��������� repeatCount �������ã���
	 */
	void Stop(void);

public: // ��ʱ�����Ի�ȡ/����

	/** ��ȡ��ʱ���ĵ�ǰ״̬�������ʱ���������У���Ϊ true������Ϊ false�� */
	bool IsRunning(void) const;

	/** ��ȡ��ʱ���� 0 ��ʼ�󴥷����ܴ���������������˼�ʱ������ֻ��������ú�Ĵ��������� */
	DWORD GetCurrentCount(void) const;

	/** ��ȡ��ʱ���¼�����ӳ٣��Ժ���Ϊ��λ���� */
	DWORD GetDelay(void) const;

	/**
	 * @brief ���ü�ʱ���¼�����ӳ٣��Ժ���Ϊ��λ����
	 * ����ڼ�ʱ����������ʱ�����ӳټ�������ʱ��������ͬ�� repeatCount ��������������
	 * ���С�� 50 ��������Ϊ 50 ���롣
	 */
	void SetDelay(DWORD _value);

	/** ��ȡ��ʱ�������ܴ��� */
	DWORD GetRepeatCount(void) const;
	
	/**
	 * @brief ���ü�ʱ�������ܴ�����
	 * ����ظ���������Ϊ 0�����ʱ���������������У�ֱ�������� stop() ���������ֹͣ��
	 * ����ظ�������Ϊ 0�������м�ʱ�������д���Ϊָ���Ĵ�����������õ� repeatCount
	 * �������ڻ�С�� currentCount�����ʱ����ֹͣ���Ҳ����ٴδ�����
	 */
	void SetRepeatCount(DWORD _value);

public: // ʵ�� events::IEventDispatcher �ӿ�
	virtual bool AddEventListener(
		LPCTSTR _event_type,
		w2x::events::EventListenerPtr& _listener
		);

	virtual bool DispatchEvent(w2x::events::CEvent& _event) const;

	virtual bool HasEventListener(LPCTSTR _event_type) const;

	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		const w2x::events::EventListenerPtr& _listener
		);

	virtual bool RemoveEventListener(
		LPCTSTR _event_type,
		LPCTSTR _listener_id
		);

private:
	class CImpl;
	CImpl* const m_impl;
};


W2X_DEFINE_NS_END(utils)
W2X_NS_END


#endif // __DAOPHONE_COMMON_TIMER_H__
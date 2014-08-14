/*******************************************************************************
文件:	event_listener.cpp
描述:	参见 event_listener.h
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2014-08-14
修改:
*******************************************************************************/

#include "stdafx.h"
#include "event_listener.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class CEventListener;


IEventListener::~IEventListener(void)
{}


class CEventListener: public IEventListener
{
public:
	CEventListener(LPCTSTR _listener_id, PVOID _target, Function _function);
	CEventListener(LPCTSTR _listener_id, IBase* _target, Selector _selector);
	virtual ~CEventListener(void);

public:
	virtual inline bool IsValid(void) const;

	virtual inline void SetEnabled(bool _enabled);

	virtual inline bool IsEnabled(void) const;

	virtual inline void SetRegistered(bool _registered);

	virtual inline bool IsRegistered() const;

	virtual inline LPCTSTR GetListenerId(void) const;

	virtual void Execute(const CEvent& _event);

private:
	inline bool SetListenerId(LPCTSTR _listener_id);

private:
	bool m_enabled;
	bool m_registered;
	Function m_function;
	Selector m_selector;
	PVOID m_selector_target;
	LPTSTR m_listener_id;
};


CEventListener::CEventListener(
	LPCTSTR _listener_id,
	PVOID _target,
	Function _function
	)
	: m_enabled(true)
	, m_registered(false)
	, m_function(_function)
	, m_selector(NULL)
	, m_selector_target(_target)
	, m_listener_id(NULL)
{
	this->SetListenerId(_listener_id);
}


CEventListener::CEventListener(
	LPCTSTR _listener_id, 
	IBase* _target, 
	Selector _function
	)
	: m_enabled(true)
	, m_registered(false)
	, m_function(NULL)
	, m_selector(_function)
	, m_selector_target(_target)
	, m_listener_id(NULL)
{
	this->SetListenerId(_listener_id);
}


CEventListener::~CEventListener(void)
{

}


inline bool CEventListener::IsValid(void) const
{
	return (NULL != m_function) || 
		(NULL != m_selector_target && NULL != m_selector);
}

inline void CEventListener::SetEnabled(bool _enabled)
{
	m_enabled = _enabled;
}

inline bool CEventListener::IsEnabled(void) const
{
	return m_enabled;
}

inline void CEventListener::SetRegistered(bool _registered)
{
	m_registered = _registered;
}

inline bool CEventListener::IsRegistered() const
{
	return m_registered;
}

inline LPCTSTR CEventListener::GetListenerId(void) const
{
	return m_listener_id;
}


inline bool CEventListener::SetListenerId(LPCTSTR _listener_id)
{
	_ASSERT(NULL != _listener_id);
	if (NULL == m_listener_id) {
		return false;
	}
	SAFE_DELETE_ARRAY(m_listener_id);
	const size_t words_count = _tcslen(_listener_id) + 1;
	m_listener_id = new TCHAR[words_count];
	_tcscpy_s(m_listener_id, words_count, _listener_id);
	return true;
}


void CEventListener::Execute(const CEvent& _event)
{
	if (NULL != m_function) {
		m_function(m_selector_target, _event);
	}
	else if (NULL != m_selector_target && NULL != m_selector) {
		IBase* selector_target = static_cast<IBase*>(m_selector_target);
		(selector_target->*m_selector)(_event);
	}
}


EventListenerPtr IEventListener::Create(
	LPCTSTR _listener_id,
	PVOID _target,
	Function _function
	)
{
	_ASSERT(NULL != _function);
	if (NULL == _function) {
		return NULL;
	}
	return EventListenerPtr(new CEventListener(_listener_id, _target, _function));
}


EventListenerPtr IEventListener::Create(
	LPCTSTR _listener_id,
	IBase* _target,
	Selector _selector
	)
{
	_ASSERT(NULL != _target);
	_ASSERT(NULL != _selector);
	if (NULL == _target || NULL == _selector) {
		return NULL;
	}
	return EventListenerPtr(new CEventListener(_listener_id, _target, _selector));
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END
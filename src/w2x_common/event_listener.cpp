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
	CEventListener(LPCTSTR _listener_id, const Callback& _callback);
	virtual ~CEventListener(void);

public:
	virtual inline bool IsValid(void) const;

	virtual inline void SetEnabled(bool _enabled);

	virtual inline bool IsEnabled(void) const;

	virtual inline void SetRegistered(bool _registered);

	virtual inline bool IsRegistered() const;

	virtual inline LPCTSTR GetListenerId(void) const;

	virtual inline void Execute(CEvent& _event);

private:
	inline void SetListenerId(LPCTSTR _listener_id);

private:
	bool m_enabled;
	bool m_registered;
	Callback m_callback;
	LPTSTR m_listener_id;

	static size_t sm_next_auto_listener_id;
};


size_t CEventListener::sm_next_auto_listener_id = 0;


CEventListener::CEventListener(LPCTSTR _listener_id, const Callback& _callback)
	: m_enabled(true)
	, m_registered(false)
	, m_callback(_callback)
	, m_listener_id(NULL)
{
	this->SetListenerId(_listener_id);
}


CEventListener::~CEventListener(void)
{

}


inline bool CEventListener::IsValid(void) const
{
	return (NULL != m_callback);
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


inline void CEventListener::SetListenerId(LPCTSTR _listener_id)
{
	if (NULL == _listener_id || TEXT('\0') == _listener_id[0]) {
		m_listener_id = new TCHAR[32];
		_stprintf_s(m_listener_id, 32,
			TEXT("w2x:listener:%d"), sm_next_auto_listener_id++);
		return;
	}
	SAFE_DELETE_ARRAY(m_listener_id);
	const size_t words_count = _tcslen(_listener_id) + 1;
	m_listener_id = new TCHAR[words_count];
	_tcscpy_s(m_listener_id, words_count, _listener_id);
}


inline void CEventListener::Execute(CEvent& _event)
{
	if (NULL != m_callback) {
		m_callback(_event);
	}
}


EventListenerPtr IEventListener::Create(
	LPCTSTR _listener_id, 
	const Callback& _callback
	)
{
	_ASSERT(NULL != _callback);
	if (NULL == _callback) {
		return NULL;
	}
	return EventListenerPtr(new CEventListener(_listener_id, _callback));
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END
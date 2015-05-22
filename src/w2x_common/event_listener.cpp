/*******************************************************************************
 * 文件:	event_listener.cpp
 * 描述:	参见 event_listener.h
 * 作者:	wu.xiongxing					
 * 邮箱:	wxxweb@gmail.com
 * 日期:	2014-08-14
 * 修改:	2015-05-22
 ******************************************************************************/

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

	virtual inline bool IsSame(const EventListenerPtr& _right) const;

	virtual inline LPCTSTR GetListenerId(void) const;

	virtual inline void SetEnabled(bool _enabled);

	virtual inline bool IsEnabled(void) const;

	virtual inline void SetRegistered(bool _registered);

	virtual inline bool IsRegistered() const;

	virtual inline void SetProtected(bool _protected);

	virtual inline bool IsProtected(void) const;

	virtual const Callback& GetCallback(void) const;

	virtual inline void Execute(CEvent& _event);

private:
	inline void SetListenerId(LPCTSTR _listener_id);

	LPTSTR GenerateListenerId(void) const;

private:
	bool m_enabled;
	bool m_registered;
	bool m_protected;
	Callback m_callback;
	LPTSTR m_listener_id;
};


CEventListener::CEventListener(LPCTSTR _listener_id, const Callback& _callback)
	: m_enabled(true)
	, m_registered(false)
	, m_protected(false)
	, m_callback(_callback)
	, m_listener_id(NULL)
{
	this->SetListenerId(_listener_id);
}


CEventListener::~CEventListener(void)
{
	SAFE_DELETE_ARRAY(m_listener_id);
}


inline bool CEventListener::IsSame(const EventListenerPtr& _right) const
{
	const IEventListener* const right_ptr = _right.get();
	if (NULL == right_ptr) {
		return false;
	}
	if (right_ptr == this) {
		return true;
	}
// 	if (right_ptr->GetCallback() == m_callback) {
// 		return true;
// 	}
	if (0 == _tcscmp(m_listener_id, right_ptr->GetListenerId())) {
		return true;
	}
	return false;
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


inline void CEventListener::SetProtected(bool _protected)
{
	m_protected = _protected;
}


inline bool CEventListener::IsProtected(void) const
{
	return m_protected;
}


inline void CEventListener::SetListenerId(LPCTSTR _listener_id)
{
	SAFE_DELETE_ARRAY(m_listener_id);
	if (NULL == _listener_id || TEXT('\0') == _listener_id[0]) {
		m_listener_id = this->GenerateListenerId();
		return;
	}
	
	const size_t words_count = _tcslen(_listener_id) + 1;
	m_listener_id = new TCHAR[words_count];
	_tcscpy_s(m_listener_id, words_count, _listener_id);
}


LPTSTR CEventListener::GenerateListenerId(void) const
{
	const std::type_info& target_type = m_callback.target_type();

	TCHAR hash_code_str[16] = TEXT("");
	_stprintf_s(hash_code_str, TEXT("%u"), target_type.hash_code());
	const size_t hash_code_len = _tcslen(hash_code_str);

	LPCSTR wrap_begin = "_Callable_pmf<";
	LPCSTR type_name_a = target_type.name();
	LPCSTR copy_begin = strstr(type_name_a, wrap_begin);

	if (NULL == copy_begin) {
		LPTSTR listener_id = new TCHAR[hash_code_len + 1];
		_tcscpy_s(listener_id, hash_code_len + 1, hash_code_str);
		return listener_id;
	}

	copy_begin += strlen(wrap_begin);
	LPCSTR copy_end = strstr(copy_begin, ",class");
	if (NULL == copy_end) {
		copy_end = copy_begin + strlen(copy_begin);
	}

	const size_t type_name_len = copy_end - copy_begin;
	const size_t buf_words = type_name_len + hash_code_len + 1;
	LPTSTR listener_id = new TCHAR[buf_words];
	memset(listener_id, 0, buf_words * sizeof(TCHAR));
	
	for (size_t i = 0; copy_begin + i < copy_end; ++i) {
		listener_id[i] = static_cast<TCHAR>(copy_begin[i]);
	}
	_tcscpy_s(listener_id + type_name_len, buf_words - type_name_len, hash_code_str);
	return listener_id;
}


const IEventListener::Callback& CEventListener::GetCallback(void) const
{
	return m_callback;
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
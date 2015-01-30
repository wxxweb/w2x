/******************************************************************************
�ļ�:	string_util.h
����:	�ַ������ʵ�ù��ߣ�������չ std::string �� std::wstring �Ĺ��ܡ�
����:	wxxweb
����:	wxxweb@gmail.com
����:	2014-12-03
�޸�:
*******************************************************************************/

#ifndef __W2X_COMMON_STRING_UTIL_H__
#define __W2X_COMMON_STRING_UTIL_H__
#pragma once

#include <string>
#include "macros.h"

/** ����UNICODE����ʹ�ÿ��ַ�����ַ���C++�ַ��� */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


W2X_NS_BEGIN


class W2X_COMMON_API CStringUtil
{
public:
	CStringUtil(void);

	/** ͨ������ STL �ַ������������󣬲��� CStringUtil ��ʱ���� STL ���� */
	template <class Str> CStringUtil(Str& _string);

	~CStringUtil(void);

private:
	CStringUtil(const CStringUtil&);
	void operator =(const CStringUtil&);

public:
	/** ���� STL �ַ������������󣬲��� CStringUtil ��ʱ���� STL ���� */
	template <class Str>
	inline void Attach(Str& _string);

	/** ���� STL �ַ��������ٶ���ִ�в��� */
	inline void Detach(void);

	/** ��ȡ������ std::string ����ָ�� */
	inline std::string* GetStringA(void) const;

	/** ��ȡ������ std::wstring ����ָ�� */
	inline std::wstring* GetStringW(void) const;

	/** �Ա����ӵ� STL �ַ������и�ʽ������ */
	template <class Ch>
	inline void FormatV(const Ch* _format, va_list _args);

	/** �Ա����ӵ� STL �ַ������и�ʽ������ */
	template <class Ch>
	inline void Format(const Ch* _format, ...);

	/** ֱ�Ӷ� STL �ַ������и�ʽ�����������踽�� */
	template <class Str, class Ch>
	static inline void Format(Str& _string, const Ch* _format, ...);

private:
	bool m_is_mbs;	// �Ƿ�Ϊ���ֽ��ַ���
	void* m_str;
	void* m_data;
	size_t m_data_count;
};


template <class Str> CStringUtil::CStringUtil(Str& _string)
	: m_is_mbs(typeid(_string) == typeid(std::string))
	, m_str(&_string)
	, m_data(nullptr)
	, m_data_count(0)
{}


template <class Str>
inline void CStringUtil::Attach(Str& _string)
{
	m_is_mbs = typeid(_string) == typeid(std::string);
	m_str = &_string;
}


inline void CStringUtil::Detach(void)
{
	m_str = nullptr;
}


inline std::string* CStringUtil::GetStringA(void) const
{
	return reinterpret_cast<std::string*>(m_str);
}


inline std::wstring* CStringUtil::GetStringW(void) const
{
	return reinterpret_cast<std::wstring*>(m_str);
}


template <class Ch>
void CStringUtil::FormatV(const Ch* _format, va_list _args)
{
	size_t formatted_count = 0;
	if (m_is_mbs) {
		formatted_count = 1 + _vscprintf(
			reinterpret_cast<const char*>(_format), _args);
	} else {
		formatted_count = 1 + _vscwprintf(
			reinterpret_cast<const wchar_t*>(_format), _args);
	}
	if (0 == formatted_count) {
		_ASSERT(false);
		return;
	}

	if (m_data_count < formatted_count) {
		delete [] m_data;
		m_data_count = formatted_count;
		m_data = new Ch[m_data_count];
	}

	if (m_is_mbs) {
		vsprintf_s(reinterpret_cast<char*>(m_data), m_data_count,
			reinterpret_cast<const char*>(_format), _args);
		*reinterpret_cast<std::string*>(m_str) = reinterpret_cast<char*>(m_data);
	} else {
		vswprintf_s(
			reinterpret_cast<wchar_t*>(m_data), m_data_count,
			reinterpret_cast<const wchar_t*>(_format), _args);
		*reinterpret_cast<std::wstring*>(m_str) = reinterpret_cast<wchar_t*>(m_data);
	}
}


template <class Ch>
inline void CStringUtil::Format(const Ch* _format, ...)
{
	if (nullptr == m_str || nullptr == _format || 0 == _format[0]) {
		_ASSERT(false);
		return;
	}

	va_list arg_list;
	va_start(arg_list, _format);
	this->FormatV(_format, arg_list);
	va_end(arg_list);
}


template <class Str, class Ch>
inline void CStringUtil::Format(Str& _string, const Ch* _format, ...)
{
	CStringUtil util(_string);

	va_list arg_list;
	va_start(arg_list, _format);
	util.FormatV(_format, arg_list);
	va_end(arg_list);
}


W2X_NS_END

#endif // __W2X_COMMON_STRING_UTIL_H__
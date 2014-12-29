/******************************************************************************
文件:	string_util.h
描述:	字符串相关实用工具，用于扩展 std::string 和 std::wstring 的功能。
作者:	wxxweb
邮箱:	wxxweb@gmail.com
创建:	2014-12-03
修改:
*******************************************************************************/

#ifndef __W2X_COMMON_STRING_UTIL_H__
#define __W2X_COMMON_STRING_UTIL_H__
#pragma once

#include <string>
#include "macros.h"

W2X_NS_BEGIN


class CStringUtil
{
public:
	CStringUtil(void);

	/** 通过附加 STL 字符串来构建对象，操作 CStringUtil 即时操作 STL 对象 */
	template <class Str> CStringUtil(Str& _string);

	~CStringUtil(void);

private:
	CStringUtil(const CStringUtil&);
	void operator =(const CStringUtil&);

public:
	/** 附加 STL 字符串来构建对象，操作 CStringUtil 即时操作 STL 对象 */
	template <class Str>
	inline void Attach(Str& _string);

	/** 分离 STL 字符串，不再对其执行操作 */
	inline void Detach(void);

	/** 获取被附加 std::string 对象指针 */
	inline std::string* GetStringA(void) const;

	/** 获取被附加 std::wstring 对象指针 */
	inline std::wstring* GetStringW(void) const;

	/** 对被附加的 STL 字符串进行格式化操作 */
	template <class Ch>
	inline void FormatV(const Ch* _format, va_list _args);

	/** 对被附加的 STL 字符串进行格式化操作 */
	template <class Ch>
	inline void Format(const Ch* _format, ...);

	/** 直接对 STL 字符串进行格式化操作，无需附加 */
	template <class Str, class Ch>
	static inline void Format(Str& _string, const Ch* _format, ...);

private:
	bool m_is_mbs;	// 是否为多字节字符串
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
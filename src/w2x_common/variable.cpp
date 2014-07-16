/*****************************************************************************
 * 文名：	variable.h
 * 描述：	参见 variable.cpp
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-03-13
 *****************************************************************************/

#include "stdafx.h"
#include "Variable.h"
#include "macros.h"

W2X_NAME_SPACE_BEGIN

static const size_t MAX_NUMBER_LEN = 65;

static Var s_var_null;							// 用于返回的空对象
static std::map<INT, Var> s_nt_2_str_map;		// 保存负数转换的字符串
static std::map<DWORD, Var> s_dw_2_str_map;		// 保存整数转换的字符串
static std::map<DOUBLE, Var> s_dbl_2_str_map;	// 保存浮点数转换的字符串


Var::Var(void) : m_type(kNull), m_num_str(NULL), m_length(0)
{
	this->InitValue();
}

Var::Var(bool _value) : m_type(kBool), m_num_str(NULL), m_length(0)
{ 
	this->InitValue();
	m_value.bl = _value;
}

Var::Var(INT _value) : m_type(kInt), m_num_str(NULL), m_length(0)
{ 
	this->InitValue();
	m_value.nt = _value;
}

Var::Var(UINT _value) : m_type(kUint), m_num_str(NULL), m_length(0)
{ 
	this->InitValue();
	m_value.unt = _value;
}

Var::Var(DWORD _value) : m_type(kDword), m_num_str(NULL), m_length(0)
{ 
	this->InitValue();
	m_value.dw = _value;
}

Var::Var(DOUBLE _value) : m_type(kNumber), m_num_str(NULL), m_length(0)
{ 
	this->InitValue();
	m_value.dbl = _value;
}

Var::Var(LPCTSTR _value) : m_type(kString), m_num_str(NULL), m_length(0)
{
	this->InitValue();
	m_length = (NULL == _value) ? 0 : _tcslen(_value);
	const size_t nCount = m_length + 1;
	m_value.str = new TCHAR[nCount];
	if (NULL != _value) {
		memcpy((void*)m_value.str, (void*)_value, nCount * sizeof(TCHAR));
	}
	else {
		*(m_value.str) = TEXT('\0');
	}
}

Var::Var(const Array& _value) : m_type(kArray), m_num_str(NULL), m_length(0)
{
	this->InitValue();
	m_length = _value.size();
	m_value.ary = new Array();
	*(m_value.ary) = _value;
}

Var::Var(const Object& _value) : m_type(kObject), m_num_str(NULL), m_length(0)
{
	this->InitValue();
	m_length = _value.size();
	m_value.obj = new Object();
	*(m_value.obj) = _value;
}

Var::Var(const Var& right) : m_type(right.m_type), m_num_str(NULL), m_length(0)
{
	this->InitValue();
	Var::Copy(*this, right);
}

Var::~Var(void)
{
	if (kString == m_type)
	{
		SAFE_DELETE_ARRAY(m_value.str);
	}
	else if (kArray == m_type)
	{
		SAFE_DELETE(m_value.ary);
	}
	else if (kObject == m_type)
	{
		SAFE_DELETE(m_value.obj);
	}
}

Var& Var::operator =(bool _value)
{
	return Var::Copy(*this, Var(_value));
}

Var& Var::operator =(INT _value)
{
	return Var::Copy(*this, Var(_value));
}

Var& Var::operator =(UINT _value)
{
	return Var::Copy(*this, Var(_value));
}

Var& Var::operator =(DWORD _value)
{
	return Var::Copy(*this, Var(_value));
}

Var& Var::operator =(DOUBLE _value)
{
	return Var::Copy(*this, Var(_value));
}

Var& Var::operator =(LPCTSTR _value)
{
	return Var::Copy(*this, Var(_value));
}

Var& Var::operator =(const Var& _right)
{
	return Var::Copy(*this, _right);
}

Var& Var::operator =(const Array& _right)
{
	return Var::Copy(*this, Var(_right));
}

Var& Var::operator =(const Object& _right)
{
	return Var::Copy(*this, Var(_right));
}

bool Var::operator <(const Var& right) const
{
	if (kString == m_type && kString == right.m_type)
	{
		LPCTSTR left_str  = m_value.str;
		LPCTSTR right_str = right.m_value.str;
		int left_nt = _ttoi(left_str);
		int right_nt = _ttoi(right_str);
		if (0 == left_nt && TEXT('0') != left_str[0] ||
			0 == right_nt && TEXT('0') != right_str[0])
		{
			return 0 < _tcscmp(left_str, right_str);
		}
		return left_nt < right_nt;
	}
	else if (kString == m_type)
	{
		return _ttoi(m_value.str) < right.m_value.nt;
	}
	else if (kString == right.m_type)
	{
		return m_value.nt < _ttoi(right.m_value.str);
	}
	else
	{
		return m_value.nt < right.m_value.nt;
	}
}

bool Var::operator ==(const Var& right) const
{
	if (kBool == m_type || kBool == right.m_type)
	{
		return (bool)(*this) == (bool)right;
	}
	else if (kString == m_type && kString == right.m_type)
	{
		return 0 == _tcsicmp(m_value.str, right.m_value.str);
	}
	else if (kString == m_type)
	{
		return _ttoi(m_value.str) == right.m_value.nt;
	}
	else if (kString == right.m_type)
	{
		return m_value.nt == _ttoi(right.m_value.str);
	}
	else
	{
		return m_value.nt == right.m_value.nt;
	}
}

Var& Var::operator [](LPCTSTR _index)
{
	if (kObject == m_type && NULL != m_value.obj)
	{
		return (*m_value.obj)[_index];
	}
	else if (kArray == m_type && NULL != m_value.ary)
	{
		return m_value.ary->at(UINT(Var(_index)));
	}

	throw std::out_of_range("Invalid w2x::Var subscript");
}

Var& Var::operator [](INT _index)
{
	if (kObject == m_type && NULL != m_value.obj)
	{
		return (*m_value.obj)[_index];
	}
	else if (kArray == m_type && NULL != m_value.ary)
	{
		return m_value.ary->at(_index);
	}

	throw std::out_of_range("Invalid w2x::Var subscript");
}

Var::operator bool(void) const
{
	if (kString == m_type)
	{
		bool is_false = 0 == m_length;
		is_false |= 1 == m_length && TEXT('0') == m_value.str[0];
		return !is_false;
	}
	else if (kArray == m_type || kObject == m_type)
	{
		return false;
	}
	return 0 != m_value.nt;
}

Var::operator INT(void) const
{
	if (kString == m_type)
	{
		return _ttoi(m_value.str);
	}
	else if (kNumber == m_type)
	{
		return static_cast<INT>(m_value.dbl);
	}
	else if (kArray == m_type || kObject == m_type)
	{
		return 0;
	}
	return m_value.nt;
}

Var::operator UINT(void) const
{
	if (kString == m_type)
	{
		return static_cast<UINT>(_ttoi(m_value.str));
	}
	else if (kNumber == m_type)
	{
		return static_cast<UINT>(m_value.dbl);
	}
	else if (kArray == m_type || kObject == m_type)
	{
		return 0;
	}
	return m_value.unt;
}

Var::operator DWORD(void) const
{
	if (kString == m_type)
	{
		return static_cast<DWORD>(_ttoi(m_value.str));
	}
	else if (kNumber == m_type)
	{
		return static_cast<DWORD>(m_value.dbl);
	}
	else if (kArray == m_type || kObject == m_type)
	{
		return 0;
	}
	return m_value.dw;
}

Var::operator DOUBLE(void) const
{
	if (kString == m_type)
	{
		return static_cast<DOUBLE>(_ttof(m_value.str));
	}
	else if (kArray == m_type || kObject == m_type)
	{
		return 0.0F;
	}
	else if (kInt == m_type)
	{
		return static_cast<DOUBLE>(m_value.nt);
	}
	else if (kNumber != m_type)
	{
		return static_cast<DOUBLE>(m_value.dw);
	}
	return m_value.dbl;
}

Var::operator LPCTSTR(void) const
{
	if (kString == m_type)
	{
		return m_value.str;
	}
	else if (kNull == m_type)
	{
		return TEXT("");
	}
	else if (kArray ==  m_type)
	{
		return TEXT("[Array]");
	}
	else if (kObject == m_type)
	{
		return TEXT("[Object]");
	}
	else if (kBool == m_type || 0 == m_value.nt || 1 == m_value.nt)
	{
		return m_value.bl ? TEXT("true") : TEXT("false");
	}
	else if (NULL != m_num_str)
	{
		return m_num_str;
	}
	
	if (kNumber == m_type)
	{
		std::map<DOUBLE, Var>::iterator it = s_dbl_2_str_map.find(m_value.dbl);
		if (s_dbl_2_str_map.end() == it) 
		{
			TCHAR temp_str[MAX_NUMBER_LEN] = TEXT("");
			_stprintf_s(temp_str, MAX_NUMBER_LEN, TEXT("%g"), m_value.dbl);
			s_dbl_2_str_map[m_value.dbl] = temp_str;
			it = s_dbl_2_str_map.find(m_value.dbl);
		}
		const_cast<Var*>(this)->m_num_str = it->second;
	}
	else if(0 < m_value.nt)
	{
		std::map<DWORD, Var>::iterator it = s_dw_2_str_map.find(m_value.dw);
		if (s_dw_2_str_map.end() == it)
		{
			TCHAR temp_str[MAX_NUMBER_LEN] = TEXT("");
			_ui64tot_s(m_value.dw, temp_str, MAX_NUMBER_LEN, 10);
			s_dw_2_str_map[m_value.dw] = temp_str;
			it = s_dw_2_str_map.find(m_value.dw);
		}
		const_cast<Var*>(this)->m_num_str = it->second;
	}
	else if (0 > m_value.nt)
	{
		std::map<INT, Var>::iterator it = s_nt_2_str_map.find(m_value.nt);
		if (s_nt_2_str_map.end() == it)
		{
			TCHAR temp_str[MAX_NUMBER_LEN] = TEXT("");
			_i64tot_s(m_value.nt, temp_str, MAX_NUMBER_LEN, 10);
			s_nt_2_str_map[m_value.nt] = temp_str;
			it = s_nt_2_str_map.find(m_value.nt);
		}
		const_cast<Var*>(this)->m_num_str = it->second;
	}
	else 
	{
		return TEXT("");
	}

	return m_num_str;
}

Var::operator Array*(void) const
{
	if (kArray == m_type)
	{
		return m_value.ary;
	}
	return NULL;
}

Var::operator Object*(void) const
{
	if (kObject == m_type)
	{
		return m_value.obj;
	}
	return NULL;
}

Var::Type Var::GetType(void) const 
{
	return m_type;
}

bool Var::IsTypeOf(Type _type) const
{
	return _type == m_type;
}

UINT Var::GetLength(void) const
{
	return m_length;
}

void Var::InitValue(void)
{
	memset((void*)&m_value, 0, sizeof(m_value));
}

Var& Var::Copy(Var& left, const Var& right)
{
	if (&left == &right)
	{
		return left;
	}
	if (kString == left.m_type && NULL != left.m_value.str)
	{
		SAFE_DELETE_ARRAY(left.m_value.str);
	}
	left.m_type = right.m_type;
	left.m_length = right.m_length;
	if (kString == right.m_type)
	{
		const size_t nCount = right.m_length + 1;
		left.m_value.str = new TCHAR[nCount];
		memcpy((void*)left.m_value.str, 
			(void*)right.m_value.str, nCount * sizeof(TCHAR));
	}
	else
	{
		left.m_num_str = NULL;
		if (kArray == right.m_type)
		{
			if (NULL == left.m_value.ary) {
				left.m_value.ary = new Array(*(right.m_value.ary));
			}
			else {
				*(left.m_value.ary) = *(right.m_value.ary);
			}
		}
		else if (kObject == right.m_type)
		{
			if (NULL == left.m_value.obj) {
				left.m_value.obj = new Object(*(right.m_value.obj));
			}
			else {
				*(left.m_value.obj) = *(right.m_value.obj);
			}
		}
		else {
			left.m_value = right.m_value;
		}
	}
	
	return left;
}


W2X_NAME_SPACE_END
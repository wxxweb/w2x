/*****************************************************************************
 * 文名：	variable.h
 * 描述：	Var 类仿弱类型变量，可接受数值和字符及之间的赋值取值。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-03-13
 *****************************************************************************/

#ifndef __W2X_COMMON_VARIABLE_H__
#define __W2X_COMMON_VARIABLE_H__

#include "exports.h"
#include <map>
#include <vector>


W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API Var
{
public:
	enum Type {
		kNull,
		kBool,
		kInt,
		kUint,
		kDword,
		kNumber,
		kString,
		kArray,
		kObject,
	};

	typedef std::vector<Var> Array;
	typedef std::map<Var, Var> Object;

public:
	Var(void);
	Var(bool _value);
	Var(INT _value);
	Var(UINT _value);
	Var(DWORD _value);
	Var(DOUBLE _value);
	Var(LPCTSTR _value);
	Var(const Var& _value);
	Var(const Array& _value);
	Var(const Object& _value);
	virtual ~Var(void);

	Var& operator =(bool _value);
	Var& operator =(INT _value);
	Var& operator =(UINT _value);
	Var& operator =(DWORD _value);
	Var& operator =(DOUBLE _value);
	Var& operator =(LPCTSTR _value);
	Var& operator =(const Var& _right);
	Var& operator =(const Array& _value);
	Var& operator =(const Object& _value);

	bool operator <(const Var& _right) const;
	bool operator ==(const Var& _right) const;
	Var& operator [](INT _index);
	Var& operator [](LPCTSTR _index);
	
	operator bool(void) const;
	operator INT(void) const;
	operator UINT(void) const;
	operator DWORD(void) const;
	operator DOUBLE(void) const;
	operator LPCTSTR(void) const;
	operator Array*(void) const;
	operator Object*(void) const;

	Type GetType(void) const;
	bool IsTypeOf(Type _type) const;
	UINT GetLength(void) const;

private:
	// 禁止隐式，防止重载冲突
	Var& operator [](const Var& _right);

	void InitValue(void);
	static Var& Copy(Var& _left, const Var& _right);

private:
	union Value {
		bool	bl;
		INT		nt;
		UINT	unt;
		DWORD	dw;
		double	dbl;
		LPTSTR	str;
		Array*	ary;
		Object*	obj;
	};

private:
	Type m_type;
	Value m_value;
	LPCTSTR m_num_str;
	UINT m_length;
};


W2X_NAME_SPACE_END


inline bool operator ==(bool _flag, const w2x::Var& _right)
{
	return _right.operator ==(w2x::Var(_flag));
}

inline bool operator ==(const w2x::Var& _left, LPCTSTR _right_str)
{
	return _left.operator ==(w2x::Var(_right_str));
}

inline bool operator !=(const w2x::Var& _left, INT _right_nt)
{
	return !_left.operator ==(w2x::Var(_right_nt));
}

inline bool operator !=(INT _left_nt, const w2x::Var& _right)
{
	return !_right.operator ==(w2x::Var(_left_nt));
}

inline bool operator !=(const w2x::Var& _left, LPCTSTR _right_str)
{
	return !_left.operator ==(w2x::Var(_right_str));
}


#endif /* __W2X_COMMON_VARIABLE_H__ */
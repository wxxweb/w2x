/******************************************************************************
 * 文件:		ref_ptr.h
 * 描述:		智能指针类 CRefPtr 用于对象的引用计数。使用该类来替代手动操作，即在引用计数对
 *			象中手动调用函数 AddRef() 和函数 Release()。通过自动操作，避免经常因忘记释
 *			放一个对象而造成的内存泄露。CRefPtr 对象所管理的引用计数对象需要实现成员函数
 *			AddRef() 和 Release()。最简单的办法是让用户自定义的类继承 IBase，并通过宏
 *			W2X_IMPLEMENT_REFCOUNTING 来实现引用计数相关函数接口，请参加 base.h。
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 创建:		2014-02-23
 * 修改:		2014-05-21
 ******************************************************************************/

#ifndef __W2X_COMMON_REF_PTR_H__
#define __W2X_COMMON_REF_PTR_H__

#include "macros.h"

W2X_NAME_SPACE_BEGIN


template <class T>
class CRefPtr 
{
public:
	CRefPtr(void) : m_ptr(NULL) {}

	CRefPtr(T* _ptr) : m_ptr(_ptr) {
		if (NULL != m_ptr) {
			m_ptr->w2xAddRef();
		}
	}

	CRefPtr(const CRefPtr<T>& _ref) : m_ptr(_ref.m_ptr) {
		if (NULL != m_ptr) {
			m_ptr->w2xAddRef();
		}
	}

	/** @note 暂时不为虚析构，请勿继承 */
	~CRefPtr(void) {
		if (NULL != m_ptr && 0 == m_ptr->w2xRelease()) { 
			m_ptr = NULL;
		}
	}

public:
	template <class X>
	operator X*(void) const {
		return static_cast<X*>(m_ptr);
	}

	operator T*(void) const {
		return m_ptr;
	}

	T* operator->(void) const {
		return m_ptr;
	}

	template <class X>
	bool operator==(X* _ptr) {
		return _ptr == static_cast<T*>(m_ptr);
	}

	bool operator==(T* _ptr) {
		return _ptr == m_ptr;
	}

	bool operator==(CRefPtr<T>& _ref) {
		return _ref.m_ptr == m_ptr;
	}

	template <class X>
	CRefPtr<T>& operator=(X* _ptr) {
		// 因为首先调用 AddRef() 所以自赋值是可行的
		if (NULL != _ptr) { 
			_ptr->w2xAddRef();
		}
		if (NULL != m_ptr) {
			m_ptr->w2xRelease();
		}
		m_ptr = static_cast<T*>(_ptr);
		return *this;
	}

	CRefPtr<T>& operator=(T* _ptr) {
		// 因为首先调用 AddRef() 所以自赋值是可行的
		if (NULL != _ptr) { 
			_ptr->w2xAddRef();
		}
		if (NULL != m_ptr) { 
			m_ptr->w2xRelease();
		}
		m_ptr = _ptr;
		return *this;
	}

	template <class X>
	CRefPtr<T>& operator=(const CRefPtr<X>& _ref) {
		return *this = _ref.get();
	}

	CRefPtr<T>& operator=(const CRefPtr<T>& _ref) {
		return *this = _ref.m_ptr;
	}

	T* get(void) const { 
		return m_ptr;
	}

	void swap(T** _ptr_ptr) {
		T* ptr = m_ptr;
		m_ptr = *_ptr_ptr;
		*_ptr_ptr = p;
	}

	void swap(CRefPtr<T>& _ref) {
		swap(&_ref.m_ptr);
	}

	int attach(void) {
		if (NULL == m_ptr) {
			return 0;
		}
		return m_ptr->w2xAddRef();
	}

	int detach(void) {
		if (NULL == m_ptr) {
			return 0;
		}
		int retval = m_ptr->w2xRelease();
		if (0 == retval) {
			m_ptr = NULL;
		}
		return retval;
	}

private:
	T* m_ptr;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_PTR_H__ */
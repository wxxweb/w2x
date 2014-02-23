/******************************************************************************
 * 文件：	ref_ptr.h
 * 描述：	智能指针类 CRefPtr 用于对象的引用计数。使用该类来替代手动操作，即在引用计数对
			象中手动调用函数 AddRef() 和函数 Release()。通过自动操作，避免经常因忘记释
			放一个对象而造成的内存泄露。CRefPtr 对象所管理的引用计数对象需要实现成员函数
			AddRef() 和 Release()。最简单的办法是让用户自定义的类继承 IBase，并通过宏
			W2X_IMPLEMENT_REFCOUNTING 来实现引用计数相关函数接口，请参加 base.h。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_REF_PTR_H__
#define __W2X_COMMON_REF_PTR_H__


W2X_NAME_SPACE_END


template <class T>
class CRefPtr 
{
public:
	CRefPtr(void) : m_ptr(NULL) {}

	CRefPtr(T* _ptr) : m_ptr(_ptr) {
		if (NULL != m_ptr) { 
			m_ptr->AddRef(); 
		}
	}

	CRefPtr(const CRefPtr<T>& _ref) : m_ptr(_ref.m_ptr) {
		if (NULL != m_ptr) {
			m_ptr->AddRef(); 
		}
	}

	~CRefPtr(void) {
		if (NULL != m_ptr) { 
			m_ptr->Release(); 
		}
	}

public:
	T* get(void) const { return m_ptr; }
	operator T*(void) const { return m_ptr; }
	T* operator->(void) const { return m_ptr; }

	CRefPtr<T>& operator=(T* _ptr) {
		// 因为首先调用 AddRef() 所以自赋值是可行的
		if (NULL != _ptr) { 
			_ptr->AddRef(); 
		}
		if (NULL != m_ptr) { 
			m_ptr->Release(); 
		}
		m_ptr = _ptr;
		return *this;
	}

	CRefPtr<T>& operator=(const CefRefPtr<T>& _ref) {
		return *this = _ref.m_ptr;
	}

	void swap(T** _ptr_ptr) {
		T* ptr = m_ptr;
		m_ptr = *_ptr_ptr;
		*_ptr_ptr = p;
	}

	void swap(CefRefPtr<T>& _ref) {
		swap(&_ref.m_ptr);
	}

private:
	T* const m_ptr;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_PTR_H__ */
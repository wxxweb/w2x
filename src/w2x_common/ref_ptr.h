/******************************************************************************
 * �ļ�:		ref_ptr.h
 * ����:		����ָ���� CRefPtr ���ڶ�������ü�����ʹ�ø���������ֶ��������������ü�����
 *			�����ֶ����ú��� AddRef() �ͺ��� Release()��ͨ���Զ����������⾭����������
 *			��һ���������ɵ��ڴ�й¶��CRefPtr ��������������ü���������Ҫʵ�ֳ�Ա����
 *			AddRef() �� Release()����򵥵İ취�����û��Զ������̳� IBase����ͨ����
 *			W2X_IMPLEMENT_REFCOUNTING ��ʵ�����ü�����غ����ӿڣ���μ� base.h��
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-02-23
 * �޸�:		2014-04-19
 ******************************************************************************/

#ifndef __W2X_COMMON_REF_PTR_H__
#define __W2X_COMMON_REF_PTR_H__

#include "exports.h"

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

	/** @note ��ʱ��Ϊ������������̳� */
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
		// ��Ϊ���ȵ��� AddRef() �����Ը�ֵ�ǿ��е�
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
		// ��Ϊ���ȵ��� AddRef() �����Ը�ֵ�ǿ��е�
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
		return *this = _ref.m_ptr;
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
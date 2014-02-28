/******************************************************************************
 * �ļ���	ref_ptr.h
 * ������	����ָ���� CRefPtr ���ڶ�������ü�����ʹ�ø���������ֶ��������������ü�����
			�����ֶ����ú��� AddRef() �ͺ��� Release()��ͨ���Զ����������⾭����������
			��һ���������ɵ��ڴ�й¶��CRefPtr ��������������ü���������Ҫʵ�ֳ�Ա����
			AddRef() �� Release()����򵥵İ취�����û��Զ������̳� IBase����ͨ����
			W2X_IMPLEMENT_REFCOUNTING ��ʵ�����ü�����غ����ӿڣ���μ� base.h��
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_REF_PTR_H__
#define __W2X_COMMON_REF_PTR_H__


W2X_NAME_SPACE_BEGIN


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
		// ��Ϊ���ȵ��� AddRef() �����Ը�ֵ�ǿ��е�
		if (NULL != _ptr) { 
			_ptr->AddRef(); 
		}
		if (NULL != m_ptr) { 
			m_ptr->Release(); 
		}
		m_ptr = _ptr;
		return *this;
	}

	CRefPtr<T>& operator=(const CRefPtr<T>& _ref) {
		return *this = _ref.m_ptr;
	}

	void swap(T** _ptr_ptr) {
		T* ptr = m_ptr;
		m_ptr = *_ptr_ptr;
		*_ptr_ptr = p;
	}

	void swap(CRefPtr<T>& _ref) {
		swap(&_ref.m_ptr);
	}

private:
	T* m_ptr;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_PTR_H__ */
/******************************************************************************
 * �ļ���	utility.h
 * ������	ʵ�õĺ����ͺ궨�壬��߱���Ч�ʺͽ�׳��.
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2013-05-24
 ******************************************************************************/

#ifndef W2X_COMMON_UTILITY_MACROS_H_
#define W2X_COMMON_UTILITY_MACROS_H_


/* ���� w2x �����ռ� */
#ifndef W2X_NAMESPACE_BEGIN
#define W2X_NAMESPACE_BEGIN namespace w2x {
#endif

#ifndef W2X_NAMESPACE_END
#define W2X_NAMESPACE_END }
#endif


/* ����Ϊ��һ�� NULL ������һ��ͷ�ļ������ڴ˶��� NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif /* NULL */


/* ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�Ӧ����� private: ��ʹ�� */
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(ClassName)	\
	private:								\
		(ClassName)(const (ClassName)&);	\
		void operator=(const (ClassName)&)	
#endif /* DISALLOW_COPY_AND_ASSIGN */


/* ��ȫ�ͷ�ջ�ڴ棬��ֹ delete �����Ұָ�� */
#ifndef SAFE_DELETE
#  ifdef _DEBUG
#	 define SAFE_DELETE(p) \
	 { if ((p)) { delete (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE(p) \
	 { if ((p)) { try { delete (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE */


/* ��ȫ�ͷ�ջ�ڴ棬��ֹ delete[] �����Ұָ�� */
#ifndef SAFE_DELETE_ARRAY
#  ifdef _DEBUG
#	 define SAFE_DELETE_ARRAY(p) \
	 { if ((p)) { delete[] (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE_ARRAY(p) \
	 { if ((p)) { try { delete[] (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE_ARRAY */


/* ��ȫ�ͷ�ջ�ڴ棬��ֹ free() �����Ұָ�� */
#ifndef SAFE_FREE
#  ifdef _DEBUG
#	 define SAFE_FREE(p) \
	 { if ((p)) { free((p)); (p) = NULL; } }
#  else
#	 define SAFE_FREE(p) \
	 { if ((p)) { try { free((p)); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_FREE */


/* ָ��һ������û�б�ʹ�� */
#ifndef UNUSED
#  ifdef _DEBUG
#    define UNUSED(x) (x)
#  else
#    define UNUSED(x) { (x)=(x); }
#  endif
#endif /* UNUSED */


#endif /* W2X_COMMON_UTILITY_MACROS_H_ */
/******************************************************************************
 * �ļ���	macros.h
 * ������	ʵ�õĺ����ͺ궨�壬��߱���Ч�ʺͽ�׳��.
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2013-05-24
 ******************************************************************************/

#ifndef __W2X_COMMON_UTILITY_MACROS_H__
#define __W2X_COMMON_UTILITY_MACROS_H__

//#include "debug.h"


/* ����Ϊ��һ�� NULL ������һ��ͷ�ļ������ڴ˶��� NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif /* NULL */


/* ָ��һ�������ǻص����� */
#ifndef CALLBACK
#  define CALLBACK
#endif /* CALLBACK */


/* ָ��һ��������������� */
#ifndef IN
#  define IN
#endif /* IN */


/* ָ��һ��������������� */
#ifndef OUT
#  define OUT
#endif /* OUT */

/* ָ��һ�������Ƕ�ȡ�� */
#ifndef GETTER
#  define GETTER
#endif /* GETTER */

/* ָ��һ�������������� */
#ifndef SETTER
#  define SETTER
#endif /* SETTER */


/* ָ��һ������û�б�ʹ�� */
#ifndef UNUSED
#  ifdef _DEBUG
#    define UNUSED(x) (x)
#  else
#    define UNUSED(x) { (x)=(x); }
#  endif
#endif /* UNUSED */

/* ����UNICODE����ʹ�ÿ��ַ�����ַ���C++�ַ��� */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


/* ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�Ӧ����� private: ��ʹ�� */
#ifndef W2X_DISALLOW_COPY_AND_ASSIGN
#define W2X_DISALLOW_COPY_AND_ASSIGN(ClassName)	\
	private:									\
		ClassName(const ClassName&);			\
		void operator =(const ClassName&);
#endif /* W2X_DISALLOW_COPY_AND_ASSIGN */


/* ��ȫ�ͷ�ջ�ڴ棬��ֹ delete �����Ұָ�� */
#ifndef SAFE_DELETE
#  ifdef _DEBUG
#	 define SAFE_DELETE(p) \
	 { if (NULL != (p)) { delete (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE(p) \
	 { if (NULL != (p)) { try { delete (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE */


/* ��ȫ�ͷ�ջ�ڴ棬��ֹ delete[] �����Ұָ�� */
#ifndef SAFE_DELETE_ARRAY
#  ifdef _DEBUG
#	 define SAFE_DELETE_ARRAY(p) \
	 { if (NULL != (p)) { delete[] (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE_ARRAY(p) \
	 { if (NULL != (p)) { try { delete[] (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE_ARRAY */


/* ��ȫ�ͷ�ջ�ڴ棬��ֹ free() �����Ұָ�� */
#ifndef SAFE_FREE
#  ifdef _DEBUG
#	 define SAFE_FREE(p) \
	 { if (NULL != (p)) { free((p)); (p) = NULL; } }
#  else
#	 define SAFE_FREE(p) \
	 { if (NULL != (p)) { try { free((p)); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_FREE */


/* �жϱ��ʽ e �Ľ���Ƿ�Ϊ�� */
#ifndef IS_TRUE
#  define IS_TRUE(e) (static_cast<bool>(e))
#endif


/* ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ִ�� if ����� */
#ifndef IF_FALSE_ASSERT
#  define IF_FALSE_ASSERT(e) if (IS_TRUE(e) ? false : (ASSERT(!("IF_FALSE: "#e)), true))
#endif


/* ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ִ�� if ����� */
#ifndef IF_NULL_ASSERT
#  define IF_NULL_ASSERT(p) IF_FALSE_ASSERT(NULL != (p))
#endif


/* ������ʽ e �Ľ��Ϊ��, ���� */
#ifndef IF_FALSE_RETURN
#  define IF_FALSE_RETURN(e) if (!IS_TRUE(e)) {return;}
#endif


/* ������ʽ e �Ľ��Ϊ��, ����ָ��ֵ r */
#ifndef IF_FALSE_RETURN_VALUE
#  define IF_FALSE_RETURN_VALUE(e,r) if (!IS_TRUE(e)) {return (r);}
#endif


/* ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ */
#ifndef IF_FALSE_ASSERT_RETURN
#  define IF_FALSE_ASSERT_RETURN(e) IF_FALSE_ASSERT(e) {return;}
#endif


/* ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ָ��ֵ r */
#ifndef IF_FALSE_ASSERT_RETURN_VALUE
#  define IF_FALSE_ASSERT_RETURN_VALUE(e,r) IF_FALSE_ASSERT(e) {return (r);}
#endif


/* ���ָ�� p ֵΪ��, ���� */
#ifndef IF_NULL_RETURN
#  define IF_NULL_RETURN(p) IF_FALSE_RETURN(NULL != (p))
#endif


/* ���ָ�� p ֵΪ��, ����ָ��ֵ r */
#ifndef IF_NULL_RETURN_VALUE
#  define IF_NULL_RETURN_VALUE(p,r) IF_FALSE_RETURN_VALUE(NULL != (p), (r))
#endif


/* ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ */
#ifndef IF_NULL_ASSERT_RETURN
#  define IF_NULL_ASSERT_RETURN(p) IF_FALSE_ASSERT_RETURN(NULL != (p))
#endif


/* ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ָ��ֵ r */
#ifndef IF_NULL_ASSERT_RETURN_VALUE
#  define IF_NULL_ASSERT_RETURN_VALUE(p,r) IF_FALSE_ASSERT_RETURN_VALUE(NULL != (p), r)
#endif


#ifdef _DEBUG
#  define W2X_OUTPUT_DEBUG_STR_A(str) \
	(NULL != str && 0 != str[0] && (::OutputDebugStringA(str), true))
#  define W2X_OUTPUT_DEBUG_STR_W(str) \
	(NULL != str && 0 != str[0] && (::OutputDebugStringW(str), true))
#else
#  define W2X_OUTPUT_DEBUG_STR_A(str) 0
#  define W2X_OUTPUT_DEBUG_STR_W(str) 0
#endif

#ifdef _UNICODE
#  define W2X_OUTPUT_DEBUG_STR(str) W2X_OUTPUT_DEBUG_STR_W(str)
#else
#  define W2X_OUTPUT_DEBUG_STR(str) W2X_OUTPUT_DEBUG_STR_A(str)
#endif


/* ���������ռ䲢����俪ʼ�ͽ���λ�� */
#ifndef W2X_DEFINE_NAME_SPACE_BEGIN
#  define W2X_DEFINE_NAME_SPACE_BEGIN(name) namespace name {
#endif
#ifndef W2X_DEFINE_NAME_SPACE_END
#  define W2X_DEFINE_NAME_SPACE_END(name) };
#endif


/* ���� w2x �����ռ� */
#ifndef W2X_NAME_SPACE_BEGIN
#  define W2X_NAME_SPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAME_SPACE_END
#  define W2X_NAME_SPACE_END };
#endif



#endif /* __W2X_COMMON_UTILITY_MACROS_H__ */
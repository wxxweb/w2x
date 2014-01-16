/******************************************************************************
 * �ļ���	utility.h
 * ������	ʵ�õĺ����ͺ궨�壬��߱���Ч�ʺͽ�׳��.
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2013-05-24
 ******************************************************************************/

#ifndef __W2X_COMMON_UTILITY_MACROS_H__
#define __W2X_COMMON_UTILITY_MACROS_H__

#include "debug.h"


/* ����Ϊ��һ�� NULL ������һ��ͷ�ļ������ڴ˶��� NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif /* NULL */


/* ָ��һ������û�б�ʹ�� */
#ifndef UNUSED
#  ifdef _DEBUG
#    define UNUSED(x) (x)
#  else
#    define UNUSED(x) { (x)=(x); }
#  endif
#endif /* UNUSED */


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


/* ������ʽ e �Ľ��Ϊ��, ����ָ��ֵ r */
#ifndef IF_FALSE_RETURN
#  define IF_FALSE_RETURN(e,r) if (!IS_TRUE(e)) {return (r);}
#endif


/* ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ r */
#ifndef IF_FALSE_ASSERT_RETURN
#  define IF_FALSE_ASSERT_RETURN(e,r) ASSERT(IS_TRUE(e)); IF_FALSE_RETURN((e), (r))
#endif


/* ���ָ�� p ֵΪ��, ���� r */
#ifndef IF_NULL_RETURN
#  define IF_NULL_RETURN(p,r) if (NULL == (p)) {return (r);}
#endif


/* ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ r */
#ifndef IF_NULL_ASSERT_RETURN
#  define IF_NULL_ASSERT_RETURN(p,r) ASSERT(NULL != (p)); IF_NULL_RETURN((p), (r))
#endif


#endif /* __W2X_COMMON_UTILITY_MACROS_H__ */
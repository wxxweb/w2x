/******************************************************************************
 * 文件：	macros.h
 * 描述：	实用的函数和宏定义，提高编码效率和健壮性.
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2013-05-24
 ******************************************************************************/

#ifndef __W2X_COMMON_UTILITY_MACROS_H__
#define __W2X_COMMON_UTILITY_MACROS_H__

#include "debug.h"


/* 不想为了一个 NULL 而包含一个头文件，顾在此定义 NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif /* NULL */


/* 指明一个函数是回调函数 */
#ifndef CALLBACK
#  define CALLBACK
#endif /* CALLBACK */


/* 指明一个参数是输入参数 */
#ifndef IN
#  define IN
#endif /* IN */


/* 指明一个参数是输出参数 */
#ifndef OUT
#  define OUT
#endif /* OUT */


/* 指明一个变量没有被使用 */
#ifndef UNUSED
#  ifdef _DEBUG
#    define UNUSED(x) (x)
#  else
#    define UNUSED(x) { (x)=(x); }
#  endif
#endif /* UNUSED */

/* 根据UNICODE决定使用宽字符或多字符版C++字符串 */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


/* 禁止使用拷贝构造函数和 operator= 赋值操作的宏应该类的 private: 中使用 */
#ifndef W2X_DISALLOW_COPY_AND_ASSIGN
#define W2X_DISALLOW_COPY_AND_ASSIGN(ClassName)	\
	private:									\
		ClassName(const ClassName&);			\
		void operator =(const ClassName&);
#endif /* W2X_DISALLOW_COPY_AND_ASSIGN */


/* 安全释放栈内存，防止 delete 后产生野指针 */
#ifndef SAFE_DELETE
#  ifdef _DEBUG
#	 define SAFE_DELETE(p) \
	 { if (NULL != (p)) { delete (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE(p) \
	 { if (NULL != (p)) { try { delete (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE */


/* 安全释放栈内存，防止 delete[] 后产生野指针 */
#ifndef SAFE_DELETE_ARRAY
#  ifdef _DEBUG
#	 define SAFE_DELETE_ARRAY(p) \
	 { if (NULL != (p)) { delete[] (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE_ARRAY(p) \
	 { if (NULL != (p)) { try { delete[] (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE_ARRAY */


/* 安全释放栈内存，防止 free() 后产生野指针 */
#ifndef SAFE_FREE
#  ifdef _DEBUG
#	 define SAFE_FREE(p) \
	 { if (NULL != (p)) { free((p)); (p) = NULL; } }
#  else
#	 define SAFE_FREE(p) \
	 { if (NULL != (p)) { try { free((p)); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_FREE */


/* 判断表达式 e 的结果是否为真 */
#ifndef IS_TRUE
#  define IS_TRUE(e) (static_cast<bool>(e))
#endif


/* 如果表达式 e 的结果为假, 弹出断言对话框, 并继续执行 if 语句体 */
#ifndef IF_FALSE_ASSERT
#  define IF_FALSE_ASSERT(e) if (IS_TRUE(e) ? false : (ASSERT(!("IF_FALSE: "#e)), true))
#endif


/* 如果指针 p 值为空,  弹出断言对话框, 并继续执行 if 语句体 */
#ifndef IF_NULL_ASSERT
#  define IF_NULL_ASSERT(p) IF_FALSE_ASSERT(NULL != (p))
#endif


/* 如果表达式 e 的结果为假, 返回 */
#ifndef IF_FALSE_RETURN
#  define IF_FALSE_RETURN(e) if (!IS_TRUE(e)) {return;}
#endif


/* 如果表达式 e 的结果为假, 返回指定值 r */
#ifndef IF_FALSE_RETURN_VALUE
#  define IF_FALSE_RETURN_VALUE(e,r) if (!IS_TRUE(e)) {return (r);}
#endif


/* 如果表达式 e 的结果为假, 弹出断言对话框, 并返回 */
#ifndef IF_FALSE_ASSERT_RETURN
#  define IF_FALSE_ASSERT_RETURN(e) IF_FALSE_ASSERT(e) {return;}
#endif


/* 如果表达式 e 的结果为假, 弹出断言对话框, 并返回指定值 r */
#ifndef IF_FALSE_ASSERT_RETURN_VALUE
#  define IF_FALSE_ASSERT_RETURN_VALUE(e,r) IF_FALSE_ASSERT(e) {return (r);}
#endif


/* 如果指针 p 值为空, 返回 */
#ifndef IF_NULL_RETURN
#  define IF_NULL_RETURN(p) IF_FALSE_RETURN(NULL != (p))
#endif


/* 如果指针 p 值为空, 返回指定值 r */
#ifndef IF_NULL_RETURN_VALUE
#  define IF_NULL_RETURN_VALUE(p,r) IF_FALSE_RETURN_VALUE(NULL != (p), (r))
#endif


/* 如果指针 p 值为空,  弹出断言对话框, 并返回 */
#ifndef IF_NULL_ASSERT_RETURN
#  define IF_NULL_ASSERT_RETURN(p) IF_FALSE_ASSERT_RETURN(NULL != (p))
#endif


/* 如果指针 p 值为空,  弹出断言对话框, 并返回指定值 r */
#ifndef IF_NULL_ASSERT_RETURN_VALUE
#  define IF_NULL_ASSERT_RETURN_VALUE(p,r) IF_FALSE_ASSERT_RETURN_VALUE(NULL != (p), r)
#endif


#ifdef _DEBUG
#  define W2X_OUT_PUT_DEBUG_STRING_A(str) \
	(NULL != str && 0 != str[0] && (::OutputDebugStringA(str), true))
#  define W2X_OUT_PUT_DEBUG_STRING_W(str) \
	(NULL != str && 0 != str[0] && (::OutputDebugStringW(str), true))
#else
#  define W2X_OUT_PUT_DEBUG_STRING_A(str) 0
#  define W2X_OUT_PUT_DEBUG_STRING_W(str) 0
#endif

#ifdef _UNICODE
#  define W2X_OUT_PUT_DEBUG_STRING(str) W2X_OUT_PUT_DEBUG_STRING_W(str)
#else
#  define W2X_OUT_PUT_DEBUG_STRING(str) W2X_OUT_PUT_DEBUG_STRING_A(str)
#endif


#endif /* __W2X_COMMON_UTILITY_MACROS_H__ */
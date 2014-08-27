/******************************************************************************
文件:		macros.h
描述:		定义一些实用的宏，提高编码效率和健壮性。
作者:		wu.xiongxing
邮箱:		wxxweb@gmail.com
创建:		2013-05-24
修改:		2014-07-02
 ******************************************************************************/

#ifndef __W2X_COMMON_UTILITY_MACROS_H__
#define __W2X_COMMON_UTILITY_MACROS_H__


/** 不想为了一个 NULL 而包含一个头文件，顾在此定义 NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif // NULL

/** 模仿 MFC 的 ASSERT */
#ifndef ASSERT
#  define ASSERT _ASSERT
#endif // ASSERT

/** 指明一个函数是回调函数 */
#ifndef CALLBACK
#  define CALLBACK
#endif // CALLBACK


/** 指明一个参数是输入参数 */
#ifndef IN
#  define IN
#endif


/** 指明一个参数是输出参数 */
#ifndef OUT
#  define OUT
#endif

/** 指明一个函数是读取器 */
#ifndef GETTER
#  define GETTER
#endif

/** 指明一个函数是设置器 */
#ifndef SETTER
#  define SETTER
#endif


/** 指明一个变量没有被使用 */
#ifndef UNUSED_ALWAYS
#  ifdef _DEBUG
#    define UNUSED_ALWAYS(x) (x)
#  else
#    define UNUSED_ALWAYS(x) ((x) = (x))
#  endif
#endif // UNUSED


/** 安全释放栈内存，防止 delete 后产生野指针 */
#ifndef SAFE_DELETE
#  ifdef _DEBUG
#	 define SAFE_DELETE(p) \
	 do{ if (NULL != (p)) { delete (p); (p) = NULL; } }while(0)
#  else
#	 define SAFE_DELETE(p) \
	 do{ if (NULL != (p)) { try{ delete (p); } catch(...){} (p) = NULL; } }while(0)
#  endif
#endif // SAFE_DELETE


/** 安全释放栈内存，防止 delete[] 后产生野指针 */
#ifndef SAFE_DELETE_ARRAY
#  ifdef _DEBUG
#	 define SAFE_DELETE_ARRAY(p) \
	 do{ if (NULL != (p)) { delete[] (p); (p) = NULL; } }while(0)
#  else
#	 define SAFE_DELETE_ARRAY(p) \
	 do{ if (NULL != (p)) { try{ delete[] (p); } catch(...){} (p) = NULL; } }while(0)
#  endif
#endif // SAFE_DELETE_ARRAY


/** 安全释放栈内存，防止 free() 后产生野指针 */
#ifndef SAFE_FREE
#  ifdef _DEBUG
#	 define SAFE_FREE(p) \
	 do{ if (NULL != (p)) { free((p)); (p) = NULL; } }while(0)
#  else
#	 define SAFE_FREE(p) \
	 do{ if (NULL != (p)) { try { free((p)); } catch(...) {} (p) = NULL; } }while(0)
#  endif
#endif // SAFE_FREE


/** 判断表达式 e 的结果是否为真 */
#ifndef IS_TRUE
#  define IS_TRUE(e) static_cast<bool>(e)
#endif


/** 如果表达式 e 的结果为假, 弹出断言对话框, 并继续执行 if 语句体 */
#ifndef IF_FALSE_ASSERT
#  define IF_FALSE_ASSERT(e) if (IS_TRUE(e) ? false : (ASSERT(!("IF_FALSE: "#e)), true))
#endif


/** 如果表达式 e 的结果为假, 返回 */
#ifndef IF_FALSE_RETURN
#  define IF_FALSE_RETURN(e) if (!IS_TRUE(e)) {return;}
#endif


/** 如果表达式 e 的结果为假, 返回指定值 r */
#ifndef IF_FALSE_RETURN_VALUE
#  define IF_FALSE_RETURN_VALUE(e,r) if (!IS_TRUE(e)) {return (r);}
#endif


/** 如果表达式 e 的结果为假, 弹出断言对话框, 并返回 */
#ifndef IF_FALSE_ASSERT_RETURN
#  define IF_FALSE_ASSERT_RETURN(e) IF_FALSE_ASSERT(e) {return;}
#endif


/** 如果表达式 e 的结果为假, 弹出断言对话框, 并返回指定值 r */
#ifndef IF_FALSE_ASSERT_RETURN_VALUE
#  define IF_FALSE_ASSERT_RETURN_VALUE(e,r) IF_FALSE_ASSERT(e) {return (r);}
#endif


/** 如果指针 p 值为空,  弹出断言对话框, 并继续执行 if 语句体 */
#ifndef IF_NULL_ASSERT
#  define IF_NULL_ASSERT(p) if ((NULL == (p)) ? (ASSERT(!("IF_NULL: "#p)), true) : false)
#endif


/** 如果指针 p 值为空, 返回 */
#ifndef IF_NULL_RETURN
#  define IF_NULL_RETURN(p) if (NULL == (p)) {return;}
#endif


/** 如果指针 p 值为空, 返回指定值 r */
#ifndef IF_NULL_RETURN_VALUE
#  define IF_NULL_RETURN_VALUE(p,r) if (NULL == (p)) {return (r);}
#endif


/** 如果指针 p 值为空,  弹出断言对话框, 并返回 */
#ifndef IF_NULL_ASSERT_RETURN
#  define IF_NULL_ASSERT_RETURN(p) IF_NULL_ASSERT(p) {return;}
#endif


/** 如果指针 p 值为空,  弹出断言对话框, 并返回指定值 r */
#ifndef IF_NULL_ASSERT_RETURN_VALUE
#  define IF_NULL_ASSERT_RETURN_VALUE(p,r) IF_NULL_ASSERT(p) {return (r);}
#endif


/** 根据UNICODE决定使用宽字符或多字符版C++字符串 */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


/** 构建用于回调的类函数对象，对象绑定不带参数的类成员函数 */
#ifndef W2X_CALLBACK_0
#  define W2X_CALLBACK_0(__selector__, __target__) \
	std::bind(&__selector__, __target__)
#endif


/** 构建用于回调的类函数对象，对象绑定带有1个参数的类成员函数 */
#ifndef W2X_CALLBACK_1
#  define W2X_CALLBACK_1(__selector__, __target__) \
	 std::bind(&__selector__, __target__, std::placeholders::_1)
#endif


/** 构建用于回调的类函数对象，对象绑定带有2个参数的类成员函数 */
#ifndef W2X_CALLBACK_2
#  define W2X_CALLBACK_2(__selector__, __target__) \
	std::bind(&__selector__, __target__, \
      std::placeholders::_1, std::placeholders::_2)
#endif


/** 禁止使用拷贝构造函数和 operator= 赋值操作的宏应该类的 private: 中使用 */
#ifndef W2X_DISALLOW_COPY_AND_ASSIGN
#define W2X_DISALLOW_COPY_AND_ASSIGN(ClassName)	\
	private: \
	ClassName(const ClassName&); \
	void operator =(const ClassName&);
#endif // W2X_DISALLOW_COPY_AND_ASSIGN


/** 定义命名空间并标记其开始和结束位置 */
#ifndef W2X_DEFINE_NAME_SPACE_BEGIN
#  define W2X_DEFINE_NAME_SPACE_BEGIN(name) namespace name {
#endif
#ifndef W2X_DEFINE_NAME_SPACE_END
#  define W2X_DEFINE_NAME_SPACE_END(name) };
#endif

#ifndef W2X_DEFINE_NS_BEGIN
#  define W2X_DEFINE_NS_BEGIN(name) namespace name {
#endif
#ifndef W2X_DEFINE_NS_END
#  define W2X_DEFINE_NS_END(name) };
#endif


/** 定义 w2x 命名空间 */
#ifndef W2X_NAME_SPACE_BEGIN
#  define W2X_NAME_SPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAME_SPACE_END
#  define W2X_NAME_SPACE_END };
#endif

#ifndef W2X_NS_BEGIN
#  define W2X_NS_BEGIN namespace w2x {
#endif
#ifndef W2X_NS_END
#  define W2X_NS_END };
#endif


/** 使用 w2x 命名空间 */
#ifndef USING_NS_W2X
#  define USING_NS_W2X using namespace w2x
#endif


/** 定义 C 语言方式导出宏 */
#ifndef __cplusplus
#  ifndef W2X_EXTERN_C
#    define W2X_EXTERN_C
#  endif
#else
#  ifndef W2X_EXTERN_C
#    define W2X_EXTERN_C extern "C"
#  endif
#endif


/** 定义模块的导入导出宏 */
#ifdef W2X_COMMON_BUILD_STATIC // 构建静态库时定义该宏，否则为动态库
#  ifndef W2X_COMMON_API
#    define W2X_COMMON_API
#  endif
#else // !W2X_COMMON_BUILD_STATIC
#  ifdef W2X_COMMON_EXPORTS // 需要从本动态库中导出符号时定义该宏
#    ifndef W2X_COMMON_API
#      define W2X_COMMON_API __declspec(dllexport)
#    endif
#  else // !W2X_COMMON_EXPORTS
#    ifndef W2X_COMMON_API
#      define W2X_COMMON_API __declspec(dllimport)
#    endif
#  endif // W2X_COMMON_EXPORTS
#endif // W2X_COMMON_BUILD_STATIC


#endif /* __W2X_COMMON_UTILITY_MACROS_H__ */
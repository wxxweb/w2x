/******************************************************************************
 * 文件：	utility.h
 * 描述：	实用的函数和宏定义，提高编码效率和健壮性.
 * 作者：	wu.xiongxing
 * 时间：	2013-05-24
 ******************************************************************************/

#ifndef W2X_COMMON_UTILITY_MACROS_H_
#define W2X_COMMON_UTILITY_MACROS_H_


/* 定义 w2x 命名空间 */
#ifndef W2X_NAMESPACE_BEGIN
#define W2X_NAMESPACE_BEGIN namespace w2x {
#endif

#ifndef W2X_NAMESPACE_END
#define W2X_NAMESPACE_END }
#endif


/* 不想为了一个 NULL 而包含一个头文件，顾在此定义 NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif /* NULL */


/* 禁止使用拷贝构造函数和 operator= 赋值操作的宏应该类的 private: 中使用 */
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(ClassName)	\
	private:								\
		(ClassName)(const (ClassName)&);	\
		void operator=(const (ClassName)&)	
#endif /* DISALLOW_COPY_AND_ASSIGN */


/* 安全释放栈内存，防止 delete 后产生野指针 */
#ifndef SAFE_DELETE
#  ifdef _DEBUG
#	 define SAFE_DELETE(p) \
	 { if ((p)) { delete (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE(p) \
	 { if ((p)) { try { delete (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE */


/* 安全释放栈内存，防止 delete[] 后产生野指针 */
#ifndef SAFE_DELETE_ARRAY
#  ifdef _DEBUG
#	 define SAFE_DELETE_ARRAY(p) \
	 { if ((p)) { delete[] (p); (p) = NULL; } }	
#  else
#	 define SAFE_DELETE_ARRAY(p) \
	 { if ((p)) { try { delete[] (p); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_DELETE_ARRAY */


/* 安全释放栈内存，防止 free() 后产生野指针 */
#ifndef SAFE_FREE
#  ifdef _DEBUG
#	 define SAFE_FREE(p) \
	 { if ((p)) { free((p)); (p) = NULL; } }
#  else
#	 define SAFE_FREE(p) \
	 { if ((p)) { try { free((p)); } catch(...) {} (p) = NULL; } }
#  endif
#endif /* SAFE_FREE */


/* 指明一个变量没有被使用 */
#ifndef UNUSED
#  ifdef _DEBUG
#    define UNUSED(x) (x)
#  else
#    define UNUSED(x) { (x)=(x); }
#  endif
#endif /* UNUSED */


#endif /* W2X_COMMON_UTILITY_MACROS_H_ */
/******************************************************************************
�ļ�:		macros.h
����:		����һЩʵ�õĺ꣬��߱���Ч�ʺͽ�׳�ԡ�
����:		wu.xiongxing
����:		wxxweb@gmail.com
����:		2013-05-24
�޸�:		2014-07-02
 ******************************************************************************/

#ifndef __W2X_COMMON_UTILITY_MACROS_H__
#define __W2X_COMMON_UTILITY_MACROS_H__


/** ����Ϊ��һ�� NULL ������һ��ͷ�ļ������ڴ˶��� NULL */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL	0
#  else
#    define NULL ((void *)0)
#  endif
#endif // NULL

/** ģ�� MFC �� ASSERT */
#ifndef ASSERT
#  define ASSERT _ASSERT
#endif // ASSERT

/** ָ��һ�������ǻص����� */
#ifndef CALLBACK
#  define CALLBACK
#endif // CALLBACK


/** ָ��һ��������������� */
#ifndef IN
#  define IN
#endif


/** ָ��һ��������������� */
#ifndef OUT
#  define OUT
#endif

/** ָ��һ�������Ƕ�ȡ�� */
#ifndef GETTER
#  define GETTER
#endif

/** ָ��һ�������������� */
#ifndef SETTER
#  define SETTER
#endif


/** ָ��һ������û�б�ʹ�� */
#ifndef UNUSED_ALWAYS
#  ifdef _DEBUG
#    define UNUSED_ALWAYS(x) (x)
#  else
#    define UNUSED_ALWAYS(x) ((x) = (x))
#  endif
#endif // UNUSED


/** ��ȫ�ͷ�ջ�ڴ棬��ֹ delete �����Ұָ�� */
#ifndef SAFE_DELETE
#  ifdef _DEBUG
#	 define SAFE_DELETE(p) \
	 do{ if (NULL != (p)) { delete (p); (p) = NULL; } }while(0)
#  else
#	 define SAFE_DELETE(p) \
	 do{ if (NULL != (p)) { try{ delete (p); } catch(...){} (p) = NULL; } }while(0)
#  endif
#endif // SAFE_DELETE


/** ��ȫ�ͷ�ջ�ڴ棬��ֹ delete[] �����Ұָ�� */
#ifndef SAFE_DELETE_ARRAY
#  ifdef _DEBUG
#	 define SAFE_DELETE_ARRAY(p) \
	 do{ if (NULL != (p)) { delete[] (p); (p) = NULL; } }while(0)
#  else
#	 define SAFE_DELETE_ARRAY(p) \
	 do{ if (NULL != (p)) { try{ delete[] (p); } catch(...){} (p) = NULL; } }while(0)
#  endif
#endif // SAFE_DELETE_ARRAY


/** ��ȫ�ͷ�ջ�ڴ棬��ֹ free() �����Ұָ�� */
#ifndef SAFE_FREE
#  ifdef _DEBUG
#	 define SAFE_FREE(p) \
	 do{ if (NULL != (p)) { free((p)); (p) = NULL; } }while(0)
#  else
#	 define SAFE_FREE(p) \
	 do{ if (NULL != (p)) { try { free((p)); } catch(...) {} (p) = NULL; } }while(0)
#  endif
#endif // SAFE_FREE


/** �жϱ��ʽ e �Ľ���Ƿ�Ϊ�� */
#ifndef IS_TRUE
#  define IS_TRUE(e) static_cast<bool>(e)
#endif


/** ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ִ�� if ����� */
#ifndef IF_FALSE_ASSERT
#  define IF_FALSE_ASSERT(e) if (IS_TRUE(e) ? false : (ASSERT(!("IF_FALSE: "#e)), true))
#endif


/** ������ʽ e �Ľ��Ϊ��, ���� */
#ifndef IF_FALSE_RETURN
#  define IF_FALSE_RETURN(e) if (!IS_TRUE(e)) {return;}
#endif


/** ������ʽ e �Ľ��Ϊ��, ����ָ��ֵ r */
#ifndef IF_FALSE_RETURN_VALUE
#  define IF_FALSE_RETURN_VALUE(e,r) if (!IS_TRUE(e)) {return (r);}
#endif


/** ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ */
#ifndef IF_FALSE_ASSERT_RETURN
#  define IF_FALSE_ASSERT_RETURN(e) IF_FALSE_ASSERT(e) {return;}
#endif


/** ������ʽ e �Ľ��Ϊ��, �������ԶԻ���, ������ָ��ֵ r */
#ifndef IF_FALSE_ASSERT_RETURN_VALUE
#  define IF_FALSE_ASSERT_RETURN_VALUE(e,r) IF_FALSE_ASSERT(e) {return (r);}
#endif


/** ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ִ�� if ����� */
#ifndef IF_NULL_ASSERT
#  define IF_NULL_ASSERT(p) if ((NULL == (p)) ? (ASSERT(!("IF_NULL: "#p)), true) : false)
#endif


/** ���ָ�� p ֵΪ��, ���� */
#ifndef IF_NULL_RETURN
#  define IF_NULL_RETURN(p) if (NULL == (p)) {return;}
#endif


/** ���ָ�� p ֵΪ��, ����ָ��ֵ r */
#ifndef IF_NULL_RETURN_VALUE
#  define IF_NULL_RETURN_VALUE(p,r) if (NULL == (p)) {return (r);}
#endif


/** ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ */
#ifndef IF_NULL_ASSERT_RETURN
#  define IF_NULL_ASSERT_RETURN(p) IF_NULL_ASSERT(p) {return;}
#endif


/** ���ָ�� p ֵΪ��,  �������ԶԻ���, ������ָ��ֵ r */
#ifndef IF_NULL_ASSERT_RETURN_VALUE
#  define IF_NULL_ASSERT_RETURN_VALUE(p,r) IF_NULL_ASSERT(p) {return (r);}
#endif


/** ����UNICODE����ʹ�ÿ��ַ�����ַ���C++�ַ��� */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


/** �������ڻص����ຯ�����󣬶���󶨲������������Ա���� */
#ifndef W2X_CALLBACK_0
#  define W2X_CALLBACK_0(__selector__, __target__) \
	std::bind(&__selector__, __target__)
#endif


/** �������ڻص����ຯ�����󣬶���󶨴���1�����������Ա���� */
#ifndef W2X_CALLBACK_1
#  define W2X_CALLBACK_1(__selector__, __target__) \
	 std::bind(&__selector__, __target__, std::placeholders::_1)
#endif


/** �������ڻص����ຯ�����󣬶���󶨴���2�����������Ա���� */
#ifndef W2X_CALLBACK_2
#  define W2X_CALLBACK_2(__selector__, __target__) \
	std::bind(&__selector__, __target__, \
      std::placeholders::_1, std::placeholders::_2)
#endif


/** ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�Ӧ����� private: ��ʹ�� */
#ifndef W2X_DISALLOW_COPY_AND_ASSIGN
#define W2X_DISALLOW_COPY_AND_ASSIGN(ClassName)	\
	private: \
	ClassName(const ClassName&); \
	void operator =(const ClassName&);
#endif // W2X_DISALLOW_COPY_AND_ASSIGN


/** ���������ռ䲢����俪ʼ�ͽ���λ�� */
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


/** ���� w2x �����ռ� */
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


/** ʹ�� w2x �����ռ� */
#ifndef USING_NS_W2X
#  define USING_NS_W2X using namespace w2x
#endif


/** ���� C ���Է�ʽ������ */
#ifndef __cplusplus
#  ifndef W2X_EXTERN_C
#    define W2X_EXTERN_C
#  endif
#else
#  ifndef W2X_EXTERN_C
#    define W2X_EXTERN_C extern "C"
#  endif
#endif


/** ����ģ��ĵ��뵼���� */
#ifdef W2X_COMMON_BUILD_STATIC // ������̬��ʱ����ú꣬����Ϊ��̬��
#  ifndef W2X_COMMON_API
#    define W2X_COMMON_API
#  endif
#else // !W2X_COMMON_BUILD_STATIC
#  ifdef W2X_COMMON_EXPORTS // ��Ҫ�ӱ���̬���е�������ʱ����ú�
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
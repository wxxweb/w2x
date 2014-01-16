/*****************************************************************************
�ļ�����		exports.h
������		���� common ģ��ĵ��뵼����.
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2013.12.10
******************************************************************************/

#ifndef __W2X_COMMON_EXPORTS_H__
#define __W2X_COMMON_EXPORTS_H__


#ifdef W2X_COMMON_EXPORTS
#  ifndef  W2X_COMMON_API
#    define W2X_COMMON_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_COMMON_API
#    define W2X_COMMON_API __declspec(dllimport)
#  endif
#endif


/* ���� w2x �����ռ� */
#ifndef W2X_NAMESPACE_BEGIN
#  define W2X_NAMESPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAMESPACE_END
#  define W2X_NAMESPACE_END };
#endif


#endif /* __W2X_COMMON_EXPORTS_H__ */
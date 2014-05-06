/*****************************************************************************
 * �ļ�:		exports.h
 * ����:		���� common ģ��ĵ��뵼����.
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2013-12-10
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


/** ���������ռ䲢����俪ʼ�ͽ���λ�� */
#ifndef W2X_DEFINE_NAME_SPACE_BEGIN
#  define W2X_DEFINE_NAME_SPACE_BEGIN(name) namespace name {
#endif
#ifndef W2X_DEFINE_NAME_SPACE_END
#  define W2X_DEFINE_NAME_SPACE_END(name) };
#endif


/** ���� w2x �����ռ� */
#ifndef W2X_NAME_SPACE_BEGIN
#  define W2X_NAME_SPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAME_SPACE_END
#  define W2X_NAME_SPACE_END };
#endif


/** ʹ�� w2x �����ռ� */
#ifndef USING_NS_W2X
#  define USING_NS_W2X using namespace w2x
#endif


#endif /* __W2X_COMMON_EXPORTS_H__ */
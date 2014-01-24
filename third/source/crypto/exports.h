/*****************************************************************************
 * ������	exports.h
 * ������	���� crypto ģ��ĵ��뵼����.
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2013-01-25
 *****************************************************************************/

#ifndef __W2X_CRYPTO_EXPORTS_H__
#define __W2X_CRYPTO_EXPORTS_H__


#ifdef W2X_CRYPTO_EXPORTS
#  ifndef  W2X_CRYPTO_API
#    define W2X_CRYPTO_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_CRYPTO_API
#    define W2X_CRYPTO_API __declspec(dllimport)
#  endif
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


#endif /* __W2X_CRYPTO_EXPORTS_H__ */
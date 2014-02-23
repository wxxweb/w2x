/*****************************************************************************
�ļ�����		exports.h
������		���� flash ģ��ĵ��뵼����.
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2014-02-17
******************************************************************************/

#ifndef __W2X_FLASH_EXPORTS_H__
#define __W2X_FLASH_EXPORTS_H__


#ifdef W2X_FLASH_EXPORTS
#  ifndef  W2X_FLASH_API
#    define W2X_FLASH_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_FLASH_API
#    define W2X_FLASH_API __declspec(dllimport)
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


#ifdef W2X_FLASH_EXPORTS
	W2X_NAME_SPACE_BEGIN
	W2X_DEFINE_NAME_SPACE_BEGIN(ui)
	W2X_DEFINE_NAME_SPACE_END(ui)
	W2X_NAME_SPACE_END
	namespace internal = w2x::ui;
#endif


#endif /* __W2X_FLASH_EXPORTS_H__ */
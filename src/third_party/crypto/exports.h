/*****************************************************************************
 * 文名：	exports.h
 * 描述：	定义 crypto 模块的导入导出宏.
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2013-01-25
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


/* 定义命名空间并标记其开始和结束位置 */
#ifndef W2X_DEFINE_NAME_SPACE_BEGIN
#  define W2X_DEFINE_NAME_SPACE_BEGIN(name) namespace name {
#endif
#ifndef W2X_DEFINE_NAME_SPACE_END
#  define W2X_DEFINE_NAME_SPACE_END(name) };
#endif


/* 定义 w2x 命名空间 */
#ifndef W2X_NAME_SPACE_BEGIN
#  define W2X_NAME_SPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAME_SPACE_END
#  define W2X_NAME_SPACE_END };
#endif


#endif /* __W2X_CRYPTO_EXPORTS_H__ */